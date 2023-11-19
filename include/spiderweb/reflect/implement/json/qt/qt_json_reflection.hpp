#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "reflect/reflect_json.hpp"

namespace reflect {
namespace json {
template <typename U>
struct ReaderWriter {
  inline static bool Get(const QJsonValue &json, U &result) {
    result = json.toVariant().value<U>();
    return true;
  }

  inline static void Write(QJsonValue &json, const U &result) { json = result; }

  inline static void Write(QJsonValue &json, const char *key, const U &result) {
    auto object = json.toObject();
    object[key] = result;
    json = object;
  }

  inline static void Append(QJsonValue &json, const U &result) {
    auto object = json.toArray();
    object.append(result);
    json = object;
  }
};

template <>
struct ReaderWriter<std::string> {
  inline static bool Get(const QJsonValue &json, std::string &result) {
    result = json.toVariant().value<QString>().toStdString();
    return true;
  }

  inline static void Write(QJsonValue &json, const std::string &result) { json = result.c_str(); }

  inline static void Write(QJsonValue &json, const char *key, const std::string &result) {
    auto object = json.toObject();
    object[key] = result.c_str();
    json = object;
  }

  inline static void Append(QJsonValue &json, const std::string &result) {
    auto object = json.toArray();
    object.append(result.c_str());
    json = object;
  }
};

template <>
struct JsonValueVisitor<QJsonValue> {
  template <typename U>
  inline static bool Get(const QJsonValue &json, U &result) {
    ReaderWriter<typename std::remove_const<U>::type>::Get(json, result);
    return true;
  }

  inline static QJsonValue ValueOfKey(const QJsonValue &value, const char *key) {
    return value[key];
  }

  inline static QJsonArray ToArray(const QJsonValue &value) { return value.toArray(); }

  template <typename U>
  inline static void Write(QJsonValue &json, U &result) {
    ReaderWriter<typename std::remove_const<U>::type>::Write(json, result);
  }

  template <typename U>
  inline static void Write(QJsonValue &json, const char *key, U &result) {
    ReaderWriter<typename std::remove_const<U>::type>::Write(json, key, result);
  }

  template <typename U>
  inline static void Append(QJsonValue &json, U &result) {
    ReaderWriter<typename std::remove_const<U>::type>::Append(json, result);
  }

  inline static QJsonValue NewEmptyValueFrom(const QJsonValue &) { return QJsonValue(); }

  inline static QJsonValue NewEmptyArrayFrom(const QJsonValue &) { return QJsonValue(); }

  inline static bool IsNull(const QJsonValue &value) {
    return value.isNull() || value.isUndefined();
  }
};

struct QJsonValueBuilder {
  QJsonValue operator()(const char *json) const {
    const auto doc = QJsonDocument().fromJson(json);
    if (doc.isObject()) {
      return doc.object();
    } else if (doc.isArray()) {
      return doc.array();
    }
    return QJsonValue();
  }
};

struct QJsonReader : reflect::json::Reader<QJsonValue> {
  explicit QJsonReader(const char *json)
      : root(QJsonValueBuilder()(json)), reflect::json::Reader<QJsonValue>(&root) {
    assert(!root.isNull());
  }

  QJsonReader() : reflect::json::Reader<QJsonValue>(&root) {}

  inline QString ToString() const {
    QJsonDocument doc(root.toObject());
    return doc.toJson(QJsonDocument::Compact);
  }

  QJsonValue root;
};

struct QJsonWriter : reflect::json::Writer<QJsonValue> {
  explicit QJsonWriter(const char *json)
      : root(QJsonValueBuilder()(json)), reflect::json::Writer<QJsonValue>(&root) {
    assert(!root.isNull());
  }

  QJsonWriter() : reflect::json::Writer<QJsonValue>(&root) {}

  inline QString ToString() const {
    QJsonDocument doc(root.toObject());
    return doc.toJson(QJsonDocument::Compact);
  }

  QJsonValue root;
};

}  // namespace json
}  // namespace reflect
