#pragma once

#include <system_error>

#include "spiderweb/core/spiderweb_eventsignal.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {
namespace io {
template <typename IoImpl>
class IoPrivate;
}

class NamedPipe : public Object {
  class Private;

 public:
  explicit NamedPipe(const std::string &name, Object *parent = nullptr);

  ~NamedPipe() override;

  void Open();

  void Close();

  void Write(const uint8_t *data, std::size_t size);

  void Write(const std::vector<uint8_t> &data);

  EventSignal<void()> OpenSuccess;

  EventSignal<void(const std::error_code &ec)> Error;

  EventSignal<void(const io::BufferReader &buffer)> BytesRead;

  EventSignal<void(std::size_t n)> BytesWritten;

 private:
  std::shared_ptr<io::IoPrivate<Private>> d;
};
}  // namespace spiderweb
