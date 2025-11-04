#pragma once

#include <memory>
#include <system_error>

#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/net/spiderweb_endpoint.h"

namespace spiderweb {
namespace net {

class Datagram {
 public:
  Datagram();

  inline const EndPoint& GetEndPoint() const {
    return endpoint_;
  }

  inline const std::vector<uint8_t> Data() const {
    return data_;
  }

 private:
  Datagram(const EndPoint& endpoint, const uint8_t* data, std::size_t size)
      : endpoint_(endpoint), data_(data, data + size) {
  }

  EndPoint             endpoint_;
  std::vector<uint8_t> data_;

  friend class UdpSocket;
};

class UdpSocket : public Object {
 public:
  class Private;

  explicit UdpSocket(Object* parent = nullptr);

  ~UdpSocket() override;

  void Open();

  void SendTo(const EndPoint& endpoint, const uint8_t* data, std::size_t size);

  Notify<const std::error_code&> Error;

  Notify<std::size_t> BytesWritten;

  Notify<Datagram> DatagramArrived;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace net
}  // namespace spiderweb
