#pragma once

#include <cstdint>
#include <type_traits>

namespace spiderweb {
namespace io {

/**
 * @brief bit access
 *
 * The bitmap reader/writer is used to provide bit access functionality.
 *
 * For a given integer variable, sometimes we need to access (read or write)
 *
 * one or more bits. The usual practice is to write a macro to encapsulate
 *
 * the displacement operation. But Bitmap (Reader/Writer) provides a more
 *
 * intuitive interface.
 *
 * @example
 *
 * //access bit 31
 * uint32_t id = 0;
 * spiderweb::io::BitmapWriter<uint32_t, 31, 31> w(id);
 * w.Set(1); //id should be 0x80000000
 *
 * // access 7-13 bit
 * spiderweb::io::BitmapReader<uint32_t, 7, 13> r(id);
 *
 * const auto value = w.Value();
 */

template <typename T, std::size_t low, std::size_t high,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
struct BitmapReader {
  explicit BitmapReader(T v) : value(v) {
  }

  inline T Value() const {
    constexpr auto num = sizeof(T) * 8 - 1 - high;
    static_assert(num >= 0, "bad arg");

    T result = value << num;
    result = result >> (num + low);
    return result;
  }

 private:
  const T value = 0;
};

template <typename T, std::size_t low, std::size_t high,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
struct BitmapWriter {
  explicit BitmapWriter(T &v) : value(v) {
  }

  inline void Set(const T v) {
    T mask = ((1 << (high - low + 1)) - 1) << low;
    value &= ~mask;
    value |= (v << low) & mask;
  }

 private:
  T &value;
};

}  // namespace io
}  // namespace spiderweb
