#pragma once

#include <assert.h>

#include <string>

#include "reflect/reflect_json.hpp"

extern "C" {
#include "yyjson.h"
}

#define YYJSON_GET_VALUE(signature, type)                     \
  static bool json_get_value(yyjson_value js, type &result) { \
    if (!yyjson_mut_is_null(js.val())) {                      \
      bool ret = yyjson_mut_is_##signature(js.val());         \
      assert(ret);                                            \
      result = yyjson_mut_get_##signature(js.val());          \
      return true;                                            \
    }                                                         \
    return false;                                             \
  }

namespace spiderweb {
namespace reflect {
namespace json {

class yyjson_value {
 public:
  yyjson_value() = default;
  ~yyjson_value() = default;
  yyjson_value(yyjson_mut_doc *doc, yyjson_mut_val *val) : doc_(doc), val_(val) {
  }

  const yyjson_mut_val *val() const {
    return val_;
  }
  yyjson_mut_val *val() {
    return val_;
  }

  yyjson_mut_doc *doc() const {
    return doc_;
  }

 private:
  yyjson_mut_doc *doc_ = nullptr;
  yyjson_mut_val *val_ = nullptr;
};

class yyjson_value_array {
 public:
  yyjson_value_array() = default;
  ~yyjson_value_array() = default;

  inline std::size_t size() const {
    assert(is_valid());
    return yyjson_mut_arr_size(const_cast<yyjson_mut_val *>(ptr_.val()));
  }

  inline yyjson_value operator[](std::size_t index) const {
    assert(is_valid());
    return yyjson_value(ptr_.doc(),
                        yyjson_mut_arr_get(const_cast<yyjson_mut_val *>(ptr_.val()), index));
  }

  inline bool is_valid() const {
    assert(ptr_.val());
    assert(yyjson_mut_is_arr((yyjson_mut_val *)ptr_.val()));
    return true;
  }

  inline void borrow(yyjson_value json) {
    ptr_ = json;
  }

 private:
  yyjson_value ptr_;
};

YYJSON_GET_VALUE(bool, bool)
YYJSON_GET_VALUE(uint, uint64_t)
YYJSON_GET_VALUE(sint, int64_t)
YYJSON_GET_VALUE(int, int)
YYJSON_GET_VALUE(real, double)
YYJSON_GET_VALUE(real, float)
YYJSON_GET_VALUE(str, std::string)

static void json_append_value(yyjson_value &array, const yyjson_value &value) {
  bool ok = yyjson_mut_arr_append(array.val(), const_cast<yyjson_mut_val *>(value.val()));
}

static void json_set_value(yyjson_value &json, const char *key, int value) {
  assert(json.val());
  assert(json.doc());

  yyjson_mut_obj_add_int(json.doc(), json.val(), key, value);
}

static void json_set_value(yyjson_value &json, int value) {
  assert(json.val());
  assert(json.doc());

  if (yyjson_likely(json.val())) {
    json.val()->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT;
    json.val()->uni.i64 = value;
  }
}

static void json_set_value(yyjson_value &json, bool value) {
  assert(json.val());
  assert(json.doc());

  if (yyjson_likely(json.val())) {
    json.val()->tag = YYJSON_TYPE_BOOL | static_cast<uint8_t>(static_cast<uint8_t>(value) << 3);
  }
}

static void json_set_value(yyjson_value &json, uint64_t value) {
  assert(json.val());
  assert(json.doc());

  if (yyjson_likely(json.val())) {
    json.val()->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT;
    json.val()->uni.u64 = value;
  }
}

static void json_set_value(yyjson_value &json, int64_t value) {
  assert(json.val());
  assert(json.doc());

  if (yyjson_likely(json.val())) {
    json.val()->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT;
    json.val()->uni.i64 = value;
  }
}

static void json_set_value(yyjson_value &json, double value) {
  assert(json.val());
  assert(json.doc());

  if (yyjson_likely(json.val())) {
    json.val()->tag = YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL;
    json.val()->uni.f64 = value;
  }
}

static void json_set_value(yyjson_value &json, float value) {
  json_set_value(json, static_cast<double>(value));
}

static void json_set_value(yyjson_value &json, const char *value) {
  assert(json.val());
  assert(json.doc());

  const auto len = strlen(value);
  char      *new_str = unsafe_yyjson_mut_strncpy(json.doc(), value, len);
  if (yyjson_likely(json.val() && new_str)) {
    json.val()->tag = (static_cast<uint64_t>(len) << YYJSON_TAG_BIT) | YYJSON_TYPE_STR;
    json.val()->uni.str = new_str;
  }
}

static void json_set_value(yyjson_value &json, const std::string &value) {
  json_set_value(json, value.c_str());
}

static void json_set_value(yyjson_value &json, const char *key, yyjson_value value) {
  assert(json.val());
  assert(json.doc());

  yyjson_mut_obj_add_val(json.doc(), json.val(), key, value.val());
}

template <>
struct JsonValueVisitor<yyjson_value> {
  template <typename U>
  inline static bool Get(const yyjson_value json, U &result) {
    return json_get_value(json, result);
  }

