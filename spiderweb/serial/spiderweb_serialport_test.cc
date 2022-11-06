#include "spiderweb/serial/spiderweb_serialport.h"

#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_event_spy.h"

class SerialPortTest : public testing::Test {};

TEST_F(SerialPortTest, Construct) {
  spdlog::set_level(spdlog::level::trace);

  spiderweb::EventLoop          loop;
  spiderweb::serial::SerialPort serial;

  spiderweb::EventSpy spy(&serial, &spiderweb::serial::SerialPort::Error);

  serial.Open("/dev/ttyS1");

  spy.Wait();

  EXPECT_EQ(spy.Count(), 0);
  std::cout << spy.Count() << "\n";
  std::error_code ec;

  // std::tie(ec) = spy.LastResult<std::error_code>();

  serial.SetBaudRate(spiderweb::serial::BaudRate::k115200, ec);
  ASSERT_TRUE(ec.value() == 0) << ec.message();
  EXPECT_TRUE(serial.GetBaudRate() == spiderweb::serial::BaudRate::k115200);
}
