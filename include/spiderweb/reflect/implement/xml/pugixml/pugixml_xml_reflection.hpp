#pragma once

#include <sstream>
#include <string>

#include "pugixml.hpp"
#include "reflect/reflect_enum.hpp"
#include "reflect/reflect_xml.hpp"

namespace spiderweb {
namespace reflect {
template <int Indent>
struct PugiXmlIndent;

#define PUGIXML_IDENT_N(N, indent)                             \
  template <>                                                  \
  struct PugiXmlIndent<N> {                                    \
    static constexpr const char *value = indent;               \
    static constexpr const int   flags = pugi::format_default; \
  };

PUGIXML_IDENT_N(0, "")
PUGIXML_IDENT_N(1, " ")
PUGIXML_IDENT_N(2, "  ")
PUGIXML_IDENT_N(3, "   ")
PUGIXML_IDENT_N(4, "    ")
PUGIXML_IDENT_N(5, "     ")
PUGIXML_IDENT_N(6, "      ")
PUGIXML_IDENT_N(7, "       ")
PUGIXML_IDENT_N(8, "        ")

template <>
struct PugiXmlIndent<-1> {
  static constexpr const char *value = "";
  static constexpr const int   flags = pugi::format_raw;
};

class PugiXml {
 public:
  explicit PugiXml(pugi::xml_node root) : root_(root) {
  }

  inline void GetAttribute(const char *name, uint32_t &value) const {
    value = root_.attribute(name).as_uint();
  }

  inline void GetAttribute(const char *name, uint64_t &value) const {
    value = root_.attribute(name).as_ullong();
  }

  inline void GetAttribute(const char *name, int32_t &value) const {
    value = root_.attribute(name).as_int();
  }

  inline void GetAttribute(const char *name, int64_t &value) const {
    value = root_.attribute(name).as_llong();
  }

  inline void GetAttribute(const char *name, float &value) const {
    value = root_.attribute(name).as_float();
  }

  inline void GetAttribute(const char *name, double &value) const {
    value = root_.attribute(name).as_double();
  }

  inline void GetAttribute(const char *name, bool &value) const {
    value = root_.attribute(name).as_bool();
  }

  inline void GetAttribute(const char *name, std::string &value) const {
    value = root_.attribute(name).as_string();
  }

  inline void GetValue(uint32_t &value) const {
    value = root_.text().as_uint();
  }

  inline void GetValue(uint64_t &value) const {
    value = root_.text().as_ullong();
  }

  inline void GetValue(int32_t &value) const {
    value = root_.text().as_int();
  }

  inline void GetValue(int64_t &value) const {
    value = root_.text().as_llong();
  }

  inline void GetValue(float &value) const {
    value = root_.text().as_float();
  }

  inline void GetValue(double &value) const {
    value = root_.text().as_double();
  }

  inline void GetValue(bool &value) const {
    value = root_.text().as_bool();
  }

  inline void GetValue(std::string &value) const {
    value = root_.text().as_string();
  }

  template <typename T>
  void SetAttribute(const char *name, const T &value) {
    root_.append_attribute(name).set_value(value);
  }

  void SetAttribute(const char *name, const std::string &value) {
    root_.append_attribute(name).set_value(value.c_str());
  }

  template <typename T>
  void SetValue(const T &value) {
    root_.text().set(value);
  }

  void SetValue(const std::string &value) {
    root_.text().set(value.c_str());
  }

  static inline bool HasValue(const pugi::xml_attribute &attri) {
    return attri != nullptr;
  }

  inline bool HasValue() const {
    return !root_.empty();
  }

  inline const pugi::xml_attribute GetAttributeNode(const char *name) const {
    return root_.attribute(name);
  }

  inline const PugiXml FindChild(const char *tag) const {
    return PugiXml(root_.child(tag));
  }

  inline const pugi::xml_object_range<pugi::xml_named_node_iterator> FindChilds(
      const char *tag) const {
    return root_.children(tag);
  }

  inline const PugiXml FirstChild() const {
    return PugiXml(root_.first_child());
  }

  inline PugiXml CreateChild(const char *name) {
    return PugiXml(root_.append_child(name));
  }

  template <int Indent>
  inline const std::string ToString() const {
    static_assert(Indent <= 8 && Indent >= -1, "unsupported indent");

    std::stringstream s;
    root_.print(s, PugiXmlIndent<Indent>::value, PugiXmlIndent<Indent>::flags);

    return s.str();
  }

 private:
  pugi::xml_node root_;
};

class PugiXmlBuilder {
 public:
  PugiXmlBuilder(const char *xml, std::size_t size) {
    doc_.load_buffer(xml, size);
  }

  PugiXmlBuilder() = default;

  inline const PugiXml Root() const {
    return PugiXml(doc_.root());
  }

  inline PugiXml Root() {
    return PugiXml(doc_.root());
  }

 private:
  pugi::xml_document doc_;
};

using PugiXmlReader = reflect::XmlReader<reflect::PugiXml, reflect::PugiXmlBuilder>;

using PugiXmlWriter = reflect::XmlWriter<reflect::PugiXml, reflect::PugiXmlBuilder>;

}  // namespace reflect
}  // namespace spiderweb
