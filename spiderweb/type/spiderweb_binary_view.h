#ifndef SPIDERWEB_BINARY_VIEW_H
#define SPIDERWEB_BINARY_VIEW_H

#include <cstring>
#include <system_error>

#include "arch/spiderweb_arch.hpp"
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
  char     buf[2];
};

template <typename T>
union Pack<T, 4> {
  T        v;
  uint32_t u;
  char     buf[4];
};

template <typename T>
union Pack<T, 8> {
  T        v;
  uint64_t u;
  char     buf[8];
};
};  // namespace detail

class BinaryView {
 public:
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value>::type>
  static T ViewAs(const uint8_t *buf, std::size_t size, arch::ArchType type, bool mid = false) {
    constexpr int kTypeSize = sizeof(T);
    PPK_ASSERT_FATAL(size >= kTypeSize, "%d < type size %d", size, kTypeSize);

    detail::Pack<T, kTypeSize> pack;
    std::memcpy(pack.buf, buf, kTypeSize);

    if (mid) {
    }

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
