#ifndef SPIDERWEB_SERIALPORT_PRIVATE_H
#define SPIDERWEB_SERIALPORT_PRIVATE_H

#include <map>

#include "asio.hpp"
#include "core/spiderweb_eventloop.h"
#include "reflect/reflect_enum.hpp"
#include "spiderweb/serial/spiderweb_serialport.h"

REFLECT_ENUM(spiderweb::serial::SerialPort::StopBits, asio::serial_port::stop_bits::type,
             (kOne, asio::serial_port::stop_bits::one), (kTwo, asio::serial_port::stop_bits::two))

REFLECT_ENUM(spiderweb::serial::SerialPort::DataBits, int, (k5, 5), (k6, 6), (k7, 7), (k8, 8))

REFLECT_ENUM(spiderweb::serial::SerialPort::BaudRate, int, (k1200, 1200), (k2400, 2400),
             (k4800, 4800), (k9600, 9600), (k19200, 19200), (k38400, 38400), (k57600, 57600),
             (k115200, 115200))

REFLECT_ENUM(spiderweb::serial::SerialPort::Parity, asio::serial_port::parity::type,
             (kNoParity, asio::serial_port::parity::none),
             (kEvenParity, asio::serial_port::parity::even),
             (kOddParity, asio::serial_port::parity::odd))

namespace spiderweb {
namespace serial {

class SerialPort::Private {
 public:
  explicit Private(const std::string &port, SerialPort *qq)
      : q(qq), serial_port(qq->ownerEventLoop()->IoService(), port) {
  }

  void SetParity(Parity parity) {
    serial_port.set_option(asio::serial_port_base::parity(
        reflect::ToAliasType<asio::serial_port::parity::type>(parity)));
  }

  void SetBaudRate(BaudRate baudrate) {
    serial_port.set_option(asio::serial_port::baud_rate(reflect::ToAliasType<int>(baudrate)));
  }

  void SetDataBits(DataBits bits) {
    serial_port.set_option(asio::serial_port::character_size(reflect::ToAliasType<int>(bits)));
  }

  void SetStopBits(StopBits stopbits) {
    serial_port.set_option(asio::serial_port::stop_bits(
        reflect::ToAliasType<asio::serial_port::stop_bits::type>(stopbits)));
  }

  SerialPort       *q = nullptr;
  asio::serial_port serial_port;
};

}  // namespace serial
}  // namespace spiderweb

#endif
