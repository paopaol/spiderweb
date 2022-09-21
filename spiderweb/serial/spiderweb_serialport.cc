#include "spiderweb_serialport.h"

#include "core/internal/thread_check.h"
#include "io/private/spiderweb_stream_private.h"
#include "private/spiderweb_serialport_private.h"

namespace spiderweb {
namespace serial {
SerialPort::SerialPort(const std::string &port, Object *parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(port, this)) {
}

SerialPort::~SerialPort() {
  SPIDERWEB_CALL_THREAD_CHECK(SerialPort::~SerialPort);
  d->impl.q = nullptr;
}

void SerialPort::SetBaudRate(BaudRate baudrate) {
  d->impl.SetBaudRate(baudrate);
}

void SerialPort::SetDataBits(DataBits bits) {
  d->impl.SetDataBits(bits);
}

void SerialPort::SetParity(Parity parity) {
  d->impl.SetParity(parity);
}

void SerialPort::SetStopBits(StopBits stopbits) {
  d->impl.SetStopBits(stopbits);
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
