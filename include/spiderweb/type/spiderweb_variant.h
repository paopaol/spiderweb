#pragma once

#include <string>

#include "absl/types/any.h"
#include "private/spiderweb_variant_private.h"

namespace spiderweb {

class Variant {
 public:
  Variant() = default;

  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value>::type>
  Variant(T def);

  template <typename T,
            typename = typename std::enable_if<std::is_convertible<T, std::string>::value>::type,
            typename = void>
  Variant(T &&def);

  /**
   * @brief copy construct
   */
  Variant(const Variant &rh);

  /**
   * @brief move construct
   */
  Variant(Variant &&rh) noexcept;

  /**
   * @brief copy assgin
   */
  Variant &operator=(const Variant &rh);

  /**
   * @brief move assgin
   */
  Variant &operator=(Variant &&rh) noexcept;

  /**
   * @brief copy/move assgin
   */
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value>::type>
  Variant &operator=(T &&sub_v);

  template <typename T,
            typename = typename std::enable_if<std::is_convertible<T, std::string>::value>::type,
            typename = void>
  Variant &operator=(T &&v);

  bool IsNull() const;

  void Swap(Variant &rh);

  std::string ToString() const;

  int32_t ToInt() const;

  uint32_t ToUint() const;

  bool ToBool() const;

  float ToFloat() const;

  void Clear();

  void SetValue(absl::any v);

  template <typename T>
  T &Get();

  template <typename T>
  const T &Get() const;

 private:
  void Assign(const Variant &rh);

  void StealFrom(Variant &rh);

  void MoveFrom(Variant &rh);

  detail::Var v_{detail::Monostate{}};
};

template <typename T, typename U>
Variant::Variant(T def) : v_(std::forward<T>(def)) {
}

template <typename T, typename U, typename R>
Variant::Variant(T &&def) : v_(std::string(std::forward<T>(def))) {
}

inline Variant::Variant(const Variant &rh) {
  Assign(rh);
}

inline Variant::Variant(Variant &&rh) noexcept {
  StealFrom(rh);
}

inline Variant &Variant::operator=(const Variant &rh) {
  Assign(rh);
  return *this;
}

inline Variant &Variant::operator=(Variant &&rh) noexcept {
  StealFrom(rh);
  return *this;
}

template <typename T, typename U>
Variant &Variant::operator=(T &&sub_v) {
  v_ = std::forward<T>(sub_v);

  return *this;
}

inline bool Variant::IsNull() const {
  return v_.index() == 0;
}

template <typename T, typename U, typename R>
inline Variant &Variant::operator=(T &&v) {
  v_ = std::forward<T>(v);

  return *this;
}

inline void Variant::Swap(Variant &rh) {
  v_.swap(rh.v_);
}

inline std::string Variant::ToString() const {
  return absl::visit(detail::VisitorAs<std::string>(), v_);
}

inline int32_t Variant::ToInt() const {
  return detail::ToIntOrFloat<int32_t>(v_);
}

inline uint32_t Variant::ToUint() const {
  return detail::ToIntOrFloat<uint32_t>(v_);
}

inline bool Variant::ToBool() const {
  return detail::ToIntOrFloat<bool>(v_);
}

inline float Variant::ToFloat() const {
  return detail::ToIntOrFloat<float>(v_);
}

inline void Variant::Clear() {
  v_ = detail::Monostate{};
}

inline void Variant::SetValue(absl::any v) {
  v_ = std::move(v);
}

template <typename T>
T &Variant::Get() {
  return absl::get<T>(v_);
}

template <typename T>
const T &Variant::Get() const {
  return absl::get<T>(v_);
}

inline void Variant::Assign(const Variant &rh) {
  if (this == &rh) {
    return;
  }

  v_ = rh.v_;
}

inline void Variant::StealFrom(Variant &rh) {
  if (this == &rh) {
    return;
  }

  MoveFrom(rh);
}

inline void Variant::MoveFrom(Variant &rh) {
  v_ = std::move(rh.v_);
}

}  // namespace spiderweb
