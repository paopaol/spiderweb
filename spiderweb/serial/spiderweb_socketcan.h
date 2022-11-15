#ifndef SPIDERWEB_SOCKETCAN_H
#define SPIDERWEB_SOCKETCAN_H

#include "canary/filter.hpp"
#include "canary/frame_header.hpp"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {

namespace io {
template <typename IoImpl>
class IoPrivate;
}

namespace serial {
struct CanFrame {
  canary::frame_header        header;
  std::array<std::uint8_t, 8> payload{0};
};

enum class SocketCanType { kRaw, kIsotp };

class SocketCan final : public Object {
 public:
  class Private;

  explicit SocketCan(Object *parent = nullptr);

  ~SocketCan() override;

  void Open(const std::string &can, SocketCanType type);

  void Close();

  void Write(const CanFrame &frame);

  EventSignal<void()> OpenSuccess;

  EventSignal<void(const std::error_code &ec)> Error;

  EventSignal<void(const CanFrame &frame)> FrameRead;

  EventSignal<void(std::size_t n)> BytesWritten;

 private:
  std::shared_ptr<io::IoPrivate<Private>> d;
};

}  // namespace serial
}  // namespace spiderweb

#endif
