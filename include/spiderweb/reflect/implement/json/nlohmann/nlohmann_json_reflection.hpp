#pragma once

#include "json.hpp"
#include "reflect/reflect_json.hpp"

namespace reflect {
namespace json {
template <>
struct JsonValueVisitor<nlohmann::json> {
  template <typename U>
  inline static bool Get(const nlohmann::json &json, U &result) {
    json.get_to(result);
    return true;
  }

  template <typename U>
  inline static void Write(nlohmann::json &json, U &result) {
    json = std::move(result);
  }

  template <typename U>
  inline static void Write(nlohmann::json &json, const char *key, U &result) {
    json[key] = std::move(result);
  }

  template <typename U>
  inline static void Append(nlohmann::json &json, U &result) {
    json.emplace_back(std::move(result));
  }

  inline static const nlohmann::json &ValueOfKey(const nlohmann::json &value, const char *key) {
    if (value.find(key) != value.end()) {
      return value[key];
    }
    static const nlohmann::json null;
    return null;
  }

  inline static const nlohmann::json &ToArray(const nlohmann::json &value) {
    return value;
  }

  inline static nlohmann::json NewEmptyValueFrom(const nlohmann::json &) {
    return nlohmann::json();
  }

  inline static nlohmann::json NewEmptyArrayFrom(const nlohmann::json &) {
    return nlohmann::json();
  }

  inline static bool IsNull(const nlohmann::json &value) {
    return value.is_null();
  }
};

struct NlJsonValueBuilder {
  nlohmann::json operator()(const char *json) const {
    return nlohmann::json::parse(json);
  }
};

struct NlJsonReader : reflect::json::Reader<nlohmann::json> {
  explicit NlJsonReader(const char *json)
      : root(NlJsonValueBuilder()(json)), reflect::json::Reader<nlohmann::json>(&root) {
    assert(!root.is_null());
  }

  NlJsonReader() : reflect::json::Reader<nlohmann::json>(&root) {
  }

  std::string ToString() const {
    return root.dump();
  }

 public:
  nlohmann::json root;
};

struct NlJsonWriter : reflect::json::Writer<nlohmann::json> {
  explicit NlJsonWriter(const char *json)
      : root(NlJsonValueBuilder()(json)), reflect::json::Writer<nlohmann::json>(&root) {
    assert(!root.is_null());
  }

  NlJsonWriter() : reflect::json::Writer<nlohmann::json>(&root) {
  }

  std::string ToString() const {
    return root.dump();
  }

 public:
  nlohmann::json root;
};
}  // namespace json
}  // namespace reflect
