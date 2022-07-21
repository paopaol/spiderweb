#pragma once

#include <arpa/inet.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

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

template <typename T>
class Streamer {
 public:
  ~Streamer() {}

  template <typename Beg, typename End>
  inline std::size_t Write(Beg beg, End end) {
    return static_cast<T &>(*this).Write(beg, end);
  }

  inline void Reset() { static_cast<T &>(*this).Reset(); }

  template <typename U>
  inline const U &Stream() const {
    return static_cast<T &>(*this).Stream();
  }

  template <typename Beg, typename End>
  inline std::size_t Preppend(Beg beg, End end) {
    return static_cast<T &>(*this).Preppend(beg, end);
  }

 private:
};

class DefaultStreamer : public Streamer<DefaultStreamer> {
 public:
  DefaultStreamer() = default;

  template <typename Beg, typename End>
  inline std::size_t Write(Beg beg, End end) {
    vec_.insert(vec_.end(), beg, end);
    return preallocated_;
  }

  inline void Reset() { vec_.clear(); }

  inline const std::vector<uint8_t> &Stream() const { return vec_; }

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
  BinaryWriter(Streamer<StreamerType> &stream) : stream_(stream) {}

  /**
   * @brief return encoded size;
   */
  template <typename T, typename... Args>
  inline std::size_t Size(const T &arg, Args &&...args) {
    return Size(arg) + Size(std::forward<Args>(args)...);
  }

  inline std::size_t Size(const char *data) { return strlen(data); }

  template <std::size_t N, typename T>
  inline std::size_t Size(const arch::EndianConvertor<N, T> &data) {
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

  template <std::size_t N, typename T>
  inline std::size_t Write(const arch::EndianConvertor<N, T> &data) {
    stream_.Write(data.Begin(), data.End());
    return data.Size();
  }

  /**
   * @brief preppend some data
   */
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

  template <std::size_t N, typename T>
  inline std::size_t Preppend(const arch::EndianConvertor<N, T> &data) {
    stream_.Preppend(data.Begin(), data.End());
    return data.Size();
  }

  /**
   * @brief reset the streamer for reuse
   */
  inline void Reset() { stream_.Reset(); }

 private:
  Streamer<StreamerType> &stream_;
};
}  // namespace spiderweb
