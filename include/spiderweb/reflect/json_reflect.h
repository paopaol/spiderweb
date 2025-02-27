#ifndef SPIDERWEB_REFLECT_JSON_H
#define SPIDERWEB_REFLECT_JSON_H

#include <memory>

#include "spiderweb/reflect/enum_reflect.h"

namespace spiderweb {
namespace reflect {

template <typename T>
struct JsonValueVisitor {
  // template <typename U> bool operator()(const T &json, U &result);
  //
  // template <typename U> void Write(T &json, U &result);
  //
  // template <typename U>
  // inline void Write(T &json, const char *key, U &result);
  //
  // template <typename U> inline void Append(T &json, U &result);
  //
  // template <typename U> static U ValueOfKey(const T &value, const char *key);
  //
  // template <typename U> static U ToArray(const T &value);
  //
  // static T NewEmptyValueFrom(const U &value);
  //
  // static T NewEmptyArrayFrom(const nlohmann::json
  // &value);
};

template <typename T>
class JsonReader;
template <typename T>
class JsonWriter;
template <typename U, typename T>
struct Meta;

// base type
template <typename U, typename T>
struct Meta {
  static constexpr bool IsMeta = false;
  using ValueType = U;
  using JsonType = T;
  using Visitor = JsonValueVisitor<JsonType>;

  inline bool FromJson(const JsonType &json, ValueType *result) const {
    return Visitor::Get(json, *result);
  }

  inline bool FromJson(const JsonType &json, const char *key, ValueType *result) const {
    return FromJson(Visitor::ValueOfKey(json, key), result);
  }

  inline void ToJson(const ValueType *result, JsonType &json) const {
    Visitor::Write(json, *result);
  }

  inline void ToJson(const ValueType *result, const char *key, JsonType &json) const {
    Visitor::Write(json, key, *result);
  }
};

template <typename U, typename T>
struct Meta<std::shared_ptr<U>, T> {
  static constexpr bool IsMeta = false;
  using ValueType = U;
  using ValueTypePtr = std::shared_ptr<ValueType>;
  using JsonType = T;
  using Visitor = JsonValueVisitor<JsonType>;

  inline bool FromJson(const JsonType &json, ValueTypePtr *result) const {
    if (!(*result)) {
      result->reset(new ValueType);
    }

    if (Visitor::IsNull(json)) {
      return false;
    }
    Meta<ValueType, JsonType> meta;
    return meta.FromJson(json, result->get());
  }

  inline void ToJson(const ValueTypePtr *result, JsonType &json) const {
    if (!(*result)) {
      return;
    }

    Meta<ValueType, JsonType> meta;
    meta.ToJson(result->get(), json);
  }

  inline void ToJson(const ValueTypePtr *result, const char *key, JsonType &json) const {
    if (!(*result)) {
      return;
    }

    Meta<ValueType, JsonType> meta;
    meta.ToJson(result->get(), key, json);
  }
};

template <typename T>
struct Meta<std::string, T> {
  static constexpr bool IsMeta = false;
  using ValueType = std::string;
  using JsonType = T;
  using Visitor = JsonValueVisitor<JsonType>;

  inline bool FromJson(const JsonType &json, std::string *result) const {
    if (Visitor::IsNull(json)) {
      return false;
    }
    return Visitor::Get(json, *result);
  }

  inline void ToJson(const std::string *result, JsonType &json) const {
    if (!result) {
      return;
    }
    Visitor::Write(json, *result);
  }

  inline void ToJson(const std::string *result, const char *key, JsonType &json) const {
    if (!result) {
      return;
    }
    Visitor::Write(json, key, *result);
  }
};

// array type
template <typename ValueType, template <typename Elem, typename...> class Cont, typename JsonType>
struct ArrayMeta {
  using Visitor = JsonValueVisitor<JsonType>;

  static constexpr bool IsMeta = Meta<ValueType, JsonType>::IsMeta;

  inline bool FromJson(const JsonType &json, Cont<ValueType> *result) const {
    return this->FromJsonArray(Visitor::ToArray(json), result);
  }

  inline bool FromJson(const JsonType &json, const char *key, Cont<ValueType> *result) const {
    const auto &value = Visitor::ValueOfKey(json, key);
    const auto &array = Visitor::ToArray(value);
    return this->FromJsonArray(array, result);
  }

