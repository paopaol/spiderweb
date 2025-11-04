#include <absl/strings/str_cat.h>

#include <asio.hpp>

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/net/spiderweb_endpoint.h"

namespace spiderweb {
namespace net {
using asio::ip::address_v4;
using asio::ip::address_v6;
using asio::ip::tcp;
using asio::ip::udp;

struct EndPointVisitor {
  template <typename T>
  static typename T::endpoint MakeAsioEndpoint(const EndPoint& from) {
    if (from.IsIpv6()) {
      address_v6::bytes_type v6_bytes;

      std::copy(from.addr_bytes_.begin(), from.addr_bytes_.end(), v6_bytes.begin());
      return typename T::endpoint(address_v6(v6_bytes), from.Port());
    } else {
      address_v4::bytes_type v4_bytes;

      std::copy(from.addr_bytes_.begin(), from.addr_bytes_.begin() + 4, v4_bytes.begin());
      return typename T::endpoint(address_v4(v4_bytes), from.Port());
    }
  }

  template <typename T>
  static EndPoint FromAsioEndpoint(const T& from) {
    EndPoint          to;
    asio::ip::address asio_addr = from.address();

    to.is_ipv6_ = asio_addr.is_v6();
    to.port_ = from.port();

    if (to.is_ipv6_) {
      auto v6_bytes = asio_addr.to_v6().to_bytes();
      std::copy(v6_bytes.begin(), v6_bytes.end(), to.addr_bytes_.begin());
    } else {
      auto v4_bytes = asio_addr.to_v4().to_bytes();
      std::copy(v4_bytes.begin(), v4_bytes.end(), to.addr_bytes_.begin());
    }
    return to;
  }
};

}  // namespace net
}  // namespace spiderweb
