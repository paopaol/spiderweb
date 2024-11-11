#include <sstream>

#include "pugixml.hpp"
#include "spiderweb/reflect/xml_node.h"

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

class XmlNode::Private {
 public:
  pugi::xml_node root_;
};

XmlNode::XmlNode() : d(new Private) {
}

XmlNode::~XmlNode() = default;

XmlNode::XmlNode(const XmlNode &other) : d(new Private) {
  d->root_ = other.d->root_;
}

XmlNode::XmlNode(XmlNode &&other) noexcept {
  std::swap(other.d, d);
}

XmlNode &XmlNode::operator=(XmlNode &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::swap(other.d, d);

  return *this;
}

XmlNode &XmlNode::operator=(const XmlNode &other) {
  if (&other == this) {
    return *this;
  }

  d->root_ = other.d->root_;

  return *this;
}

void XmlNode::GetAttribute(const char *, PlaceHolderValue &) const {
}

void XmlNode::GetAttribute(const char *name, uint32_t &value) const {
  value = d->root_.attribute(name).as_uint();
}

void XmlNode::GetAttribute(const char *name, uint64_t &value) const {
  value = d->root_.attribute(name).as_ullong();
}

void XmlNode::GetAttribute(const char *name, int32_t &value) const {
  value = d->root_.attribute(name).as_int();
}

void XmlNode::GetAttribute(const char *name, int64_t &value) const {
  value = d->root_.attribute(name).as_llong();
}

void XmlNode::GetAttribute(const char *name, float &value) const {
  value = d->root_.attribute(name).as_float();
}

void XmlNode::GetAttribute(const char *name, double &value) const {
  value = d->root_.attribute(name).as_double();
}

void XmlNode::GetAttribute(const char *name, bool &value) const {
  value = d->root_.attribute(name).as_bool();
}

void XmlNode::GetAttribute(const char *name, std::string &value) const {
  value = d->root_.attribute(name).as_string();
}

void XmlNode::SetAttribute(const char *name, PlaceHolderValue value) {
}

void XmlNode::SetAttribute(const char *name, uint32_t value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, uint64_t value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, int32_t value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, int64_t value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, float value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, double value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, bool value) {
  d->root_.append_attribute(name).set_value(value);
}

void XmlNode::SetAttribute(const char *name, const std::string &value) {
  d->root_.append_attribute(name).set_value(value.c_str());
}

void XmlNode::GetValue(uint32_t &value) const {
  value = d->root_.text().as_uint();
}

void XmlNode::GetValue(uint64_t &value) const {
  value = d->root_.text().as_ullong();
}

void XmlNode::GetValue(int32_t &value) const {
  value = d->root_.text().as_int();
}

void XmlNode::GetValue(int64_t &value) const {
  value = d->root_.text().as_llong();
}

inline void XmlNode::GetValue(float &value) const {
  value = d->root_.text().as_float();
}

void XmlNode::GetValue(double &value) const {
  value = d->root_.text().as_double();
}

void XmlNode::GetValue(bool &value) const {
  value = d->root_.text().as_bool();
}

void XmlNode::GetValue(std::string &value) const {
  value = d->root_.text().as_string();
}

void XmlNode::SetValue(uint32_t value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(uint64_t value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(int32_t value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(int64_t value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(float value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(double value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(bool value) {
  d->root_.text().set(value);
}

void XmlNode::SetValue(const std::string &value) {
  d->root_.text().set(value.c_str());
}

bool XmlNode::HasAtrributeValue(const XmlNode &xml, const char *name) {
  auto attri = xml.d->root_.attribute(name);
  return attri != nullptr;
}

bool XmlNode::HasValue() const {
  return !d->root_.empty();
}

XmlNode XmlNode::FindChild(const char *tag) const {
  XmlNode xml;
  xml.d->root_ = d->root_.child(tag);
  return xml;
}

XmlNode XmlNode::FirstChild() const {
  XmlNode xml;
  xml.d->root_ = d->root_.first_child();

  return xml;
}

XmlNode XmlNode::CreateChild(const char *name) {
  XmlNode xml;
  xml.d->root_ = d->root_.append_child(name);

  return xml;
}

void XmlNode::ForEachChilds(const char                                 *tag,
                            const std::function<void(const XmlNode &)> &handler) const {
  const auto childs = d->root_.children(tag);
  for (const auto &child : childs) {
    XmlNode xml;
    xml.d->root_ = child;
    handler(xml);
  }
}

const std::string XmlNode::ToString() const {
  std::stringstream s;
  d->root_.print(s, PugiXmlIndent<2>::value, PugiXmlIndent<2>::flags);

  return s.str();
}

class XmlBuilder::Private {
 public:
  pugi::xml_document doc_;
};

XmlBuilder::XmlBuilder(const char *xml, std::size_t size) : d(new Private) {
  d->doc_.load_buffer(xml, size);
}

XmlBuilder::XmlBuilder() : d(new Private()) {
}

XmlNode XmlBuilder::Root() const {
  auto xml = XmlNode();
  xml.d->root_ = d->doc_.root();

  return xml;
}

XmlNode XmlBuilder::Root() {
  auto xml = XmlNode();
  xml.d->root_ = d->doc_.root();

  return xml;
}
}  // namespace reflect
}  // namespace spiderweb
