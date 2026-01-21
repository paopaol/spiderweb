#ifndef SPIDERWEB_SERIALPORT_PRIVATE_H
#define SPIDERWEB_SERIALPORT_PRIVATE_H

#include "asio.hpp"
#include "asio/serial_port.hpp"
#include "core/internal/asio_cast.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/reflect/enum_reflect.h"
#include "spiderweb/serial/spiderweb_serialport.h"

REFLECT_ENUM(spiderweb::serial::StopBits, asio::serial_port::stop_bits::type,
             (kOne, asio::serial_port::stop_bits::one), (kTwo, asio::serial_port::stop_bits::two))

REFLECT_ENUM(spiderweb::serial::DataBits, int, (k5, 5), (k6, 6), (k7, 7), (k8, 8))

REFLECT_ENUM(spiderweb::serial::BaudRate, int, (k1200, 1200), (k2400, 2400), (k4800, 4800),
             (k9600, 9600), (k19200, 19200), (k38400, 38400), (k57600, 57600), (k115200, 115200))

REFLECT_ENUM(spiderweb::serial::Parity, asio::serial_port::parity::type,
             (kNoParity, asio::serial_port::parity::none),
             (kEvenParity, asio::serial_port::parity::even),
             (kOddParity, asio::serial_port::parity::odd))

namespace spiderweb {
namespace serial {

class SerialPort::Private {
 public:
  explicit Private(SerialPort* qq) : q(qq), serial_port(AsioService(qq->ownerEventLoop())) {
  }

  template <typename AsyncStream, typename Handler>
  void Open(AsyncStream& stream, const std::string& port, Handler&& handler) {
    std::error_code ec;
    stream.open(port, ec);
    q->QueueTask([ec, h = std::forward<Handler>(handler)]() { h(ec); });
  }

  void SetParity(Parity parity, std::error_code& ec) {
    asio::serial_port::parity::type v;
    reflect::MapTo(parity, v);
    (void)serial_port.set_option(asio::serial_port_base::parity(v), ec);
  }

  Parity GetParity() const {
    asio::serial_port_base::parity opt;
    serial_port.get_option(opt);

    return reflect::MapFrom(opt.value(), Parity::kNoParity);
  }

  void SetBaudRate(BaudRate baudrate, std::error_code& ec) {
    int v;
    reflect::MapTo(baudrate, v);

    (void)serial_port.set_option(asio::serial_port::baud_rate(v), ec);
  }

  BaudRate GetBaudRate() const {
    asio::serial_port_base::baud_rate opt;

    serial_port.get_option(opt);

    return reflect::MapFrom(opt.value(), BaudRate::k115200);
  }

  void SetDataBits(DataBits bits, std::error_code& ec) {
    int v;
    reflect::MapTo(bits, v);
    (void)serial_port.set_option(asio::serial_port::character_size(v), ec);
  }

  DataBits GetDataBits() const {
    asio::serial_port_base::character_size opt;

    serial_port.get_option(opt);

    return reflect::MapFrom(opt.value(), DataBits::k8);
  }

  void SetStopBits(StopBits stopbits, std::error_code& ec) {
    asio::serial_port::stop_bits::type v;

    reflect::MapTo(stopbits, v);

    (void)serial_port.set_option(asio::serial_port::stop_bits(v), ec);
  }

  StopBits GetStopBits() const {
    asio::serial_port_base::stop_bits opt;

    serial_port.get_option(opt);

    return reflect::MapFrom(opt.value(), StopBits::kOne);
  }

  //////////////

  inline const char* Description() const {
    return "SerialPort";
  }

  template <typename AsyncStream, typename Handler>
  void Read(AsyncStream& stream, const asio::mutable_buffers_1& buffer, Handler&& handler) {
    stream.async_read_some(buffer, std::forward<Handler>(handler));
  }

  template <typename AsyncStream, typename Handler>
  void Write(AsyncStream& stream, const asio::mutable_buffers_1& buffers, Handler&& handler) {
    asio::async_write(stream, buffers, asio::transfer_all(), std::forward<Handler>(handler));
  }

  void Readden(const io::BufferReader& reader) {
    spider_emit q->BytesRead(reader);
  }

  void Written(std::size_t size) {
    spider_emit q->BytesWritten(size);
  }

  void Error(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &SerialPort::Error, ec);
  }

  template <typename AsyncStream>
  void Close(AsyncStream& stream) {
    std::error_code ec;
    stream.close(ec);
  }

  void OpenSuccess() {
    spider_emit Object::Emit(q, &SerialPort::OpenSuccess);
  }

  void OpenFailed(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &SerialPort::OpenFailed, ec);
  }

  SerialPort*       q = nullptr;
  asio::serial_port serial_port;
};

}  // namespace serial
}  // namespace spiderweb

#endif
