﻿#ifndef SPIDERWEB_REFLECT_XML_H
#define SPIDERWEB_REFLECT_XML_H

#include <memory>
#include <string>
#include <type_traits>

#include "spiderweb/reflect/enum_reflect.h"

namespace spiderweb {
namespace reflect {

template <typename XmlValueType, typename ValueType>
struct XmlMeta;

// test is bool,int,floating point...
template <typename T>
struct IsSimpleT {
  constexpr static bool value = std::is_integral<T>::value || std::is_floating_point<T>::value;
};

namespace detail {

template <typename XmlValue>
inline bool ReadTagImpl(XmlValue & /*node*/, const char * /*name*/, PlaceHolderValue & /*value*/) {
  return true;
}

template <typename XmlValue>
inline void WriteTagImpl(XmlValue &node, const char *name, const PlaceHolderValue &value) {
}

template <typename XmlValue, typename ValueType>
inline bool ReadTagImpl(XmlValue &node, const char *name, ValueType &value) {
  // we If it is a null pointer, we consider reading data from the node itself
  if (!name) {
    if (node.HasValue()) {
      reflect::XmlMeta<XmlValue, ValueType>::Read(node, value);
      return true;
    }
  }

  auto child = node.FindChild(name);
  if (child.HasValue()) {
    reflect::XmlMeta<XmlValue, ValueType>::Read(child, value);
    return true;
  }
  return false;
}

template <typename XmlValue, typename ValueType>
inline void WriteTagImpl(XmlValue &node, const char *name, const ValueType &value) {
  using ReturnType = decltype(node.CreateChild(name));

  ReturnType new_child = node.CreateChild(name);
  reflect ::XmlMeta<XmlValue, ValueType>::Write(new_child, value);
}

template <typename XmlValue, typename ValueType,
          template <typename Elem, typename = std::allocator<Elem>> class Array>
inline void WriteTagImpl(XmlValue &node, const char *name, const Array<ValueType> &values) {
  using ReturnType = decltype(node.CreateChild(name));
  for (const auto &value : values) {
    ReturnType new_child = node.CreateChild(name);
    reflect ::XmlMeta<XmlValue, ValueType>::Write(new_child, value);
  }
}

template <typename XmlValue, typename ValueType,
          template <typename Elem, typename = std::allocator<Elem>> class Array>
inline bool ReadTagImpl(XmlValue &node, const char *name, Array<ValueType> &values) {
  using MetaType = reflect::XmlMeta<XmlValue, ValueType>;

  node.ForEachChilds(name, [&](const XmlValue &child) {
    ValueType result{};
    MetaType::Read(child, result);
    values.push_back(std::move(result));
  });
  return true;
}

template <typename XmlValue, typename ValueType>
inline void WriteTagImpl(XmlValue &node, const char *name,
                         const std::shared_ptr<ValueType> &value) {
  using ReturnType = decltype(node.CreateChild(name));

  /**
   * @brief We have to decide whether we need to create new child nodes before
   *
   * writing. For pointers, if the pointer is null, then there is no need to
   *
   * create a new one
   */
  if (!value) {
    return;
  }

  ReturnType new_child = node.CreateChild(name);
  reflect ::XmlMeta<XmlValue, std::shared_ptr<ValueType>>::Write(new_child, value);
}
}  // namespace detail

/**
 * @brief for simple type, such as int,uint,float, bool
 */
template <typename XmlValueType, typename ValueType>
struct XmlMeta {
  template <typename =
                typename std::enable_if<IsSimpleT<ValueType>::value ||
                                        std::is_constructible<std::string, ValueType>::value>::type>
  inline static void Read(const XmlValueType &node, ValueType &value) {
    if (node.HasValue()) {
      node.GetValue(value);
    }
  }

