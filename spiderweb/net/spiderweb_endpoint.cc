#include "spiderweb/net/spiderweb_endpoint.h"

#include "spiderweb_endpoint_visitor.h"

namespace spiderweb {
namespace net {

EndPoint::EndPoint(const std::string& ip, uint16_t port) : port_(port) {
  asio::error_code  ec;
  asio::ip::address addr = asio::ip::make_address(ip, ec);

  if (ec) {
    return;
  }

  is_ipv6_ = addr.is_v6();

  if (is_ipv6_) {
    address_v6::bytes_type v6_bytes = addr.to_v6().to_bytes();
    std::copy(v6_bytes.begin(), v6_bytes.end(), addr_bytes_.begin());
  } else {
    address_v4::bytes_type v4_bytes = addr.to_v4().to_bytes();
    std::copy(v4_bytes.begin(), v4_bytes.end(), addr_bytes_.begin());
  }
}

std::string EndPoint::String() const {
  return absl::StrCat(IpString(), ":", port_);
}

std::string EndPoint::IpString() const {
  spiderweb::ErrorCode ec;

  return EndPointVisitor::MakeAsioEndpoint<tcp>(*this).address().to_string(ec);
}
}  // namespace net
}  // namespace spiderweb