  inline void ToJson(const Cont<ValueType> *result, JsonType &json) const {
    for (const auto &value : *result) {
      Meta<ValueType, JsonType> meta;
      JsonType                  tmp = Visitor::NewEmptyValueFrom(json);

      meta.ToJson(&value, tmp);
      Visitor::Append(json, tmp);
    }
  }

  inline void ToJson(const Cont<ValueType> *result, const char *key, JsonType &json) const {
    auto array = Visitor::NewEmptyArrayFrom(json);
    ToJson(result, array);

    Visitor::Write(json, key, array);
  }

  template <typename ArrayType>
  inline bool FromJsonArray(const ArrayType &array, Cont<ValueType> *result) const {
    const auto size = array.Size();
    result->reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
      const JsonType      &json = array[i];
      JsonReader<JsonType> reader(&json);
      ValueType            tmp;
      if (!reader.FromJson(&tmp)) {
        return false;
      }
      result->push_back(tmp);
    }
    return true;
  }
};

// stl container
template <typename ValueType, template <typename Elem, typename = std::allocator<Elem>> class Cont,
          typename JsonType>
struct Meta<Cont<ValueType>, JsonType> : ArrayMeta<ValueType, Cont, JsonType> {};

// qtl container
template <typename ValueType, template <typename Elem> class Cont, typename JsonType>
struct Meta<Cont<ValueType>, JsonType> : ArrayMeta<ValueType, Cont, JsonType> {};

template <typename T>
class JsonReader {
 public:
  using JsonType = T;
  using Visitor = JsonValueVisitor<JsonType>;

  explicit JsonReader(const JsonType *access) : access_(access) {
  }
  ~JsonReader() = default;

  template <typename U>
  inline bool FromJson(U *result) const {
    using ThisMeta = Meta<U, JsonType>;
    ThisMeta meta;
    return meta.FromJson(*this->access_, result);
  }

  inline bool FromJson(PlaceHolderValue * /*result*/) const {
    return true;
  }

  inline bool FromJson(const char * /*key*/, PlaceHolderValue * /*result*/) const {
    return true;
  }

  template <typename U>
  inline bool FromJson(const char *key, U *result) const {
    const auto &v = Visitor::ValueOfKey(*access_, key);

    JsonReader<JsonType> reader(&v);
    if (Visitor::IsNull(v)) {
      return false;
    }
    return reader.FromJson(result);
  }

  template <typename U>
  U ToString() const;

  JsonReader(const JsonReader &other) = delete;
  JsonReader &operator=(const JsonReader &other) = delete;

 private:
  const JsonType *access_;
};

template <typename T>
class JsonWriter {
 public:
  using JsonType = T;
  using Visitor = JsonValueVisitor<JsonType>;

  explicit JsonWriter(JsonType *access) : access_(access) {
  }
  ~JsonWriter() = default;

  template <typename U>
  inline void ToJson(const U *result) const {
    using ThisMeta = Meta<U, JsonType>;

    ThisMeta meta;
    meta.ToJson(result, *this->access_);
  }

  inline void ToJson(const PlaceHolderValue *result) const {
  }

  template <typename U>
  inline void ToJson(const char *key, const U *result) const {
    using ThisMeta = Meta<U, JsonType>;

    ThisMeta meta;
    JsonType value = Visitor::NewEmptyValueFrom(*access_);
    meta.ToJson(result, value);

    Visitor::Write(*access_, key, value);
  }

  inline void ToJson(const char *key, const PlaceHolderValue *result) const {
  }

  template <typename U>
  U ToString() const;

  JsonWriter(const JsonWriter &other) = delete;
  JsonWriter &operator=(const JsonWriter &other) = delete;