  template <typename =
                typename std::enable_if<IsSimpleT<ValueType>::value ||
                                        std::is_constructible<std::string, ValueType>::value>::type>
  inline static void Write(XmlValueType &node, const ValueType &value) {
    node.SetValue(value);
  }
};

/**
 * @brief for std::shared_ptr<ValueType>
 */
template <typename XmlValueType, typename ValueType>
struct XmlMeta<XmlValueType, std::shared_ptr<ValueType>> {
  inline static void Read(XmlValueType &node, std::shared_ptr<ValueType> &value) {
    using MetaType = reflect::XmlMeta<XmlValueType, ValueType>;

    if (!value) {
      value = std::make_shared<ValueType>();
    }
    MetaType::Read(node, *value.get());
  }

  inline static void Write(XmlValueType &node, const std::shared_ptr<ValueType> &value) {
    using MetaType = reflect::XmlMeta<XmlValueType, ValueType>;

    MetaType::Write(node, *value.get());
  }
};

template <typename XmlValueType, typename BuilderType>
class XmlReader {
 public:
  XmlReader(const char *xml, std::size_t size) : builder_(xml, size) {
  }

  ~XmlReader() = default;

  /**
   * @brief read a pathes of tag into instance of T,
   *
   * @note the pathes is reversed:
   *
   * eg: if we want read root/subtree1/target
   *
   * we must call the methid like this, ReadInto("target", output,
   * "root", "subtree1");
   */
  template <typename T, typename... Paths>
  void Read(const char *target_tag, T &out, Paths... paths) const {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = builder_.Root();
    auto child = FindChild(root, paths...);
    auto found = std::move(child.FindChild(target_tag));

    MetaType::Read(found, out);
  }

  /**
   * @brief read a pathes of tag into instance of T,
   *
   * but we can specify a index for read.
   *
   * if the index is invalid, it will read nothing
   *
   * @note the pathes is reversed:
   *
   * eg: if we want read root/subtree1/target
   *
   * we must call the methid like this, ReadInto("target", output, index,
   *
   * "root", "subtree1");
   */
  template <typename T, typename... Pathes>
  void Read(const char *target_tag, T &out, int index, Pathes... pathes) const {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = builder_.Root();
    auto root_child = std::move(FindChild(root, pathes...));

    int  n = 0;
    bool done = false;
    root_child.ForEachChilds(target_tag, [&](const XmlValueType &child) {
      if (n++ != index) {
        return;
      }
      if (done) {
        return;
      }

      done = true;
      MetaType::Read(child, out);
    });
  }

  /**
   * @brief read a pathes of tags into instance of Array<T>,
   *
   * @note the pathes is reversed:
   *
   * eg: if we want read root/subtree1/target
   *
   * we must call the methid like this, ReadInto("target", output,
   *
   * "root", "subtree1");
   */
  template <typename T, template <typename Elem, typename = std::allocator<Elem>> class Array,
            typename... Pathes>
  void Read(const char *target_tag, Array<T> &out, Pathes... pathes) const {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = builder_.Root();
    auto root_child = std::move(FindChild(root, pathes...));

    root_child.ForEachChilds(target_tag, [&](const XmlValueType &child) {
      T result{};
      MetaType::Read(child, result);
      out.push_back(std::move(result));
    });
  }

  template <typename T>
  void Read(T &out) const {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = builder_.Root();
    auto child = root.FirstChild();

    MetaType::Read(child, out);
  }

 private:
  inline XmlValueType &FindChild(XmlValueType &node) const {
    return node;
  }

  inline XmlValueType FindChild(XmlValueType &node) {
    return std::move(node);
  }

  inline XmlValueType FindChild(XmlValueType &node, const char *tag) const {
    return node.FindChild(tag);
  }

  template <typename... Paths>
  XmlValueType FindChild(XmlValueType &node, const char *target_tag, Paths... paths) const {
    auto first = FindChild(node, target_tag);
    return FindChild(first, paths...);
  }

