#include "nljson_node.h"

#include "nlohmann/json.hpp"

namespace spiderweb {
namespace reflect {

NJsonValue::NJsonValue() = default;

NJsonValue::~NJsonValue() = default;

bool NJsonValue::GetValue(int32_t &result) const {
  result = val_.get<int32_t>();
  return true;
}

bool NJsonValue::GetValue(uint32_t &result) const {
  result = val_.get<uint32_t>();
  return true;
}

bool NJsonValue::GetValue(int64_t &result) const {
  result = val_.get<int64_t>();
  return true;
}

bool NJsonValue::GetValue(uint64_t &result) const {
  result = val_.get<uint64_t>();
  return true;
}

bool NJsonValue::GetValue(bool &result) const {
  result = val_.get<bool>();
  return true;
}

bool NJsonValue::GetValue(float &result) const {
  result = val_.get<float>();
  return true;
}

bool NJsonValue::GetValue(double &result) const {
  result = val_.get<double>();
  return true;
}

bool NJsonValue::GetValue(std::string &result) const {
  result = val_.get<std::string>();
  return true;
}

bool NJsonValue::SetValue(int32_t value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(uint32_t value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(int64_t value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(uint64_t value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(bool value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(float value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(double value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &value) {
  val_ = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, int32_t value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, uint32_t value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, int64_t value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, uint64_t value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, bool value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, float value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, double value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, const std::string &value) {
  val_[key] = value;
  return true;
}

bool NJsonValue::SetValue(const std::string &key, const NJsonValue &value) {
  val_[key] = value.val_;
  return true;
}

void NJsonValue::Append(const NJsonValue &value) {
  val_.push_back(value.val_);
}

NJsonValue NJsonValue::Value(const char *key) const {
  NJsonValue v;
  v.val_ = val_[key];
  return v;
}

NJsonValue NJsonValue::NewValue() const {
  return NJsonValue();
}

NJsonValue NJsonValue::NewArray() const {
  return NJsonValue();
}

bool NJsonValue::IsArray() const {
  return val_.is_array();
}

bool NJsonValue::IsNull() const {
  return val_.is_null();
}

NJsonArray::NJsonArray() = default;

NJsonArray::NJsonArray(const NJsonValue &json) : value_(json) {
}

NJsonArray::~NJsonArray() = default;

std::size_t NJsonArray::Size() const {
  return value_.val_.size();
}

NJsonValue NJsonArray::operator[](std::size_t index) const {
  NJsonValue v;

  v.val_ = value_.val_[index];
  return v;
}

bool NJsonArray::IsValid() const {
  return value_.val_.is_array();
}

void NJsonArray::Borrow(const NJsonValue &json) {
  value_ = json;
}

NJsonDocumentReader::NJsonDocumentReader(const char *json)
    : reflect::JsonReader<NJsonValue>(&root_) {
  root_.val_ = nlohmann::json::parse(json);
}

NJsonDocumentReader::NJsonDocumentReader() : reflect::JsonReader<NJsonValue>(&root_) {
}

NJsonDocumentReader::~NJsonDocumentReader() = default;

std::string NJsonDocumentReader::ToString() const {
  return root_.val_.dump();
}

NJsonDocumentWriter::NJsonDocumentWriter() : reflect::JsonWriter<NJsonValue>(&root_) {
}

NJsonDocumentWriter::~NJsonDocumentWriter() = default;

std::string NJsonDocumentWriter::ToString() const {
  return root_.val_.dump();
}

}  // namespace reflect
}  // namespace spiderweb
