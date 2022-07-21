
#pragma once

#include <arpa/inet.h>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace spiderweb {
namespace arch {
/**
 * @brief convert host value to BigEndian or HostEndian
 */
template <std::size_t N, typename T,
          typename = typename std::enable_if<std::is_integral<T>::value &&
                                             (N == 2 || N == 4)>::type>
class EndianConvertor {
 public:
  enum class ArchType {
    kBig,
    kHost,
  };
  explicit EndianConvertor(ArchType type, const T value) : type_(type) {
    if (type_ == ArchType::kBig) {
      converted_.value = (N == 2 ? htons(value) : htonl(value));
    } else {
      converted_.value = (N == 2 ? ntohs(value) : ntohl(value));
    }
  }

  /**
   * @brief retuurn size of encoded
   */
  inline std::size_t Size() const { return N; }

  /**
   * @brief beginning of encoded data
   */
  inline const uint8_t *Begin() const { return converted_.ch; }

  /**
   * @brief end of encoded data
   */
  inline const uint8_t *End() const { return converted_.ch + N; }

 private:
  ArchType type_;
  union {
    T       value;
    uint8_t ch[N];
  } converted_;
};

template <std::size_t N, typename T>
inline EndianConvertor<N, T> BigEndian(const T value) {
  return EndianConvertor<N, T>(EndianConvertor<N, T>::ArchType::kBig, value);
}

template <std::size_t N, typename T>
inline EndianConvertor<N, T> HostEndian(const T value) {
  return EndianConvertor<N, T>(EndianConvertor<N, T>::ArchType::kHost, value);
}
}  // namespace arch
}  // namespace spiderweb
