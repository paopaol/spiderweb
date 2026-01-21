#ifndef SPIDER_WEB_TCP_SOCKET_H
#define SPIDER_WEB_TCP_SOCKET_H

#include <system_error>

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {

namespace io {
template <typename IoImpl>
class IoPrivate;
}

namespace net {

class TcpServer;
class TcpSocket : public Object {
 public:
  class Private;

  explicit TcpSocket(Object* parent = nullptr);

  ~TcpSocket() override;

  void SetNoDelay(bool flag, spiderweb::ErrorCode& ec);

  void SetSendBufferSize(uint16_t size, spiderweb::ErrorCode& ec);

  void ConnectToHost(const std::string& ip, uint16_t port);

  void DisConnectFromHost();

  bool IsClosed() const;

  std::string LocalStringAddress() const;

  std::string RemoteStringAddress() const;

  /**
   * @brief
   */
  // bool EnableReconnectWhenLost(bool flag, uint32_t delay_ms);

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

  Notify<const io::BufferReader&> BytesRead;

  Notify<std::size_t> BytesWritten;

  /**
   * @brief when enable reconnect, if the connection is lost,
   *
   * TcpSocket will emit this signal to notify the user, it will reconnect.
   */
  Notify<> ReconnectEvent;

 private:
  explicit TcpSocket(TcpServer* parent);

  std::shared_ptr<io::IoPrivate<Private>> d;

  friend class TcpServer;
};
}  // namespace net

}  // namespace spiderweb
#endif
