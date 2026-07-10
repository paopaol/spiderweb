#pragma once

#include <system_error>

#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {

namespace io {
template <typename IoImpl>
class IoPrivate;
}

namespace net {

class UdsServer;
class UdsSocket : public Object {
 public:
  class Private;

  explicit UdsSocket(Object* parent = nullptr);

  ~UdsSocket() override;

  void SetSendBufferSize(uint16_t size);

  void ConnectTo(const std::string& uds_address);

  void DisConnect();

  bool IsClosed() const;

  std::string StringAddress() const;

  /**
   * @brief Write something to the remote peer.
   *
   * Note that the write is asynchronous though. But it does not support multiple calls to write.
   *
   * When you initiate a write call, it is not allowed to write again until the write operation is
   *
   * completed(see BytesWritten). The behavior of multiple writes is undefined.
   */
  void Write(const uint8_t* data, std::size_t size);

  void Write(const std::vector<uint8_t>& data);

  Notify<> ConnectionEstablished;

  Notify<const std::error_code&> ConnectError;

  Notify<const std::error_code&> Error;

  Notify<const std::error_code&> SetOptionError;

  Notify<const io::BufferReader&> BytesRead;

  Notify<std::size_t> BytesWritten;

  /**
   * @brief when enable reconnect, if the connection is lost,
   *
   * UdsSocket will emit this signal to notify the user, it will reconnect.
   */
  Notify<> ReconnectEvent;

 private:
  explicit UdsSocket(UdsServer* parent);

  std::shared_ptr<io::IoPrivate<Private>> d;

  friend class UdsServer;
};
}  // namespace net

}  // namespace spiderweb
