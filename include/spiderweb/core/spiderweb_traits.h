#ifndef SPIDERWEB_TRAITS_H
#define SPIDERWEB_TRAITS_H

#include <type_traits>

namespace spiderweb {

/**
 * @brief test the given type is a byte type(int8, uint8, char, ...)
 */
template <typename T>
struct IsByte {
  static constexpr bool value =
      std::is_integral<T>::value && !std::is_same<T, bool>::value && sizeof(T) == 1;
};

}  // namespace spiderweb

#endif  // !SPIDERWEB_TRAITS_H