  BuilderType builder_;
};

template <typename XmlValueType, typename BuilderType>
class XmlWriter {
 public:
  using StringType = decltype(std::declval<XmlValueType>().ToString());
  XmlWriter() = default;

  ~XmlWriter() = default;

  template <typename T, typename... Pathes>
  void Write(const char *name, const T &value, Pathes... pathes) {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = std::move(builder_.Root());
    root = std::move(CreateChild(root, pathes...));
    root = std::move(root.CreateChild(name));

    MetaType::Write(root, value);
  }

  template <typename T, template <typename Elem, typename = std::allocator<Elem>> class Array,
            typename... Pathes>
  void Write(const char *name, const Array<T> &values, Pathes... pathes) {
    using MetaType = reflect::XmlMeta<XmlValueType, T>;

    auto root = builder_.Root();
    root = CreateChild(root, pathes...);

    for (const auto &value : values) {
      auto node = root.CreateChild(name);
      MetaType::Write(node, value);
    }
  }

  XmlValueType CreateChild(XmlValueType &node, const char *name) {
    return node.CreateChild(name);
  }

  XmlValueType &CreateChild(XmlValueType &node) {
    return node;
  }

  template <typename... Pathes>
  XmlValueType CreateChild(XmlValueType &node, const char *name, Pathes... pathes) {
    auto child = node.CreateChild(name);
    return CreateChild(child, pathes...);
  }

  StringType ToString() const {
    return builder_.Root().ToString();
  }

