#include "spiderweb/io/spiderweb_buffer.h"

#include <cassert>

namespace spiderweb {
namespace io {
Buffer::Buffer() : b(8192), ridx(0), widx(0) {
}

// Read All
size_t Buffer::Read(std::vector<char> &p) {
  return ReadBytes(p, Len());
}

// Read One Byte
char Buffer::ReadByte() {
  char *ch = lastRead();
  hasReaded(1);
  return *ch;
}

// Read N Bytes from buffer
size_t Buffer::ReadBytes(std::vector<char> &p, size_t n) {
  assert(n >= 0 && "buffer::readbytes(), bad input paramer");

  p.clear();
  n = n > Len() ? Len() : n;
  std::copy(lastRead(), lastRead() + n, std::back_inserter(p));
  hasReaded(n);
  return n;
}

size_t Buffer::Read(char *buffer, size_t n) {
  assert(n >= 0 && "buffer::read(), bad input paramer");
  n = n > Len() ? Len() : n;
  std::copy(lastRead(), lastRead() + n, buffer);
  hasReaded(n);
  return n;
}

size_t Buffer::ZeroCopyRead(char *&ptr, size_t n) {
  assert(n >= 0 && "buffer::read(), bad input paramer");
  n = n > Len() ? Len() : n;
  ptr = lastRead();
  hasReaded(n);
  return n;
}

// write data into buffer
size_t Buffer::Write(const char *d, size_t len) {
  PrepareWrite(len);
  std::copy(d, d + len, beginWrite());
  CommitWrite(len);
  return len;
}

size_t Buffer::Write(const std::string &s) {
  return Write(s.c_str(), static_cast<size_t>(s.size()));
}

size_t Buffer::Write(const std::vector<char> &p) {
  return Write(p.data(), static_cast<size_t>(p.size()));
}

void Buffer::UnReadByte(/*error*/) {
  UnReadBytes(1);
}

void Buffer::UnReadBytes(size_t n /*,error &e*/) {
  assert(static_cast<size_t>(lastRead() - begin()) >= n &&
         "buffer::unreadbytes too much data size");
  ridx -= n;
}

// return unreaded data size
size_t Buffer::Len() const {
  return widx - ridx;
}

size_t Buffer::Cap() const {
  return b.size();
}

void Buffer::Reset() {
  ridx = 0;
  widx = 0;
}
void Buffer::Skip(uint32_t size) {
  char *ptr;
  ZeroCopyRead(ptr, size);
}

bool Buffer::PeekAt(std::vector<char> &p, size_t index, size_t size) {
  char *pointer = nullptr;
  if (!PointerAt(&pointer, index, size)) {
    return false;
  }
  std::copy(pointer, pointer + size, std::back_inserter(p));
  return true;
}

bool Buffer::PointerAt(char **p, size_t index, size_t size) {
  if (index < 0 || index >= Len()) {
    return false;
  }
  if (size <= 0) {
    return false;
  }
  index = ridx + index;
  size_t len = widx - index;
  if (size > len) {
    return false;
  }

  *p = b.data() + index;
  return true;
}

void Buffer::Optimization() {
  if (ridx == 0) {
    return;
  }

  size_t len = Len();
  std::copy(begin() + ridx, begin() + widx, begin());
  ridx = 0;
  widx = ridx + len;
  assert(widx < b.size());
}

void Buffer::PrepareWrite(std::size_t n) {
  if (leftSpace() < n) {
    Optimization();
  }
  if (leftSpace() < n) {
    growSpace(static_cast<size_t>(b.size() + n * 2));
  }
}

void Buffer::CommitWrite(std::size_t n) {
  hasWritten(n);
}

void Buffer::growSpace(size_t len) {
  b.resize(widx + len);
}

size_t Buffer::leftSpace() {
  return b.size() - widx;
}

void Buffer::hasWritten(size_t len) {
  widx += len;
  assert(widx <= b.size());
}
void Buffer::hasReaded(size_t len) {
  ridx += len;
}

char *Buffer::beginWrite() {
  return begin() + widx;
}

const char *Buffer::beginWrite() const {
  return begin() + widx;
}

char *Buffer::lastRead() {
  return begin() + ridx;
}

const char *Buffer::beginRead() const {
  return begin() + ridx;
}

char *Buffer::begin() {
  return &*b.begin();
}

const char *Buffer::begin() const {
  return &*b.begin();
}

}  // namespace io
}  // namespace spiderweb
