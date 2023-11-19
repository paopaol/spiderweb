#include "io/private/spiderweb_stream_private.h"
#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/io/private/spiderweb_named_pipe_private.h"

namespace spiderweb {

NamedPipe::NamedPipe(const std::string &name, Object *parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
  d->impl.name = name;
}

NamedPipe::~NamedPipe() {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::~NamedPipe);
  d->impl.q = nullptr;
}

void NamedPipe::Open() {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::~Open);
  d->StartOpen(d->impl.pipe);
}

void NamedPipe::Close() {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::~Close);
  d->Close(d->impl.pipe);
}

void NamedPipe::Write(const uint8_t *data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::~Write);
  d->StartWrite(d->impl.pipe, data, size);
}

void NamedPipe::Write(const std::vector<uint8_t> &data) {
  SPIDERWEB_CALL_THREAD_CHECK(NamedPipe::~Write);
  d->StartWrite(d->impl.pipe, data);
}

}  // namespace spiderweb
