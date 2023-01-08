#include "spiderweb/core/spiderweb_error_code.h"

namespace spiderweb {
std::string ErrorCode::FormatedMessage() const {
  std::string s;
  s.reserve(128);

  s.append(message());
  s.append(": ");
  s.append(long_error_);
  return s;
}

class ErrorCodeCategory : public std::error_category {
 public:
  const char *name() const noexcept override {
    return "spiderweb::ErrorCode";
  }

  std::string message(int ev) const override {
    switch (static_cast<ErrC>(ev)) {
      case ErrC::kOk:
        return "Ok";
      case ErrC::kInvalidArgument:
        return "InvalidArgument";
      case ErrC::kTimeout:
        return "Timeout";
      case ErrC::kCanceled:
        return "Canceled";
      case ErrC::kResourceError:
        return "ResourceError";
      case ErrC::kRuntimeError:
        return "RuntimeError";
      default:
        return "(unrecognized error)";
    }
  }
};

static const ErrorCodeCategory error_cate{};

ErrorCode MakeErrorCode(ErrC e) {
  return {static_cast<int>(e), error_cate};
}

ErrorCode MakeErrorCode(ErrC e, std::string long_err) {
  return {static_cast<int>(e), error_cate, long_err};
}

ErrorCode MakeErrorCode(std::error_code e, const std::string &long_err) {
  ErrorCode ec;
  ec.SetErrorCode(e);
  ec.SetLongMessage(long_err);
  return ec;
}

ErrorCode Ok(std::string long_err) {
  return MakeErrorCode(ErrC::kOk, std::move(long_err));
}

ErrorCode InvalidArgument(std::string long_err) {
  return MakeErrorCode(ErrC::kInvalidArgument, std::move(long_err));
}

ErrorCode Timeout(std::string long_err) {
  return MakeErrorCode(ErrC::kTimeout, std::move(long_err));
}

ErrorCode Canceled(std::string long_err) {
  return MakeErrorCode(ErrC::kCanceled, std::move(long_err));
}

ErrorCode ResourceError(std::string long_err) {
  return MakeErrorCode(ErrC::kResourceError, std::move(long_err));
}

ErrorCode RuntimeError(std::string long_err) {
  return MakeErrorCode(ErrC::kRuntimeError, std::move(long_err));
}

}  // namespace spiderweb
