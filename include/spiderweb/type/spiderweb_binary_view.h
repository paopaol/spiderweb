#ifndef SPIDERWEB_BINARY_VIEW_H
#define SPIDERWEB_BINARY_VIEW_H

#include <cstring>

#include "spiderweb/arch/spiderweb_arch.hpp"
#include "spiderweb/ppk_assert.h"

namespace spiderweb {
namespace type {

namespace detail {

template <typename T, std::size_t N>
union Pack;

template <typename T>
union Pack<T, 2> {
  T        v;
  uint16_t u;
  uint8_t  buf[2];
};

template <typename T>
union Pack<T, 4> {
  T        v;
  uint32_t u;
  uint8_t  buf[4];
};

template <typename T>
union Pack<T, 8> {
  T        v;
  uint64_t u;
  uint8_t  buf[8];
};
};  // namespace detail

class BinaryView {
 public:
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value>::type>
  static T ViewAs(const uint8_t *buf, std::size_t size, arch::ArchType type) {
    constexpr int kTypeSize = sizeof(T);
    PPK_ASSERT_FATAL(size >= kTypeSize, "%lu < type size %d", (long unsigned)size, kTypeSize);

    detail::Pack<T, kTypeSize> pack{};
    std::memcpy(pack.buf, buf, kTypeSize);

    switch (type) {
      case arch::ArchType::kBig: {
        pack.u = arch::FromEndian<arch::ArchType::kBig, kTypeSize>()(pack.u);
        return pack.v;
      } break;
      case arch::ArchType::kLittle: {
        pack.u = arch::FromEndian<arch::ArchType::kLittle, kTypeSize>()(pack.u);
        return pack.v;
      } break;
      default: {
        return T{};
      }
    }
  }
};
}  // namespace type
}  // namespace spiderweb

#endif  // ! SPIDERWEB_BINARY_VIEW_r
