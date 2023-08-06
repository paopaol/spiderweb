#pragma once

#include <string>
#include <type_traits>

namespace spiderweb {

template <class T>
struct IsVoid : std::is_same<void, typename std::remove_cv<T>::type> {};

template <typename T>
struct IsString {
  static constexpr bool value = std::is_convertible<T, std::string>::value;
};

template <typename T>
using EnableIfString = typename std::enable_if<IsString<T>::value>::type;

template <typename T>
using EnableIfNotString = typename std::enable_if<!IsString<T>::value>::type;
}  // namespace spiderweb
