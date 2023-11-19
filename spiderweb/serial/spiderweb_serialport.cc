#include "spiderweb/serial/spiderweb_serialport.h"

#include "io/private/spiderweb_stream_private.h"
#include "private/spiderweb_serialport_private.h"
#include "spiderweb/core/internal/thread_check.h"

namespace spiderweb {
namespace serial {
SerialPort::SerialPort(Object *parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
}

SerialPort::~SerialPort() {
  SPIDERWEB_CALL_THREAD_CHECK(SerialPort::~SerialPort);
  d->impl.q = nullptr;
}

void SerialPort::Open(const std::string &port) {
  d->StartOpen(d->impl.serial_port, port);
}

void SerialPort::SetParity(Parity parity, std::error_code &ec) {
  d->impl.SetParity(parity, ec);
}

spiderweb::serial::Parity SerialPort::GetParity() const {
  return d->impl.GetParity();
}

void SerialPort::SetBaudRate(BaudRate baudrate, std::error_code &ec) {
  d->impl.SetBaudRate(baudrate, ec);
}

spiderweb::serial::BaudRate SerialPort::GetBaudRate() const {
  return d->impl.GetBaudRate();
}

void SerialPort::SetDataBits(DataBits bits, std::error_code &ec) {
  d->impl.SetDataBits(bits, ec);
}

spiderweb::serial::DataBits SerialPort::GetDataBits() const {
  return d->impl.GetDataBits();
}

void SerialPort::SetStopBits(StopBits stopbits, std::error_code &ec) {
  d->impl.SetStopBits(stopbits, ec);
}

spiderweb::serial::StopBits SerialPort::GetStopBits() const {
  return d->impl.GetStopBits();
}

void SerialPort::Close() {
  SPIDERWEB_CALL_THREAD_CHECK(SerialPort::Close);
  d->Close(d->impl.serial_port);
}

void SerialPort::Write(const uint8_t *data, std::size_t size) {
  SPIDERWEB_CALL_THREAD_CHECK(SerialPort::Write);
  d->StartWrite(d->impl.serial_port, data, size);
}

void SerialPort::Write(const std::vector<uint8_t> &data) {
  SPIDERWEB_CALL_THREAD_CHECK(SerialPort::Write);
  d->StartWrite(d->impl.serial_port, data);
}

}  // namespace serial
}  // namespace spiderweb
