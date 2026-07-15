#include "spiderweb/core/spiderweb_process.h"

#include <absl/strings/match.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <cstdio>
#include <string>

#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {
class ProcessTest : public testing::Test {
 public:
  void TearDown() override {
    proc.AsyncStop();
    loop.Quit();
    loop.ExecEx();
  }

  EventLoop loop;
  Process   proc;
};

TEST_F(ProcessTest, StdOut) {
  NotifySpy spy(&proc, &Process::BytesRead);

  proc.SetProgram({"echo", "123"});

  proc.Start();

  spy.Wait();
  EXPECT_TRUE(spy.Count() > 0);

  auto [ch, reader] = spy.LastResult<Process::Channnel, io::BufferReader>();

  EXPECT_EQ(ch, Process::Channnel::kStdOut);
  auto span = reader.SpanAt(0, reader.Len());
  EXPECT_THAT(span, testing::ElementsAre('1', '2', '3', '\n'));
}

TEST_F(ProcessTest, StdErr) {
  NotifySpy spy(&proc, &Process::BytesRead);

  proc.SetProgram({"ls", "abc"});

  proc.Start();

  spy.Wait();
  EXPECT_TRUE(spy.Count() > 0);

  auto [ch, reader] = spy.LastResult<Process::Channnel, io::BufferReader>();

  EXPECT_EQ(ch, Process::Channnel::kStdErr);
  auto span = reader.SpanAt(0, reader.Len());
  EXPECT_EQ(std::string((char*)span.data(), span.size()),
            "ls: cannot access 'abc': No such file or directory\n");
}

static std::string pstatus(int status) {
  if (WIFSIGNALED(status)) {
    int sig = WTERMSIG(status);

    const char* abbrev = sigabbrev_np(sig);

    return abbrev;
  } else if (WIFEXITED(status)) {
    int code = WEXITSTATUS(status);
    return std::to_string(code);
  }
  return "";
}

TEST_F(ProcessTest, Kill) {
  NotifySpy spy(&proc, &Process::Stopped);
  proc.SetProgram({"sh", "-c", "kill -s 11 $$"});

  auto ec = proc.Start();
  EXPECT_FALSE(ec);

  spy.Wait();

  auto [status] = spy.LastResult<int>();
  puts(pstatus(status).c_str());
}

TEST_F(ProcessTest, Stop) {
  NotifySpy spy(&proc, &Process::BytesRead);
  proc.SetProgram({"top", "-b"});
  proc.SetEnv("TERM", "XTERM");

  auto ec = proc.Start();
  EXPECT_FALSE(ec);

  spy.Wait();
  auto [ch, reader] = spy.LastResult<Process::Channnel, io::BufferReader>();

  EXPECT_EQ(ch, Process::Channnel::kStdOut);
  auto span = reader.SpanAt(0, reader.Len());
  puts(std::string(reinterpret_cast<char*>(span.data()), span.size()).c_str());
  proc.AsyncStop();
}

}  // namespace spiderweb
