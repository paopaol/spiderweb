#pragma once

#include <string>

#include "absl/types/any.h"
#include "absl/types/variant.h"
#include "fmt/format.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/core/spiderweb_type_traits.h"

namespace spiderweb {

struct Monostate {};

class Variant {
 public:
  using Var = absl::variant<Monostate, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
                            std::string, float, double, bool, absl::any>;
  enum class Type {
    kBool,
    kInt16,
    kUint16,
    kInt32,
    kUint32,
    kInt64,
    kUint64,
    kFloat,
    kString,
    kAny,
    kNull,
  };

  template <typename T, typename Dumy = void>
  struct CurrentType {
    static constexpr Type type = Type::kAny;
  };

  template <typename Dumy>
  struct CurrentType<int16_t, Dumy> {
    static constexpr Type type = Type::kInt16;
  };

  template <typename Dumy>
  struct CurrentType<uint16_t, Dumy> {
    static constexpr Type type = Type::kUint16;
  };

  template <typename Dumy>
  struct CurrentType<int32_t, Dumy> {
    static constexpr Type type = Type::kInt32;
  };

  template <typename Dumy>
  struct CurrentType<uint32_t, Dumy> {
    static constexpr Type type = Type::kUint32;
  };

  template <typename Dumy>
  struct CurrentType<int64_t, Dumy> {
    static constexpr Type type = Type::kInt64;
  };

  template <typename Dumy>
  struct CurrentType<uint64_t, Dumy> {
    static constexpr Type type = Type::kUint64;
  };

  template <typename Dumy>
  struct CurrentType<float, Dumy> {
    static constexpr Type type = Type::kFloat;
  };

  template <typename Dumy>
  struct CurrentType<bool, Dumy> {
    static constexpr Type type = Type::kBool;
  };

  template <typename Dumy>
  struct CurrentType<std::string, Dumy> {
    static constexpr Type type = Type::kString;
  };

  template <typename Dumy>
  struct CurrentType<absl::any, Dumy> {
    static constexpr Type type = Type::kAny;
  };

  template <typename Dumy>
  struct CurrentType<Monostate, Dumy> {
    static constexpr Type type = Type::kNull;
  };

  Variant() = default;

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<typename std::decay<T>::type, Variant>::value &&
                            !IsString<T>::value>::type>
  Variant(T &&def)
      : v_(std::forward<T>(def)), current_type_(CurrentType<typename std::decay<T>::type>::type) {
  }

  Variant(std::string def) : v_(std::move(def)), current_type_(CurrentType<std::string>::type) {
  }

  static Variant Any(absl::any v) {
    Variant var;
    var.current_type_ = Type::kAny;
    var.v_ = std::move(v);
    return var;
  }

  /**
   * @brief copy construct
   */
  Variant(const Variant &rh) {
    Assign(rh);
  }

  /**
   * @brief move construct
   */
  Variant(Variant &&rh) noexcept {
    StealFrom(rh);
  }

  /**
   * @brief copy assgin
   */
  Variant &operator=(const Variant &rh) {
    Assign(rh);
    return *this;
  }

  /**
   * @brief move assgin
   */
  Variant &operator=(Variant &&rh) noexcept {
    StealFrom(rh);
    return *this;
  }

  /**
   * @brief copy/move assgin
   */
  template <typename T, typename = typename std::enable_if<
                            !std::is_same<typename std::decay<T>::type, Variant>::value &&
                            !IsString<T>::value>::type>
  Variant &operator=(T &&sub_v) {
    current_type_ = CurrentType<T>::type;
    v_ = std::forward<T>(sub_v);

    return *this;
  }

  Variant &operator=(std::string sub_v) {
    current_type_ = CurrentType<std::string>::type;
    v_ = std::move(sub_v);

    return *this;
  }

  inline bool IsNull() const {
    return GetType() == Type::kNull;
  }

  inline Type GetType() const {
    return current_type_;
  }

  inline void Swap(Variant &rh) {
    std::swap(current_type_, rh.current_type_);
    v_.swap(rh.v_);
  }

  std::string ToString() const;

  int64_t ToInt(bool *ok = nullptr) const;

  uint64_t ToUint(bool *ok = nullptr) const;

  bool ToBool() const;

  float ToFloat(bool *ok = nullptr) const;

 private:
  inline void Assign(const Variant &rh) {
    if (this == &rh) {
      return;
    }

    current_type_ = rh.current_type_;
    v_ = rh.v_;
  }

  inline void StealFrom(Variant &rh) {
    if (this == &rh) {
      return;
    }

    MoveFrom(rh);
  }

  inline void MoveFrom(Variant &rh) {
    std::swap(current_type_, rh.current_type_);
    v_ = std::move(rh.v_);
  }

  Var  v_{Monostate{}};
  Type current_type_ = Type::kNull;
};
}  // namespace spiderweb
