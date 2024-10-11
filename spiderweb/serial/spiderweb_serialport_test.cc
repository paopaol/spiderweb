#include "spiderweb/serial/spiderweb_serialport.h"

#include "ghc/filesystem.hpp"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"

class SerialPortTest : public testing::Test {};

TEST_F(SerialPortTest, Construct) {
  spdlog::set_level(spdlog::level::trace);

  spiderweb::EventLoop          loop;
  spiderweb::serial::SerialPort serial;

  spiderweb::NotifySpy spy(&serial, &spiderweb::serial::SerialPort::Error);

  serial.Open("/dev/ttyS1");

  spy.Wait();

  if (ghc::filesystem::exists("/dev/ttyS1")) {
    EXPECT_EQ(spy.Count(), 1);
  } else {
    EXPECT_EQ(spy.Count(), 0);
  }
}
