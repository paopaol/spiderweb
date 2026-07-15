#include "spiderweb/io/spiderweb_process_fd.h"

#include <cstdio>
#include <memory>

#include "io/private/spiderweb_process_fd_private.h"
#include "io/private/spiderweb_stream_private.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/spiderweb_check.h"

#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace spiderweb {

#if defined(_WIN32)
static ErrorCode assign_stream(fd_stream& stream, FILE* f) {
  int fd = _fileno(f);
  SPIDERWEB_VERIFY(fd >= 0, return InvalidArgument("bad_file_descriptor"));

  HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  SPIDERWEB_VERIFY(handle != INVALID_HANDLE_VALUE, return InvalidArgument("bad_file_descriptor"));

  DWORD type = GetFileType(handle);
  if (type == FILE_TYPE_DISK) {
    return InvalidArgument("not support");
  }

  ErrorCode ec;
  stream.assign(fd, ec);
  if (ec) {
    return ec;
  }
}
#else
static ErrorCode assign_stream(fd_stream& stream, FILE* f) {
  int fd = fileno(f);
  SPIDERWEB_VERIFY(fd >= 0, return InvalidArgument("bad_file_descriptor"));

  struct stat stat;
  if (fstat(fd, &stat) == -1) {
    return InvalidArgument("bad_file_descriptor");
  }

  if (S_ISREG(stat.st_mode)) {
    return InvalidArgument("no support");
  }

  ErrorCode ec;
  stream.assign(fd, ec);
  if (ec) {
    return ec;
  }

  return Ok();
}
#endif

ProcessFd::ProcessFd(Object* parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
}

ProcessFd::~ProcessFd() {
  SPIDERWEB_CALL_THREAD_CHECK(Fd::~Fd);
  d->impl.q = nullptr;
}

ErrorCode ProcessFd::Assign(FILE* f) {
  SPIDERWEB_VERIFY(f, return InvalidArgument());
  auto ec = assign_stream(d->impl.stream, f);
  if (ec) {
    return ec;
  }
  d->stopped = false;
  d->StartRead(d->impl.stream);
  return Ok();
}

ErrorCode ProcessFd::Release() {
  d->impl.stream.release();
  return Ok();
}

void ProcessFd::Write(const uint8_t* data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(Fs::Write);
  d->StartWrite(d->impl.stream, data, size);
}

void ProcessFd::Write(const std::vector<uint8_t>& data) {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::Write);
  d->StartWrite(d->impl.stream, data);
}

}  // namespace spiderweb
