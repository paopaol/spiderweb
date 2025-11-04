#ifndef SPIDERWEB_IO_BUFFER_H
#define SPIDERWEB_IO_BUFFER_H

#include <absl/types/span.h>

#include <cstdint>
#include <string>
#include <vector>

namespace spiderweb {
namespace io {
class Buffer {
 public:
  Buffer();
  ~Buffer() = default;

  // Read All
  size_t Read(std::vector<char>& p);

  // Read One Byte
  char ReadByte();

  // Read N Bytes from buffer
  size_t ReadBytes(std::vector<char>& p, size_t n);

  size_t Read(char* buffer, size_t n);
  size_t ZeroCopyRead(char*& ptr, size_t n);

  // write data into buffer
  size_t Write(const char* d, size_t len);

  size_t Write(const std::string& s);

  size_t Write(const std::vector<char>& p);

  void UnReadByte(/*error*/);

  void UnReadBytes(size_t n /*,error &e*/);

  // return unreaded data size
  size_t Len() const;

  size_t Cap() const;

  void Reset();

  void Skip(uint32_t size);

  bool PeekAt(std::vector<char>& p, size_t index, size_t size);

  bool PointerAt(char** p, size_t index, size_t size);

  void Optimization();

  void PrepareWrite(std::size_t n);

  void CommitWrite(std::size_t n);

  // ReadFrom
  // WriteTo

  char*  lastRead();
  char*  beginWrite();
  size_t leftSpace();

 private:
  void growSpace(size_t len);

  void hasWritten(size_t len);

  void hasReaded(size_t len);

  const char* beginWrite() const;

  const char* beginRead() const;

  char* begin();

  const char* begin() const;

  std::vector<char> b;
  size_t            ridx;
  size_t            widx;
};

class BufferReader {
 public:
  explicit BufferReader(Buffer& buffer) : buffer_(buffer) {
  }

  ~BufferReader() = default;

  // Read All
  inline size_t Read(std::vector<char>& p) const {
    return buffer_.Read(p);
  }

  // Read One Byte
  inline char ReadByte() const {
    return buffer_.ReadByte();
  }

  // Read N Bytes from buffer
  inline size_t ReadBytes(std::vector<char>& p, size_t n) const {
    return buffer_.ReadBytes(p, n);
  }

  inline size_t Read(char* buffer, size_t n) const {
    return buffer_.Read(buffer, n);
  }

  inline size_t ZeroCopyRead(char*& ptr, size_t n) const {
    return buffer_.ZeroCopyRead(ptr, n);
  }

  inline void Skip(uint32_t size) const {
    buffer_.Skip(size);
  }

  inline size_t PointerAt(char*& ptr, size_t index, size_t size) const {
    return buffer_.PointerAt(&ptr, index, size);
  }

  inline absl::Span<uint8_t> SpanAt(size_t index, size_t size) const {
    char* p = nullptr;
    PointerAt(p, index, size);

    return absl::Span<uint8_t>(reinterpret_cast<uint8_t*>(p), size);
  }

  inline bool PeekAt(std::vector<char>& p, size_t index, size_t size) const {
    return buffer_.PeekAt(p, index, size);
  }

  inline size_t Len() const {
    return buffer_.Len();
  }

  inline size_t Cap() const {
    return buffer_.Cap();
  }

  inline void UnReadByte(/*error*/) const {
    buffer_.UnReadByte();
  }

  inline void UnReadBytes(size_t n) const {
    buffer_.UnReadBytes(n);
  }

 private:
  Buffer& buffer_;
};

}  // namespace io
}  // namespace spiderweb

#endif
