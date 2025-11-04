#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace spiderweb {
namespace net {

class EndPoint {
 public:
  EndPoint(const std::string& ip, uint16_t port);

  EndPoint() = default;

  ~EndPoint() = default;

  std::string String() const;

  std::string IpString() const;

  inline uint16_t Port() const {
    return port_;
  }

  inline bool IsIpv6() const {
    return is_ipv6_;
  }

  inline bool operator==(const EndPoint& other) const {
    return port_ == other.port_ && is_ipv6_ == other.is_ipv6_ && addr_bytes_ == other.addr_bytes_;
  }

  inline bool operator!=(const EndPoint& other) const {
    return !(*this == other);
  }

 private:
  std::array<uint8_t, 16> addr_bytes_ = {0};
  uint16_t                port_ = 0;
  bool                    is_ipv6_ = false;

  friend struct EndPointVisitor;
};
}  // namespace net
}  // namespace spiderweb