  template <typename U>
  inline static void Write(yyjson_value &json, const U &result) {
    json_set_value(json, result);
  }

  template <typename U>
  inline static void Write(yyjson_value json, const char *key, const U &result) {
    json_set_value(json, key, result);
  }

  template <typename U>
  inline static void Append(yyjson_value json, const U &result) {
    if (!yyjson_mut_is_arr(json.val())) {
      json.val()->tag = YYJSON_TYPE_ARR | YYJSON_SUBTYPE_NONE;
    }
    json_append_value(json, result);
  }

  inline static const yyjson_value ValueOfKey(const yyjson_value value, const char *key) {
    return yyjson_value(value.doc(),
                        yyjson_mut_obj_get(const_cast<yyjson_mut_val *>(value.val()), key));
  }

  inline static const yyjson_value_array ToArray(const yyjson_value value) {
    yyjson_value_array array;
    array.borrow(value);
    return array;
  }

  inline static yyjson_value NewEmptyValueFrom(const yyjson_value &value) {
    return yyjson_value(value.doc(), yyjson_mut_obj(value.doc()));
  }

  inline static yyjson_value NewEmptyArrayFrom(const yyjson_value &value) {
    auto array = yyjson_value(value.doc(), yyjson_mut_arr(value.doc()));
    return array;
  }

  inline static bool IsNull(const yyjson_value &value) {
    return !value.val() || yyjson_mut_is_null(const_cast<yyjson_mut_val *>(value.val()));
  }
};

struct YyJsonValueBuilder {
  yyjson_value operator()(const char *json) const {
    auto           *idoc = yyjson_read(json, strlen(json), 0);
    yyjson_mut_doc *doc = yyjson_doc_mut_copy(idoc, nullptr);
    auto           *root = yyjson_mut_doc_get_root(doc);
    yyjson_doc_free(idoc);
    return yyjson_value(doc, root);
  }

  yyjson_value operator()() const {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    return yyjson_value(doc, root);
  }
};

struct YyJsonReader : reflect::json::Reader<yyjson_value> {
  explicit YyJsonReader(const char *json)
      : root(YyJsonValueBuilder()(json)), reflect::json::Reader<yyjson_value>(&root) {
  }

  YyJsonReader() : reflect::json::Reader<yyjson_value>(&root) {
  }

  ~YyJsonReader() {
    yyjson_mut_doc_free(root.doc());
  }

  std::string ToString() const {
    char       *js = yyjson_mut_val_write(root.val(), 0, nullptr);
    std::string json(js);
    free(js);
    return json;
  }

 public:
  yyjson_value root;
};

struct YyJsonWriter : reflect::json::Writer<yyjson_value> {
  YyJsonWriter() : root(YyJsonValueBuilder()()), reflect::json::Writer<yyjson_value>(&root) {
  }

  ~YyJsonWriter() {
    if (root.doc()) {
      yyjson_mut_doc_free(root.doc());
    }
  }

  std::string ToString() const {
    char       *js = yyjson_mut_val_write(root.val(), 0, nullptr);
    std::string json(js);
    free(js);
    return json;
  }

 private:
  yyjson_value root;
};
}  // namespace json
}  // namespace reflect
}  // namespace spiderweb
