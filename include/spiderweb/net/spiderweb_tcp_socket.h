#ifndef SPIDER_WEB_TCP_SOCKET_H
#define SPIDER_WEB_TCP_SOCKET_H

#include <system_error>

#include "absl/types/optional.h"
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
  /**
   * @brief 本机地址，port 为 0 表示由内核分配
   */
  struct LocalEndpoint {
    std::string ip;
    uint16_t    port = 0;
  };

  /**
   * @brief socket 选项，声明式配置
   *
   * 字段为 absl::optional，未设置表示保持原值，因此可以分多次增量声明。
   *
   * - local / recv_buffer_size 只在建连时生效（recv_buffer_size 涉及
   *   窗口缩放的 SYN 协商），连接建立之后再设置会被忽略；
   * - no_delay / send_buffer_size 在 socket 已打开时立即生效，包括 accept
   *   出来的连接。
   */
  struct Options {
    absl::optional<LocalEndpoint> local;
    absl::optional<uint32_t>      send_buffer_size;
    absl::optional<uint32_t>      recv_buffer_size;
    absl::optional<bool>          no_delay;
  };

  class Private;

  explicit TcpSocket(Object* parent = nullptr);

  ~TcpSocket() override;

  /**
   * @brief 设置 socket 选项
   *
   * 在 ConnectToHost 之前或之后都可调用，字段级合并，caller 不需要关心各
   * 选项的设置时机：建连前必须落地的选项由 ConnectToHost 内部统一应用。
   * 运行期（socket 已打开）应用失败通过 SetOptionError 通知。
   */
  void SetOption(const Options& option);

  void ConnectToHost(const std::string& ip, uint16_t port);

  void DisConnectFromHost();

  bool IsClosed() const;

  std::string LocalStringAddress() const;

  std::string RemoteStringAddress() const;

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
