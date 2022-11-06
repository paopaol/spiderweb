#ifndef SPIDERWEB_SERIALPORT_H
#define SPIDERWEB_SERIALPORT_H

#include "io/spiderweb_buffer.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {
namespace io {

template <typename IoImpl>
class IoPrivate;
}

namespace serial {

enum class StopBits {
  kOne,
  kTwo,
};

enum class BaudRate {
  k1200,
  k2400,
  k4800,
  k9600,
  k19200,
  k38400,
  k57600,
  k115200,
};

enum class DataBits {
  k5,
  k6,
  k7,
  k8,
};

enum class Parity {
  kNoParity,
  kEvenParity,
  kOddParity,
};

class SerialPort final : public Object {
 public:
  class Private;

  explicit SerialPort(Object *parent = nullptr);

  ~SerialPort() override;

  void Open(const std::string &port);

  void SetParity(Parity parity, std::error_code &ec);

  Parity GetParity() const;

  void SetBaudRate(BaudRate baudrate, std::error_code &ec);

  BaudRate GetBaudRate() const;

  void SetDataBits(DataBits bits, std::error_code &ec);

  DataBits GetDataBits() const;

  void SetStopBits(StopBits stopbits, std::error_code &ec);

  StopBits GetStopBits() const;

  void Close();

  void Write(const uint8_t *data, std::size_t size);

  void Write(const std::vector<uint8_t> &data);

  EventSignal<void()> OpenSuccess;

  EventSignal<void(const std::error_code &ec)> Error;

  EventSignal<void(const io::BufferReader &buffer)> BytesRead;

  EventSignal<void(std::size_t n)> BytesWritten;

 private:
  std::shared_ptr<io::IoPrivate<Private>> d;
};
}  // namespace serial
}  // namespace spiderweb

#endif
