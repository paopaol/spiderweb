#pragma once

#include <string>

#include "absl/types/any.h"
#include "private/spiderweb_variant_private.h"

namespace spiderweb {

class Variant {
 public:
  Variant() = default;

  template <typename T, typename = typename std::enable_if<
                            !std::is_constructible<std::string, T>::value &&
                            !std::is_same_v<typename std::decay<T>::type, Variant>>::type>
  explicit Variant(T &&v);

  explicit Variant(std::string v);

  Variant(const Variant &v);

  Variant(Variant &&v) noexcept;

  Variant &operator=(const Variant &v);

  Variant &operator=(Variant &&v) noexcept;

  Variant &operator=(std::string v);

  template <typename T, typename = typename std::enable_if<
                            !std::is_constructible<std::string, T>::value &&
                            !std::is_same_v<typename std::decay<T>::type, Variant>>::type>
  Variant &operator=(T &&v) noexcept;

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

  template <typename T>
  bool Is() const;

 private:
  void Assign(const Variant &rh);

  void StealFrom(Variant &rh);

  void MoveFrom(Variant &rh);

  detail::Var v_{detail::Monostate{}};
};

template <typename T, typename U>
Variant::Variant(T &&v) : v_(std::forward<T>(v)) {
}

inline Variant::Variant(std::string v) : v_(std::move(v)) {
}

inline Variant::Variant(const Variant &v) {
  Assign(v);
}

inline Variant::Variant(Variant &&v) noexcept {
  StealFrom(v);
}

inline Variant &Variant::operator=(const Variant &v) {
  Assign(v);
  return *this;
}

inline Variant &Variant::operator=(Variant &&v) noexcept {
  StealFrom(v);
  return *this;
}

inline Variant &Variant::operator=(std::string v) {
  v_ = std::move(v);
  return *this;
}

template <typename T, typename U>
Variant &Variant::operator=(T &&v) noexcept {
  v_ = std::forward<T>(v);
  return *this;
}

inline bool Variant::IsNull() const {
  return v_.index() == 0;
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

template <typename T>
bool Variant::Is() const {
  return absl::holds_alternative<int16_t>(v_);
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
