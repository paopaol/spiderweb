#pragma once

#include <memory>
#include <string>
#include <vector>

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_notify.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {
class Process : public Object {
 public:
  enum class Channnel : uint8_t {
    kStdOut,
    kStdErr,
  };

  enum class State : uint8_t {
    kNotRunning,
    kStarting,
    kRunning,
    kStopping,
  };

  explicit Process(Object* parent = nullptr);

  ~Process() override;

  void SetProgram(std::vector<std::string> cmdline);

  ErrorCode Start();

  ErrorCode AsyncStop();

  void SetEnv(const std::string& name, const std::string& value);

  void SetWorkingDir(const std::string& dir);

  void EnableParentEnv(bool flag);

  bool IsRunning() const;

  State GetState() const;

  /**
   * @brief 用户测必须关注该事件，否则会内存泄露,
   *
   * 如果不需要，可以扔掉
   */
  Notify<Channnel, const io::BufferReader&> BytesRead;

  Notify<State> StateChanged;

  Notify<int> Stopped;

 private:
  class Private;
  std::unique_ptr<Private> d;
};

}  // namespace spiderweb