 private:
  BuilderType builder_;
};

#define REFLECT_XML_TYPEDEF_VALUE_TYPE(member, name) \
  using member##_type = decltype(struct_type::member);

#define REFLECT_XML_ATTR(...) __VA_ARGS__

#define REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_0

#define REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_1(member) \
  if (XmlValue::HasAtrributeValue(node, #member)) {     \
    node.GetAttribute(#member, result.member);          \
  }

#define REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_2(member, name) \
  if (XmlValue::HasAtrributeValue(node, name)) {              \
    node.GetAttribute(name, result.member);                   \
  }

#define REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_3(member, name, type) \
  if (XmlValue::HasAtrributeValue(node, name)) {                    \
    type value;                                                     \
    node.GetAttribute(name, value);                                 \
    result.member = reflect::MapFrom(value, result.member);         \
  }

#define REFLECT_XML_WRITE_ATTRIBUTE_CODE_BLOCK_1(member) node.SetAttribute(#member, result.member);

#define REFLECT_XML_WRITE_ATTRIBUTE_CODE_BLOCK_2(member, name) \
  node.SetAttribute(name, result.member);

#define REFLECT_XML_WRITE_ATTRIBUTE_CODE_BLOCK_3(member, name, type) \
  {                                                                  \
    type value;                                                      \
    reflect::MapTo(result.member, value);                            \
    node.SetAttribute(name, value);                                  \
  }

#define REFLECT_XML_READ_TAG_CODE_BLOCK_0 ddd

#define REFLECT_XML_READ_TAG_CODE_BLOCK_1(member) \
  reflect::detail::ReadTagImpl(node, #member, result.member);

#define REFLECT_XML_READ_TAG_CODE_BLOCK_2(member, name) \
  reflect::detail::ReadTagImpl(node, name, result.member);

#define REFLECT_XML_READ_TAG_CODE_BLOCK_3(member, name, type)            \
  {                                                                      \
    using XmlStorageType = type;                                         \
    XmlStorageType value{};                                              \
    bool           ok = reflect::detail::ReadTagImpl(node, name, value); \
    if (ok) {                                                            \
      result.member = reflect::MapFrom(value, result.member);            \
    }                                                                    \
  }

#define REFLECT_XML_WRITE_TAG_CODE_BLOCK_1(member) \
  reflect::detail::WriteTagImpl(node, #member, result.member);

#define REFLECT_XML_WRITE_TAG_CODE_BLOCK_2(member, name) \
  reflect::detail::WriteTagImpl(node, name, result.member);

#define REFLECT_XML_WRITE_TAG_CODE_BLOCK_3(member, name, type) \
  {                                                            \
    type value;                                                \
    reflect::MapTo(result.member, value);                      \
    reflect::detail::WriteTagImpl(node, name, value);          \
  }

#define REFLECT_XML_READ_ATTRIBUTE(apair)                                     \
  VISIT_STRUCT_CONCAT(REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_,                 \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_READ_TAG(apair)                                           \
  VISIT_STRUCT_CONCAT(REFLECT_XML_READ_TAG_CODE_BLOCK_,                       \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_WRITE_ATTRIBUTE(apair)                                    \
  VISIT_STRUCT_CONCAT(REFLECT_XML_WRITE_ATTRIBUTE_CODE_BLOCK_,                \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_WRITE_TAG(apair)                                          \
  VISIT_STRUCT_CONCAT(REFLECT_XML_WRITE_TAG_CODE_BLOCK_,                      \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML(Type, ATTRS, ...)                              \
  namespace spiderweb {                                            \
  namespace reflect {                                              \
  template <typename T>                                            \
  struct XmlMeta<T, Type> {                                        \
    using XmlValue = T;                                            \
    using struct_type = Type;                                      \
    static void Read(const XmlValue &node, struct_type &result) {  \
      MACRO_MAP(REFLECT_XML_READ_ATTRIBUTE, ATTRS)                 \
      MACRO_MAP(REFLECT_XML_READ_TAG, __VA_ARGS__)                 \
    }                                                              \
    static void Write(XmlValue &node, const struct_type &result) { \
      MACRO_MAP(REFLECT_XML_WRITE_ATTRIBUTE, ATTRS)                \
      MACRO_MAP(REFLECT_XML_WRITE_TAG, __VA_ARGS__)                \
    }                                                              \
  };                                                               \
  }                                                                \
  }

#define REFLECT_XML_READ_ATTRIBUTE_SIMPLE(apair)                              \
  VISIT_STRUCT_CONCAT(REFLECT_XML_READ_ATTRIBUTE_CODE_BLOCK_,                 \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_READ_TAG_SIMPLE(apair)                                    \
  VISIT_STRUCT_CONCAT(REFLECT_XML_READ_TAG_CODE_BLOCK_,                       \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_WRITE_ATTRIBUTE_SIMPLE(apair)                             \
  VISIT_STRUCT_CONCAT(REFLECT_XML_WRITE_ATTRIBUTE_CODE_BLOCK_,                \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_WRITE_TAG_SIMPLE(apair)                                   \
  VISIT_STRUCT_CONCAT(REFLECT_XML_WRITE_TAG_CODE_BLOCK_,                      \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(apair))) \
  apair

#define REFLECT_XML_SIMPLE(Type, ATTRS, ...)                       \
  namespace spiderweb {                                            \
  namespace reflect {                                              \
  template <typename T>                                            \
  struct XmlMeta<T, Type> {                                        \
    using XmlValue = T;                                            \
    using struct_type = Type;                                      \
    static void Read(const XmlValue &node, struct_type &result) {  \
      MACRO_MAP(REFLECT_XML_READ_ATTRIBUTE_SIMPLE, ATTRS)          \
      MACRO_MAP(REFLECT_XML_READ_TAG_SIMPLE, __VA_ARGS__)          \
    }                                                              \
    static void Write(XmlValue &node, const struct_type &result) { \
      MACRO_MAP(REFLECT_XML_WRITE_ATTRIBUTE_SIMPLE, ATTRS)         \
      MACRO_MAP(REFLECT_XML_WRITE_TAG_SIMPLE, __VA_ARGS__)         \
    }                                                              \
  };                                                               \
  }                                                                \
  }

}  // namespace reflect
}  // namespace spiderweb
#endif
