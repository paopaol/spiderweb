#ifndef SPIDERWEB_TYPE_ANY_VIEW_H
#define SPIDERWEB_TYPE_ANY_VIEW_H

#include "absl/types/any.h"
#include <string>

namespace spiderweb {
namespace type {
class AnyView {
 public:
  template <typename T>
  inline static bool StrictCanConvert(const absl::any &var) {
    return var.type() == typeid(T);
  }

  inline static bool IsString(const absl::any &var) {
    return StrictCanConvert<std::string>(var) || StrictCanConvert<char *>(var);
  }

  inline static bool IsFloat(const absl::any &var) {
    return StrictCanConvert<float>(var);
  }

  inline static bool IsDouble(const absl::any &var) {
    return StrictCanConvert<double>(var);
  }

  inline static bool IsBool(const absl::any &var) {
    return StrictCanConvert<bool>(var);
  }

  inline static bool IsUint64(const absl::any &var) {
    return StrictCanConvert<uint64_t>(var);
  }

  inline static bool IsInt64(const absl::any &var) {
    return StrictCanConvert<int64_t>(var);
  }

  inline static bool IsUint32(const absl::any &var) {
    return StrictCanConvert<uint32_t>(var);
  }

  inline static bool IsInt32(const absl::any &var) {
    return StrictCanConvert<int32_t>(var);
  }

  inline static bool IsUint16(const absl::any &var) {
    return StrictCanConvert<uint16_t>(var);
  }

  inline static bool IsInt16(const absl::any &var) {
    return StrictCanConvert<int16_t>(var);
  }

  inline static bool IsInt8(const absl::any &var) {
    return StrictCanConvert<int8_t>(var);
  }

  inline static bool IsUint8(const absl::any &var) {
    return StrictCanConvert<uint8_t>(var);
  }

  inline static float ToFloat(const absl::any &var) {
    return ToPodType<float>(var);
  }

  inline static double ToDouble(const absl::any &var) {
    return ToPodType<double>(var);
  }

  inline static bool ToBool(const absl::any &var) {
    return ToPodType<bool>(var);
  }

  inline static uint64_t ToUint64(const absl::any &var) {
    return ToPodType<uint64_t>(var);
  }

  inline static int64_t ToInt64(const absl::any &var) {
    return ToPodType<int64_t>(var);
  }

  template <typename T, typename = typename std::enable_if<!std::is_class<T>::value>::type>
  inline static T ToType(const absl::any &var) {
    return ToPodType<T>(var);
  }

  template <typename T>
  inline static T ToType(const absl::any &var) {
    return absl::any_cast<T>(var);
  }

  inline static std::string ToString(const absl::any &var) {
    if (!var.has_value()) {
      return "";
    }

    if (StrictCanConvert<std::string>(var)) {
      return absl::any_cast<std::string>(var);
    }

    if (StrictCanConvert<const char *>(var)) {
      return absl::any_cast<const char *>(var);
    }

    if (IsFloat(var)) {
      return std::to_string(absl::any_cast<float>(var));
    } else if (IsDouble(var)) {
      return std::to_string(absl::any_cast<double>(var));
    } else if (IsUint64(var)) {
      return std::to_string(absl::any_cast<uint64_t>(var));
    } else if (IsInt64(var)) {
      return std::to_string(absl::any_cast<int64_t>(var));
    } else if (IsUint32(var)) {
      return std::to_string(absl::any_cast<uint32_t>(var));
    } else if (IsInt32(var)) {
      return std::to_string(absl::any_cast<int32_t>(var));
    } else if (IsUint16(var)) {
      return std::to_string(absl::any_cast<uint16_t>(var));
    } else if (IsInt16(var)) {
      return std::to_string(absl::any_cast<int16_t>(var));
    } else if (IsUint8(var)) {
      return std::to_string(absl::any_cast<uint8_t>(var));
    } else if (IsInt8(var)) {
      return std::to_string(absl::any_cast<int8_t>(var));
    } else if (IsBool(var)) {
      return std::to_string(absl::any_cast<bool>(var));
    } else {
      return "";
    }
  }

 private:
  template <typename T>
  static T ToPodType(const absl::any &var) {
    if (!var.has_value()) {
      return T{};
    }

    if (IsFloat(var)) {
      return static_cast<T>(absl::any_cast<float>(var));
    } else if (IsDouble(var)) {
      return static_cast<T>(absl::any_cast<double>(var));
    } else if (IsUint64(var)) {
      return static_cast<T>(absl::any_cast<uint64_t>(var));
    } else if (IsInt64(var)) {
      return static_cast<T>(absl::any_cast<int64_t>(var));
    } else if (IsUint32(var)) {
      return static_cast<T>(absl::any_cast<uint32_t>(var));
    } else if (IsInt32(var)) {
      return static_cast<T>(absl::any_cast<int32_t>(var));
    } else if (IsUint16(var)) {
      return static_cast<T>(absl::any_cast<uint16_t>(var));
    } else if (IsInt16(var)) {
      return static_cast<T>(absl::any_cast<int16_t>(var));
    } else if (IsUint8(var)) {
      return static_cast<T>(absl::any_cast<uint8_t>(var));
    } else if (IsInt8(var)) {
      return static_cast<T>(static_cast<T>(absl::any_cast<int8_t>(var)));
    } else if (IsBool(var)) {
      return static_cast<T>(absl::any_cast<bool>(var));
    } else {
      return T{};
    }
  }
};
}  // namespace type
}  // namespace spiderweb

#endif
