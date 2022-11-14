
#pragma once

#include <arpa/inet.h>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace spiderweb {
namespace arch {

enum class ArchType {
  // abcd
  kBig,
  kLittle,
};

template <ArchType type, std::size_t N>
struct ToEdian {};

template <>
struct ToEdian<ArchType::kBig, 2> {
  template <typename T>
  inline T operator()(T value) {
    return htons(value);
  }
};

template <>
struct ToEdian<ArchType::kBig, 4> {
  template <typename T>
  inline T operator()(T value) {
    return htonl(value);
  }
};

template <>
struct ToEdian<ArchType::kBig, 8> {
  template <typename T>
  inline T operator()(T value) {
    return htonll(value);
  }
};

template <>
struct ToEdian<ArchType::kLittle, 2> {
  template <typename T>
  inline T operator()(T value) {
    return htole16(value);
  }
};

template <>
struct ToEdian<ArchType::kLittle, 4> {
  template <typename T>
  inline T operator()(T value) {
    return htole32(value);
  }
};

template <>
struct ToEdian<ArchType::kLittle, 8> {
  template <typename T>
  inline T operator()(T value) {
    return htole64(value);
  }
};

template <arch::ArchType type, std::size_t N>
struct FromEndian;

template <>
struct FromEndian<arch::ArchType::kBig, 2> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 2>::type>
  inline T operator()(T value) const {
    return be16toh(value);
  }
};

template <>
struct FromEndian<arch::ArchType::kBig, 4> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 4>::type>
  inline T operator()(T value) const {
    return be32toh(value);
  }
};

template <>
struct FromEndian<arch::ArchType::kBig, 8> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 8>::type>
  inline T operator()(T value) const {
    return be64toh(value);
  }
};

template <>
struct FromEndian<arch::ArchType::kLittle, 2> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 2>::type>
  inline T operator()(T value) const {
    return le16toh(value);
  }
};

template <>
struct FromEndian<arch::ArchType::kLittle, 4> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 4>::type>
  inline T operator()(T value) const {
    return le32toh(value);
  }
};

template <>
struct FromEndian<arch::ArchType::kLittle, 8> {
  template <typename T,
            typename = typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 8>::type>
  inline T operator()(T value) const {
    return le64toh(value);
  }
};

/**
 * @brief convert host value to BigEndian or HostEndian
 */
template <ArchType type, std::size_t N, typename T,
          typename =
              typename std::enable_if<std::is_integral<T>::value && (N == 2 || N == 4)>::type>
class EndianConvertor {
 public:
  explicit EndianConvertor(const T value) {
    converted_.value = ToEdian<type, N>()(value);
  }

  /**
   * @brief retuurn size of encoded
   */
  inline std::size_t Size() const {
    return N;
  }

  /**
   * @brief beginning of encoded data
   */
  inline const uint8_t *Begin() const {
    return converted_.ch;
  }

  /**
   * @brief end of encoded data
   */
  inline const uint8_t *End() const {
    return converted_.ch + N;
  }

 private:
  union {
    T       value{};
    uint8_t ch[N];
  } converted_{};
};

template <std::size_t N, typename T>
inline EndianConvertor<ArchType::kBig, N, T> BigEndian(const T value) {
  return EndianConvertor<ArchType::kBig, N, T>(value);
}

template <std::size_t N, typename T>
inline EndianConvertor<ArchType::kLittle, N, T> LittleEndian(const T value) {
  return EndianConvertor<ArchType::kLittle, N, T>(value);
}

}  // namespace arch
}  // namespace spiderweb
