#pragma once

#include <string>

#include "spiderweb/reflect/json_reflect.h"

namespace spiderweb {
namespace reflect {

class JsonValue {
 public:
  struct Document {};

  struct NodeValue {};

  JsonValue();

  JsonValue(Document *doc, NodeValue *val);

  ~JsonValue();

  bool GetValue(int32_t &result) const;

  bool GetValue(uint32_t &result) const;

  bool GetValue(int64_t &result) const;

  bool GetValue(uint64_t &result) const;

  bool GetValue(bool &result) const;

  bool GetValue(float &result) const;

  bool GetValue(double &result) const;

  bool GetValue(std::string &result) const;

  bool SetValue(int32_t value);

  bool SetValue(uint32_t value);

  bool SetValue(int64_t value);

  bool SetValue(uint64_t value);

  bool SetValue(bool value);

  bool SetValue(float value);

  bool SetValue(double value);

  bool SetValue(const std::string &value);

  bool SetValue(const std::string &key, int32_t value);

  bool SetValue(const std::string &key, uint32_t value);

  bool SetValue(const std::string &key, int64_t value);

  bool SetValue(const std::string &key, uint64_t value);

  bool SetValue(const std::string &key, bool value);

  bool SetValue(const std::string &key, float value);

  bool SetValue(const std::string &key, double value);

  bool SetValue(const std::string &key, const std::string &value);

  bool SetValue(const std::string &key, const JsonValue &value);

  void Append(const JsonValue &value);

  JsonValue Value(const char *key) const;

  JsonValue NewValue() const;

  JsonValue NewArray() const;

  bool IsArray() const;

  bool IsNull() const;

 private:
  Document  *doc_ = nullptr;
  NodeValue *val_ = nullptr;

  friend class JsonDocumentReader;
  friend class JsonDocumentWriter;
  friend class JsonArray;
};

class JsonArray {
 public:
  JsonArray();

  explicit JsonArray(const JsonValue &json);

  ~JsonArray();

  std::size_t Size() const;

  bool IsValid() const;

  void Borrow(const JsonValue &json);

  JsonValue operator[](std::size_t index) const;

 private:
  JsonValue value_;
};

template <>
struct JsonValueVisitor<JsonValue> {
  template <typename U>
  inline static bool Get(const JsonValue &json, U &result) {
    return json.GetValue(result);
  }

  template <typename U>
  inline static void Write(JsonValue &json, const U &value) {
    json.SetValue(value);
  }

  template <typename U>
  inline static void Write(JsonValue &json, const char *key, const U &value) {
    json.SetValue(key, value);
  }

  inline static void Append(JsonValue &json, const JsonValue &value) {
    json.Append(value);
  }

  inline static const JsonValue ValueOfKey(const JsonValue &value, const char *key) {
    return value.Value(key);
  }

  inline static const JsonArray ToArray(const JsonValue &value) {
    JsonArray array;

    array.Borrow(value);

    return array;
  }

  inline static JsonValue NewEmptyValueFrom(const JsonValue &value) {
    return value.NewValue();
  }

  inline static JsonValue NewEmptyArrayFrom(const JsonValue &value) {
    return value.NewArray();
  }

  inline static bool IsNull(const JsonValue &value) {
    return value.IsNull();
  }
};

class JsonDocumentReader : public reflect::JsonReader<JsonValue> {
 public:
  explicit JsonDocumentReader(const char *json);

  JsonDocumentReader();

  ~JsonDocumentReader();

  std::string ToString() const;

 private:
  JsonValue root_;
};

class JsonDocumentWriter : public reflect::JsonWriter<JsonValue> {
 public:
  JsonDocumentWriter();

  ~JsonDocumentWriter();

  std::string ToString() const;

 private:
  JsonValue root_;
};
}  // namespace reflect
}  // namespace spiderweb
