#ifndef SPIDERWEB_TCP_SERVER_H
#define SPIDERWEB_TCP_SERVER_H

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"

namespace spiderweb {
namespace net {
class TcpServer : public Object {
 public:
  class Private;
  explicit TcpServer(uint16_t port, Object* parent = nullptr);

  ~TcpServer() override;

  ErrorCode ListenAndServ(const std::string& ip_v4 = "0.0.0.0");

  /**
   * @brief 监听端口
   *
   * 构造时传 0 表示由内核分配端口，ListenAndServ 之后可用本接口获取实际端口。
   * 未监听时返回 0。
   */
  uint16_t LocalPort() const;

  /**
   * @brief 设置 accept 出来的 socket 的默认选项
   *
   * 可随时调用，对之后 accept 的连接生效
   */
  void SetOption(const TcpSocket::Options& option);

  void Stop();

  Notify<TcpSocket*> InComingConnection;

  Notify<ErrorCode> Stopped;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace net
}  // namespace spiderweb

#endif
