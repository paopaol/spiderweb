#pragma once

#include <functional>

#include "spiderweb/reflect/xml_reflect.h"

namespace spiderweb {
namespace reflect {

class XmlNode {
 public:
  explicit XmlNode();

  ~XmlNode();

  XmlNode(const XmlNode &other);

  XmlNode(XmlNode &&other) noexcept;

  XmlNode &operator=(XmlNode &&other) noexcept;

  XmlNode &operator=(const XmlNode &other);

  void GetAttribute(const char *name, uint32_t &value) const;

  void GetAttribute(const char *name, uint64_t &value) const;

  void GetAttribute(const char *name, int32_t &value) const;

  void GetAttribute(const char *name, int64_t &value) const;

  void GetAttribute(const char *name, float &value) const;

  void GetAttribute(const char *name, double &value) const;

  void GetAttribute(const char *name, bool &value) const;

  void GetAttribute(const char *name, std::string &value) const;

  void SetAttribute(const char *name, uint32_t value);

  void SetAttribute(const char *name, uint64_t value);

  void SetAttribute(const char *name, int32_t value);

  void SetAttribute(const char *name, int64_t value);

  void SetAttribute(const char *name, float value);

  void SetAttribute(const char *name, double value);

  void SetAttribute(const char *name, bool value);

  void SetAttribute(const char *name, const std::string &value);

  void GetValue(uint32_t &value) const;

  void GetValue(uint64_t &value) const;

  void GetValue(int32_t &value) const;

  void GetValue(int64_t &value) const;

  void GetValue(float &value) const;

  void GetValue(double &value) const;

  void GetValue(bool &value) const;

  void GetValue(std::string &value) const;

  void SetValue(uint32_t value);

  void SetValue(uint64_t value);

  void SetValue(int32_t value);

  void SetValue(int64_t value);

  void SetValue(float value);

  void SetValue(double value);

  void SetValue(bool value);

  void SetValue(const std::string &value);

  static bool HasAtrributeValue(const XmlNode &xml, const char *name);

  bool HasValue() const;

  XmlNode FindChild(const char *tag) const;

  XmlNode FirstChild() const;

  XmlNode CreateChild(const char *name);

  void ForEachChilds(const char                                      *tag,
                     const std::function<void(const XmlNode &child)> &handler) const;

  const std::string ToString() const;

 private:
  class Private;
  std::unique_ptr<Private> d;

  friend class XmlBuilder;
};

class XmlBuilder {
  using Document = void *;

 public:
  XmlBuilder(const char *xml, std::size_t size);

  XmlBuilder();

  XmlNode Root() const;

  XmlNode Root();

 private:
  class Private;
  std::shared_ptr<Private> d;
};

using XmlDocumentReader = reflect::XmlReader<reflect::XmlNode, reflect::XmlBuilder>;

using XmlDocumentWriter = reflect::XmlWriter<reflect::XmlNode, reflect::XmlBuilder>;

}  // namespace reflect
}  // namespace spiderweb
