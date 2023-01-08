#include "spiderweb/core/spiderweb_error_code.h"

#include "gtest/gtest.h"

TEST(ErrorCode, Default) {
  spiderweb::ErrorCode e;
  EXPECT_EQ(e.value(), int(spiderweb::ErrC::kOk));
}

TEST(ErrorCode, MakeError) {
  spiderweb::ErrorCode e = spiderweb::MakeErrorCode(spiderweb::ErrC::kResourceError);
  EXPECT_EQ(e.value(), int(spiderweb::ErrC::kResourceError));
}

TEST(ErrorCode, MakeFromStdErrorCode) {
  auto ec = spiderweb::MakeErrorCode(std::make_error_code(std::errc::io_error), "ioe");
  printf("%s\n", ec.FormatedMessage().c_str());
}

TEST(ErrorCode, MakeErrorString) {
  spiderweb::ErrorCode e = spiderweb::MakeErrorCode(spiderweb::ErrC::kResourceError, "invalid");
  EXPECT_EQ(e.value(), int(spiderweb::ErrC::kResourceError));
  EXPECT_EQ(e.LongMessage(), "invalid");
  EXPECT_EQ(e.FormatedMessage(), "ResourceError: invalid");
}

TEST(ErrorCode, Timeout) {
  typedef spiderweb::ErrorCode (*Make)(std::string);
  struct Item {
    Make            op;
    spiderweb::ErrC errc;
    std::string     long_msg;
  };

  Item items[] = {
      {spiderweb::Ok, spiderweb::ErrC::kOk, "ti"},
      {spiderweb::InvalidArgument, spiderweb::ErrC::kInvalidArgument, "ti"},
      {spiderweb::Timeout, spiderweb::ErrC::kTimeout, "ti"},
      {spiderweb::Canceled, spiderweb::ErrC::kCanceled, "ti"},
      {spiderweb::ResourceError, spiderweb::ErrC::kResourceError, "ti"},
      {spiderweb::RuntimeError, spiderweb::ErrC::kRuntimeError, "ti"},
  };

  for (const auto &item : items) {
    auto e = item.op(item.long_msg);

    EXPECT_EQ(e.value(), int(item.errc));
    EXPECT_EQ(e.LongMessage(), "ti");
  }
}
