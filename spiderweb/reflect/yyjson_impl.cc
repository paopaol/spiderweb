#include <cassert>

#include "spiderweb/reflect/json_node.h"
#include "yyjson.h"

extern "C" {
#include "yyjson.h"
}

#define YYJSON_GET_VALUE(signature)                     \
  auto *val = reinterpret_cast<yyjson_mut_val *>(val_); \
  if (!yyjson_mut_is_null(val)) {                       \
    assert(yyjson_mut_is_##signature(val));             \
    result = yyjson_mut_get_##signature(val);           \
    return true;                                        \
  }                                                     \
  return false;

#define VAR_DECAL(val, doc)                               \
  auto *(val) = reinterpret_cast<yyjson_mut_val *>(val_); \
  auto *(doc) = reinterpret_cast<yyjson_mut_doc *>(doc_); \
  assert((val) && (doc));

namespace spiderweb {
namespace reflect {

JsonValue::JsonValue() = default;

JsonValue::JsonValue(Document *doc, NodeValue *val) : doc_(doc), val_(val) {
}

JsonValue::~JsonValue() = default;

bool JsonValue::GetValue(int32_t &result) const {
  YYJSON_GET_VALUE(int)
}

bool JsonValue::GetValue(uint32_t &result) const {
  YYJSON_GET_VALUE(uint)
}

bool JsonValue::GetValue(int64_t &result) const {
  YYJSON_GET_VALUE(int)
}

bool JsonValue::GetValue(uint64_t &result) const {
  YYJSON_GET_VALUE(uint)
}

bool JsonValue::GetValue(bool &result) const {
  YYJSON_GET_VALUE(bool)
}

bool JsonValue::GetValue(float &result) const {
  YYJSON_GET_VALUE(real)
}

bool JsonValue::GetValue(double &result) const {
  YYJSON_GET_VALUE(real)
}

bool JsonValue::GetValue(std::string &result) const {
  VAR_DECAL(val, doc)
  const auto *v = yyjson_mut_get_str(val);
  if (v) {
    result = v;
  }
  return v != nullptr;
}

bool JsonValue::SetValue(int32_t value) {
  return SetValue(static_cast<int64_t>(value));
}

bool JsonValue::SetValue(uint32_t value) {
  return SetValue(static_cast<uint64_t>(value));
}

bool JsonValue::SetValue(int64_t value) {
  VAR_DECAL(val, doc)

  if (yyjson_likely(val)) {
    val->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT;
    val->uni.i64 = value;
    return true;
  }
  return false;
}

bool JsonValue::SetValue(uint64_t value) {
  VAR_DECAL(val, doc)

  if (yyjson_likely(val)) {
    val->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT;
    val->uni.u64 = value;
    return true;
  }
  return false;
}

bool JsonValue::SetValue(bool value) {
  VAR_DECAL(val, doc)

  if (yyjson_likely(val)) {
    val->tag = YYJSON_TYPE_BOOL | static_cast<uint8_t>(static_cast<uint8_t>(value) << 3);
    return true;
  }
  return false;
}

bool JsonValue::SetValue(float value) {
  return SetValue(static_cast<double>(value));
}

bool JsonValue::SetValue(double value) {
  VAR_DECAL(val, doc)

  if (yyjson_likely(val)) {
    val->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL;
    val->uni.f64 = value;
    return true;
  }
  return false;
}

bool JsonValue::SetValue(const std::string &value) {
  VAR_DECAL(val, doc)

  const auto len = value.size();
  char      *new_str = unsafe_yyjson_mut_strncpy(doc, value.c_str(), len);

  if (yyjson_likely(val && new_str)) {
    val->tag = (static_cast<uint64_t>(len) << YYJSON_TAG_BIT) | YYJSON_TYPE_STR;
    val->uni.str = new_str;
    return true;
  }
  return false;
}

bool JsonValue::SetValue(const std::string &key, int32_t value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_int(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, uint32_t value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_uint(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, int64_t value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_int(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, uint64_t value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_uint(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, bool value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_bool(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, float value) {
  return SetValue(key, static_cast<double>(value));
}

bool JsonValue::SetValue(const std::string &key, double value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_real(doc, val, key.c_str(), value);
}

bool JsonValue::SetValue(const std::string &key, const std::string &value) {
  VAR_DECAL(val, doc)
  return yyjson_mut_obj_add_str(doc, val, key.c_str(), value.c_str());
}

bool JsonValue::SetValue(const std::string &key, const JsonValue &value) {
  VAR_DECAL(val, doc)
  auto *obj_val = reinterpret_cast<yyjson_mut_val *>(value.val_);

  return yyjson_mut_obj_add_val(doc, val, key.c_str(), obj_val);
}

void JsonValue::Append(const JsonValue &value) {
  VAR_DECAL(val, doc)

  if (!yyjson_mut_is_arr(val)) {
    val->tag = YYJSON_TYPE_ARR | YYJSON_SUBTYPE_NONE;
  }
  yyjson_mut_arr_append(val, reinterpret_cast<yyjson_mut_val *>(value.val_));
}

JsonValue JsonValue::Value(const char *key) const {
  VAR_DECAL(val, doc)

  JsonValue value;

  value.doc_ = doc_;
  value.val_ = reinterpret_cast<NodeValue *>(yyjson_mut_obj_get(val, key));

  return value;
}

JsonValue JsonValue::NewValue() const {
  VAR_DECAL(val, doc)

  JsonValue value;

  value.doc_ = doc_;
  value.val_ = reinterpret_cast<NodeValue *>(yyjson_mut_obj(doc));

  return value;
}

JsonValue JsonValue::NewArray() const {
  VAR_DECAL(val, doc)

  JsonValue array;

  array.doc_ = doc_;
  array.val_ = reinterpret_cast<NodeValue *>(yyjson_mut_arr(doc));

  return array;
}

bool JsonValue::IsArray() const {
  VAR_DECAL(val, doc)

  return yyjson_mut_is_arr(val);
}

bool JsonValue::IsNull() const {
  auto *val = reinterpret_cast<yyjson_mut_val *>(val_);

  return !val || yyjson_mut_is_null(const_cast<yyjson_mut_val *>(val));
}

JsonArray::JsonArray() = default;

JsonArray::JsonArray(const JsonValue &json) : value_(json) {
}

JsonArray::~JsonArray() = default;

std::size_t JsonArray::Size() const {
  assert(IsValid());
  return yyjson_mut_arr_size(reinterpret_cast<yyjson_mut_val *>(value_.val_));
}

JsonValue JsonArray::operator[](std::size_t index) const {
  assert(IsValid());

  auto *element = yyjson_mut_arr_get(reinterpret_cast<yyjson_mut_val *>(value_.val_), index);
  return JsonValue(reinterpret_cast<JsonValue::Document *>(value_.doc_),
                   reinterpret_cast<JsonValue::NodeValue *>(element));
}

bool JsonArray::IsValid() const {
  auto *(val) = reinterpret_cast<yyjson_mut_val *>(value_.val_);

  assert(val);
  assert(yyjson_mut_is_arr(val));

  return true;
}

void JsonArray::Borrow(const JsonValue &json) {
  value_ = json;
}

struct YyJsonValueBuilder {
  JsonValue operator()(const char *json) const {
    auto           *idoc = yyjson_read(json, strlen(json), 0);
    yyjson_mut_doc *doc = yyjson_doc_mut_copy(idoc, nullptr);
    auto           *root = yyjson_mut_doc_get_root(doc);
    yyjson_doc_free(idoc);

    return JsonValue(reinterpret_cast<JsonValue::Document *>(doc),
                     reinterpret_cast<JsonValue::NodeValue *>(root));
  }

  JsonValue operator()() const {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    return JsonValue(reinterpret_cast<JsonValue::Document *>(doc),
                     reinterpret_cast<JsonValue::NodeValue *>(root));
  }
};

JsonDocumentReader::JsonDocumentReader(const char *json)
    : reflect::JsonReader<JsonValue>(&root_), root_(YyJsonValueBuilder()(json)) {
}

JsonDocumentReader::JsonDocumentReader() : reflect::JsonReader<JsonValue>(&root_) {
}

JsonDocumentReader::~JsonDocumentReader() {
  auto *doc = reinterpret_cast<yyjson_mut_doc *>(root_.doc_);

  yyjson_mut_doc_free(doc);
}

std::string JsonDocumentReader::ToString() const {
  auto *val = reinterpret_cast<yyjson_mut_val *>(root_.val_);

  char *js = yyjson_mut_val_write(val, 0, nullptr);

  std::string json(js);

  free(js);

  return json;
}

JsonDocumentWriter::JsonDocumentWriter()
    : reflect::JsonWriter<JsonValue>(&root_), root_(YyJsonValueBuilder()()) {
}

JsonDocumentWriter::~JsonDocumentWriter() {
  auto *doc = reinterpret_cast<yyjson_mut_doc *>(root_.doc_);

  if (doc) {
    yyjson_mut_doc_free(doc);
  }
}

std::string JsonDocumentWriter::ToString() const {
  auto       *val = reinterpret_cast<yyjson_mut_val *>(root_.val_);
  char       *js = yyjson_mut_val_write(val, 0, nullptr);
  std::string json(js);
  free(js);
  return json;
}

}  // namespace reflect
}  // namespace spiderweb