 private:
  JsonType *access_;
};

#define REFLECT_JSON_READ_CODE_BLOCK_0()
#define REFLECT_JSON_READ_CODE_BLOCK_1(member) serilizer.FromJson(#member, &result->member);
#define REFLECT_JSON_READ_CODE_BLOCK_2(member, name) serilizer.FromJson(name, &result->member);
#define REFLECT_JSON_READ_CODE_BLOCK_3(member, name, type)    \
  {                                                           \
    using XmlStorageType = type;                              \
    XmlStorageType value;                                     \
    serilizer.FromJson(name, &value);                         \
    result->member = reflect::MapFrom(value, result->member); \
  }

#define REFLECT_JSON_WRITE_CODE_BLOCK_0()
#define REFLECT_JSON_WRITE_CODE_BLOCK_1(member) serilizer.ToJson(#member, &result->member);
#define REFLECT_JSON_WRITE_CODE_BLOCK_2(member, name) serilizer.ToJson(name, &result->member);
#define REFLECT_JSON_WRITE_CODE_BLOCK_3(member, name, type) \
  {                                                         \
    type value;                                             \
    reflect::MapTo(result->member, value);                  \
    serilizer.ToJson(name, &value);                         \
  }

#define reflect_json_expands_fromjson(PAIR)                                  \
  VISIT_STRUCT_CONCAT(REFLECT_JSON_READ_CODE_BLOCK_,                         \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES(PAIR))) \
  PAIR

#define reflect_json_expands_tojson(PAIR)                                        \
  VISIT_STRUCT_CONCAT(REFLECT_JSON_WRITE_CODE_BLOCK_,                            \
                      REFLECT_NUM_OF_ARGS(REFLECT_REMOVE_PARENTHESES_IMPL PAIR)) \
  PAIR

#define REFLECT_JSON_READ_CODE_SIMPLE_BLOCK(member) \
  {                                                 \
    serilizer.FromJson(#member, &result->member);   \
  }

#define reflect_json_expands_fromjson_simple(member) \
  {                                                  \
    serilizer.FromJson(#member, &result->member);    \
  }

#define REFLECT_JSON_WRITE_CODE_SIMPLE_BLOCK(member) serilizer.ToJson(#member, &result->member);

#define REFLECT_JSON_WRITE_CODE_SIMPLE_BLOCK_IMPL(member) \
  serilizer.ToJson(#member, &result->member);

#define reflect_json_expands_tojson_simple(member) serilizer.ToJson(#member, &result->member);

#define REFLECT_JSON(Type, ...)                                          \
  namespace spiderweb {                                                  \
  namespace reflect {                                                    \
  template <typename JsonNodeType>                                       \
  struct Meta<Type, JsonNodeType> {                                      \
    static constexpr bool IsMeta = true;                                 \
    using struct_type = Type;                                            \
    using jsonnode_type = JsonNodeType;                                  \
    bool FromJson(const JsonNodeType &json, struct_type *result) const { \
      reflect::JsonReader<jsonnode_type> serilizer(&json);               \
      MACRO_MAP(reflect_json_expands_fromjson, __VA_ARGS__)              \
      return true;                                                       \
    }                                                                    \
    void ToJson(const struct_type *result, JsonNodeType &json) const {   \
      reflect::JsonWriter<jsonnode_type> serilizer(&json);               \
      MACRO_MAP(reflect_json_expands_tojson, __VA_ARGS__)                \
    }                                                                    \
  };                                                                     \
  }                                                                      \
  }

#define REFLECT_JSON_SIMPLE(Type, ...)                                   \
  namespace spiderweb {                                                  \
  namespace reflect {                                                    \
  template <typename JsonNodeType>                                       \
  struct Meta<Type, JsonNodeType> {                                      \
    static constexpr bool IsMeta = true;                                 \
    using struct_type = Type;                                            \
    using jsonnode_type = JsonNodeType;                                  \
    bool FromJson(const JsonNodeType &json, struct_type *result) const { \
      reflect::JsonReader<jsonnode_type> serilizer(&json);               \
      MACRO_MAP(reflect_json_expands_fromjson_simple, __VA_ARGS__)       \
      return true;                                                       \
    }                                                                    \
    void ToJson(const struct_type *result, JsonNodeType &json) const {   \
      reflect::JsonWriter<jsonnode_type> serilizer(&json);               \
      MACRO_MAP(reflect_json_expands_tojson_simple, __VA_ARGS__)         \
    }                                                                    \
  };                                                                     \
  }                                                                      \
  }

}  // namespace reflect
}  // namespace spiderweb
#endif
