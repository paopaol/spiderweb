#pragma once



#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "spiderweb/arch/spiderweb_arch.hpp"

namespace spiderweb {
namespace io {
template <typename T>
class Streamer {
 public:
  ~Streamer() = default;

  template <typename Beg, typename End>
  inline std::size_t Write(Beg beg, End end) {
    return static_cast<T &>(*this).Write(beg, end);
  }

  inline void Reset() {
    static_cast<T &>(*this).Reset();
  }

  template <typename U>
  inline const U &Stream() const {
    return static_cast<T &>(*this).Stream();
  }

  template <typename Beg, typename End>
  inline std::size_t Preppend(Beg beg, End end) {
    return static_cast<T &>(*this).Preppend(beg, end);
  }
};

class DefaultStreamer : public Streamer<DefaultStreamer> {
 public:
  explicit DefaultStreamer(std::size_t cap = 128) {
    vec_.reserve(cap);
  }

  template <typename Beg, typename End>
  inline std::size_t Write(Beg beg, End end) {
    std::copy(beg, end, std::back_inserter(vec_));
    return preallocated_;
  }

  inline void Reset() {
    vec_.clear();
  }

  inline const std::vector<uint8_t> &Stream() const {
    return vec_;
  }

  template <typename Beg, typename End>
  inline std::size_t Preppend(Beg beg, End end) {
    assert((end - beg) == preallocated_);
    std::copy(beg, end, vec_.begin());
    return end - beg;
  }

  void PreAllocate(int size) {
    vec_.resize(size);
    preallocated_ = size;
  }

 private:
  std::vector<uint8_t> vec_;
  int                  preallocated_ = 0;
};

/**
 * @brief BinaryWriter is a bytes writer for binary data,
 *
 * it write all data into a stream provided by the user.
 *
 * the binary data can be integral, float, or double,
 *
 * for integral value, they can be encoded to BigEndian or HostEndian.
 *
 * for bytes value,char *,uint8_t *, direct copy into streamer.
 *
 * we can also preppend some data at the beginning of the streamer.
 *
 * thereis a default streamer for BinaryWriter named DefaultStreamer.
 *
 * you cal also  write customized Streamer. see DefaultStreamer
 */
template <typename StreamerType>
class BinaryWriter {
 public:
  explicit BinaryWriter(Streamer<StreamerType> &stream) : stream_(stream) {
  }

  /**
   * @brief return encoded size;
   */
  template <typename T, typename... Args>
  inline std::size_t Size(const T &arg, Args &&...args) {
    return Size(arg) + Size(std::forward<Args>(args)...);
  }

  inline std::size_t Size(const char *data) {
    return strlen(data);
  }

  template <arch::ArchType type, std::size_t N, typename T>
  inline std::size_t Size(const arch::EndianConvertor<type, N, T> &data) {
    return data.Size();
  }

  /**
   * @brief write data into stramer,
   *
   * return bytes written.
   */
  template <typename T, typename... Args>
  inline std::size_t Write(const T &arg, Args &&...args) {
    return Write(arg) + Write(std::forward<Args>(args)...);
  }

  inline std::size_t Write(uint8_t data) {
    stream_.Write(&data, &data + 1);
    return 1;
  }

  inline std::size_t Write(const char *data) {
    const auto size = strlen(data);
    stream_.Write(data, data + size);
    return size;
  }

  inline std::size_t Write(const std::string &data) {
    stream_.Write(data.begin(), data.end());
    return data.size();
  }

  inline std::size_t Write(const std::vector<uint8_t> &data) {
    stream_.Write(data.begin(), data.end());
    return data.size();
  }

  template <std::size_t N>
  inline std::size_t Write(const std::array<uint8_t, N> &data) {
    stream_.Write(data.begin(), data.end());
    return data.size();
  }

  template <arch::ArchType type, std::size_t N, typename T>
  inline std::size_t Write(const arch::EndianConvertor<type, N, T> &data) {
    stream_.Write(data.Begin(), data.End());
    return data.Size();
  }

  /**
   * @brief preppend some data
   */
  inline std::size_t Preppend(uint8_t data) {
    stream_.Preppend(&data, &data + 1);
    return 1;
  }

  inline std::size_t Preppend(const char *data) {
    const auto size = strlen(data);
    stream_.Preppend(data, data + size);
    return size;
  }

  inline std::size_t Preppend(const std::string &data) {
    stream_.Preppend(data.begin(), data.end());
    return data.size();
  }

  inline std::size_t Preppend(const std::vector<uint8_t> &data) {
    stream_.Preppend(data.begin(), data.end());
    return data.size();
  }

  template <arch::ArchType type, std::size_t N, typename T>
  inline std::size_t Preppend(const arch::EndianConvertor<type, N, T> &data) {
    stream_.Preppend(data.Begin(), data.End());
    return data.Size();
  }

  /**
   * @brief reset the streamer for reuse
   */
  inline void Reset() {
    stream_.Reset();
  }

 private:
  Streamer<StreamerType> &stream_;
};
}  // namespace io
}  // namespace spiderweb
