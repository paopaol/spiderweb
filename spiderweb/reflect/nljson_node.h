#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "spiderweb/reflect/json_reflect.h"

namespace spiderweb {
namespace reflect {

class NJsonValue {
 public:
  using Document = nlohmann::json;

  using NodeValue = nlohmann::json;

  NJsonValue();

  ~NJsonValue();

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

  bool SetValue(const std::string &key, const NJsonValue &value);

  void Append(const NJsonValue &value);

  NJsonValue Value(const char *key) const;

  NJsonValue NewValue() const;

  NJsonValue NewArray() const;

  bool IsArray() const;

  bool IsNull() const;

 private:
  Document  doc_;
  NodeValue val_;

  friend class NJsonDocumentReader;
  friend class NJsonDocumentWriter;
  friend class NJsonArray;
};

class NJsonArray {
 public:
  NJsonArray();

  explicit NJsonArray(const NJsonValue &json);

  ~NJsonArray();

  std::size_t Size() const;

  bool IsValid() const;

  void Borrow(const NJsonValue &json);

  NJsonValue operator[](std::size_t index) const;

 private:
  NJsonValue value_;
};

template <>
struct JsonValueVisitor<NJsonValue> {
  template <typename U>
  inline static bool Get(const NJsonValue &json, U &result) {
    return json.GetValue(result);
  }

  template <typename U>
  inline static void Write(NJsonValue &json, const U &value) {
    json.SetValue(value);
  }

  template <typename U>
  inline static void Write(NJsonValue &json, const char *key, const U &value) {
    json.SetValue(key, value);
  }

  inline static void Append(NJsonValue &json, const NJsonValue &value) {
    json.Append(value);
  }

  inline static const NJsonValue ValueOfKey(const NJsonValue &value, const char *key) {
    return value.Value(key);
  }

  inline static const NJsonArray ToArray(const NJsonValue &value) {
    NJsonArray array;

    array.Borrow(value);

    return array;
  }

  inline static NJsonValue NewEmptyValueFrom(const NJsonValue &value) {
    return value.NewValue();
  }

  inline static NJsonValue NewEmptyArrayFrom(const NJsonValue &value) {
    return value.NewArray();
  }

  inline static bool IsNull(const NJsonValue &value) {
    return value.IsNull();
  }
};

class NJsonDocumentReader : public reflect::JsonReader<NJsonValue> {
 public:
  explicit NJsonDocumentReader(const char *json);

  NJsonDocumentReader();

  ~NJsonDocumentReader();

  std::string ToString() const;

 private:
  NJsonValue root_;
};

class NJsonDocumentWriter : public reflect::JsonWriter<NJsonValue> {
 public:
  NJsonDocumentWriter();

  ~NJsonDocumentWriter();

  std::string ToString() const;

 private:
  NJsonValue root_;
};
}  // namespace reflect
}  // namespace spiderweb
