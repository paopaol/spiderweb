#include "spiderweb/serial/spiderweb_socketcan.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <spiderweb/io/spiderweb_binary_writer.hpp>

#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/ppk_assert.h"

namespace spiderweb {

TEST(canary, Send) {
  uint32_t          index = 0;
  EventLoop         loop;
  serial::SocketCan can;

  Object::Connect(&can, &serial::SocketCan::Error, &loop, [&](const std::error_code& ec) {
    fprintf(stderr, "error %s\n", ec.message().c_str());
    loop.Quit();
  });

  const auto send = [&]() {
    serial::CanFrame f;

    f.header.id(index++);
    f.header.payload_length(8);
    f.payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    can.Write(f);
  };

  Object::Connect(&can, &serial::SocketCan::OpenSuccess, &loop, send);

  Object::Connect(&can, &serial::SocketCan::BytesWritten, &loop, [&](std::size_t size) {
    if (size != 16) {
      fprintf(stderr, "BytesWritten %zu\n", size);
      abort();
    }

    loop.RunAfter(0, [&]() { send(); });
  });

  can.Open("can1", serial::SocketCanType::kRaw);

  loop.Exec();
}
}  // namespace spiderweb
