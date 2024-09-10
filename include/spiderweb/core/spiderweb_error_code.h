#pragma once

#include <system_error>

namespace spiderweb {

class ErrorCode : public std::error_code {
 public:
  ErrorCode() : std::error_code() {
  }

  ErrorCode(int v, const std::error_category &cat) : std::error_code(v, cat) {
  }

  ErrorCode(int v, const std::error_category &cat, std::string long_error)
      : std::error_code(v, cat), long_error_(std::move(long_error)) {
  }

  template <typename ErrorCodeEnum,
            typename = typename std::enable_if<std::is_error_code_enum<ErrorCodeEnum>::value>::type>
  explicit ErrorCode(ErrorCodeEnum e) noexcept : std::error_code(e) {
  }

  ErrorCode(ErrorCode &&other) noexcept {
    *this = std::move(other);
  }

  ErrorCode(const ErrorCode &other) : error_code(other) {
    long_error_ = other.long_error_;
  }

  ErrorCode &operator=(ErrorCode &&other) noexcept {
    if (this != &other) {
      long_error_ = std::move(other.long_error_);
      SetErrorCode(other);
    }
    return *this;
  }

  ErrorCode &operator=(const ErrorCode &other) {
    if (this != &other) {
      long_error_ = std::move(other.long_error_);
      SetErrorCode(other);
    }

    return *this;
  }

  ~ErrorCode() = default;

  inline void SetErrorCode(std::error_code ec) {
    std::error_code &self = *this;
    self = ec;
  }

  inline const std::string &LongMessage() const {
    return long_error_;
  }

  inline void SetLongMessage(const std::string &msg) {
    long_error_ = msg;
  }

  std::string FormatedMessage() const;

 private:
  std::string long_error_;
};

enum class ErrC {
  kOk = 0,
  kInvalidArgument,
  kTimeout,
  kCanceled,
  kResourceError,
  kRuntimeError,
};

ErrorCode MakeErrorCode(ErrC e);

ErrorCode MakeErrorCode(ErrC e, std::string long_err);

ErrorCode MakeErrorCode(std::error_code e, const std::string &long_err);

ErrorCode Ok(std::string long_err = "");

ErrorCode InvalidArgument(std::string long_err = "");

ErrorCode Timeout(std::string long_err = "");

ErrorCode Canceled(std::string long_err = "");

ErrorCode ResourceError(std::string long_err = "");

ErrorCode RuntimeError(std::string long_err = "");

}  // namespace spiderweb
