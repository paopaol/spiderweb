#include "spiderweb/core/spiderweb_process.h"

#include <absl/strings/str_cat.h>

#include <cassert>
#include <cstdio>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

#include "spiderweb/core/internal/thread_check.h"
#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/core/spiderweb_timer.h"
#include "spiderweb/io/spiderweb_process_fd.h"
#include "spiderweb/spiderweb_check.h"
#include "subprocess.h"

namespace spiderweb {
static std::error_code system_error() {
#if defined(_WIN32)
  return std::error_code(static_cast<int>(GetLastError()), std::system_category()));
#else
  return std::make_error_code(static_cast<std::errc>(errno));
#endif
}

static std::vector<const char*> ptr_vec(const std::vector<std::string>& vec) {
  std::vector<const char*> result;

  result.reserve(vec.size() + 1);

  for (const auto& el : vec) {
    result.push_back(el.c_str());
  }

  result.push_back(nullptr);

  return result;
}
static std::vector<std::string> env_vec(const std::unordered_map<std::string, std::string>& env) {
  std::vector<std::string> vec;

  vec.reserve(env.size() + 1);
  for (const auto& it : env) {
    vec.push_back(absl::StrCat(it.first, "=", it.second));
  }
  return vec;
}

class Process::Private {
 public:
  explicit Private(Process* q) : q(q), alive_timer(q) {
  }

  Process*                                     q = nullptr;
  Timer                                        alive_timer;
  State                                        state = State::kNotRunning;
  ProcessFd*                                   stdo = nullptr;
  ProcessFd*                                   stde = nullptr;
  struct subprocess_s                          proc;
  std::unordered_map<std::string, std::string> env;
  std::string                                  cwd;
  bool                                         enable_parent_env = false;
  std::vector<std::string>                     cmdline;

  bool TryCleanup(int& code);

  void SetupFd(ProcessFd* fd, FILE* f, Channnel ch);

  void SetState(State state);
};

bool Process::Private::TryCleanup(int& code) {
  if (subprocess_alive(&proc)) {
    return false;
  }

  if (stde || stdo) {
    return false;
  }

  subprocess_join(&proc, &code);
  subprocess_destroy(&proc);
  SetState(State::kNotRunning);
  return true;
}

void Process::Private::SetupFd(ProcessFd* fd, FILE* f, Channnel ch) {
  Connect(fd, &ProcessFd::BytesRead, fd,
          [this, ch](const io::BufferReader& reader) { spider_emit q->BytesRead(ch, reader); });

  Connect(fd, &ProcessFd::Error, fd, [this, fd, ch](const std::error_code&) {
    fd->Release();
    fd->DeleteLater();

    if (ch == Channnel::kStdErr) {
      stde = nullptr;
    } else {
      stdo = nullptr;
    }
  });
  fd->Assign(f);
}

void Process::Private::SetState(State state) {
  if (this->state != state) {
    this->state = state;
    spider_emit q->StateChanged(state);
  }
}

Process::Process(Object* parent) : Object(parent), d(std::make_unique<Private>(this)) {
  d->alive_timer.SetInterval(50);
  d->alive_timer.SetSingalShot(true);

  Connect(&d->alive_timer, &Timer::timeout, this, [this]() {
    int  code = 0;
    bool ok = d->TryCleanup(code);
    if (!ok) {
      d->alive_timer.Start();
      return;
    }
    spider_emit Stopped(code);
  });
}

Process::~Process() {
  d->alive_timer.Stop();

  if (d->stdo) {
    d->stdo->Release();
    d->stdo->DeleteLater();
    d->stdo = nullptr;
  }

  if (d->stde) {
    d->stde->Release();
    d->stde->DeleteLater();
    d->stde = nullptr;
  }

  int code = 0;
  d->TryCleanup(code);
}

void Process::SetProgram(std::vector<std::string> cmdline) {
  d->cmdline = std::move(cmdline);
}

ErrorCode Process::Start() {
  SPIDERWEB_CALL_THREAD_CHECK(Process::Start);

  SPIDERWEB_VERIFY(d->state == State::kNotRunning, return Ok());

  d->SetState(State::kStarting);

  assert(!d->stdo && !d->stde);

  auto cmdline = ptr_vec(d->cmdline);
  auto env_list = env_vec(d->env);
  auto env = ptr_vec(env_list);

  int options = subprocess_option_enable_async | subprocess_option_no_window |
                subprocess_option_search_user_path;
  if (d->enable_parent_env) {
    options |= subprocess_option_inherit_environment;
  }

  int res = subprocess_create_ex(cmdline.data(), options, env.data(),
                                 d->cwd.empty() ? nullptr : d->cwd.c_str(), &d->proc);
  if (0 != res) {
    d->SetState(State::kNotRunning);
    return MakeErrorCode(system_error(), "start");
  }

  d->SetState(State::kRunning);

  d->stdo = new ProcessFd(this);
  d->stde = new ProcessFd(this);

  d->SetupFd(d->stdo, subprocess_stdout(&d->proc), Channnel::kStdOut);
  d->SetupFd(d->stde, subprocess_stderr(&d->proc), Channnel::kStdErr);

  d->alive_timer.Start();

  return Ok();
}

ErrorCode Process::AsyncStop() {
  SPIDERWEB_CALL_THREAD_CHECK(Process::Stop);
  SPIDERWEB_VERIFY(d->state == State::kRunning,
                   return RuntimeError("not running,maybe stopped or starting"));

  d->SetState(State::kStopping);
  auto ret = subprocess_terminate(&d->proc);

  if (ret != 0) {
    return MakeErrorCode(system_error(), "terminate");
  }

  return Ok();
}

void Process::EnableParentEnv(bool flag) {
  d->enable_parent_env = flag;
}

void Process::SetEnv(const std::string& name, const std::string& value) {
  SPIDERWEB_CALL_THREAD_CHECK(Process::SetEnv);

  d->env[name] = value;
}

void Process::SetWorkingDir(const std::string& dir) {
  SPIDERWEB_CALL_THREAD_CHECK(Process::SetWorkingDir);
  d->cwd = dir;
}

bool Process::IsRunning() const {
  SPIDERWEB_CALL_THREAD_CHECK(Process::IsRunning);

  return GetState() == State::kRunning;
}

Process::State Process::GetState() const {
  return d->state;
}

}  // namespace spiderweb
