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
class SerialPort final : public Object {
 public:
  class Private;

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

  explicit SerialPort(const std::string &port, Object *parent = nullptr);

  ~SerialPort() override;

  void SetParity(Parity parity);

  void SetBaudRate(BaudRate baudrate);

  void SetDataBits(DataBits bits);

  void SetParity();

  void SetStopBits(StopBits stopbits);

  void Close();

  void Write(const uint8_t *data, std::size_t size);

  void Write(const std::vector<uint8_t> &data);

  EventSignal<void(const std::error_code &ec)> Error;

  EventSignal<void(const io::BufferReader &buffer)> BytesRead;

  EventSignal<void(std::size_t n)> BytesWritten;

 private:
  std::shared_ptr<io::IoPrivate<Private>> d;
};
}  // namespace serial
}  // namespace spiderweb

#endif
