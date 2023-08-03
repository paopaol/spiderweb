#pragma once

#include <string>
#include <type_traits>

namespace spiderweb {

template <typename T>
struct IsString {
  static constexpr bool value = std::is_convertible<T, std::string>::value;
};

template <typename T>
using EnableIfString = typename std::enable_if<IsString<T>::value>::type;

template <typename T>
using EnableIfNotString = typename std::enable_if<!IsString<T>::value>::type;
;
}  // namespace spiderweb
