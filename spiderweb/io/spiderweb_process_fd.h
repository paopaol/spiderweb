#pragma once

#include <memory>

#include "io/private/spiderweb_stream_private.h"
#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {

namespace io {
template <typename IoImpl>
class IoPrivate;
}

class ProcessFd : public Object {
 public:
  explicit ProcessFd(Object* parent = nullptr);

  ~ProcessFd() override;

  ErrorCode Assign(FILE* f);

  ErrorCode Release();

  void Write(const uint8_t* data, std::size_t size);

  void Write(const std::vector<uint8_t>& data);

  Notify<const std::error_code&> Error;

  Notify<const io::BufferReader&> BytesRead;

  Notify<std::size_t> BytesWritten;

 private:
  class Private;
  std::shared_ptr<io::IoPrivate<Private>> d;
};

}  // namespace spiderweb
