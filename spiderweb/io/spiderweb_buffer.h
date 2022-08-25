#ifndef SPIDERWEB_IO_BUFFER_H
#define SPIDERWEB_IO_BUFFER_H

#include <assert.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace spiderweb {
namespace io {
class Buffer {
 public:
  Buffer();
  ~Buffer() = default;

  // Read All
  size_t Read(std::vector<char> &p);

  // Read One Byte
  char ReadByte();

  // Read N Bytes from buffer
  size_t ReadBytes(std::vector<char> &p, size_t n);

  size_t Read(char *buffer, size_t n);
  size_t ZeroCopyRead(char *&ptr, size_t n);

  // write data into buffer
  size_t Write(const char *d, size_t len);

  size_t Write(const std::string &s);

  size_t Write(const std::vector<char> &p);

  void UnReadByte(/*error*/);

  void UnReadBytes(size_t n /*,error &e*/);

  // return unreaded data size
  size_t Len();

  size_t Cap();

  void Reset();

  bool PeekAt(std::vector<char> &p, size_t index, size_t size);

  void Optimization();

  void PrepareWrite(std::size_t n);

  void CommitWrite(std::size_t n);

  // ReadFrom
  // WriteTo

  char  *lastRead();
  char  *beginWrite();
  size_t leftSpace();

 private:
  void growSpace(size_t len);

  void hasWritten(size_t len);

  void hasReaded(size_t len);

  const char *beginWrite() const;

  const char *beginRead() const;

  char *begin();

  const char *begin() const;

  std::vector<char> b;
  size_t            ridx;
  size_t            widx;
};

}  // namespace io
}  // namespace spiderweb

#endif
