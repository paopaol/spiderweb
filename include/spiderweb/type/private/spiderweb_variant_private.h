#include <string>

#include "absl/strings/numbers.h"
#include "absl/types/any.h"
#include "absl/types/variant.h"

namespace spiderweb {
namespace detail {

struct Monostate {};

using Var = absl::variant<Monostate, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
                          std::string, float, double, bool, absl::any>;

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                         std::is_floating_point<T>::value ||
                                                         std::is_enum<T>::value>::type>
std::string to_string(T value) {
  return std::to_string(value);
}

template <typename Result, typename F = void>
struct VisitorAs {};

template <>
struct VisitorAs<std::string> {
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value ||
                                                           std::is_enum<T>::value>::type>
  std::string operator()(T value) const {
    return to_string(value);
  }

  std::string operator()(const std::string &value) const {
    return value;
  }

  std::string operator()(const absl::any & /*value*/) const {
    return "";
  }
};

template <typename IntType>
struct VisitorAs<IntType> {
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value ||
                                                           std::is_enum<T>::value>::type>
  IntType operator()(T value) const {
    return value;
  }

  IntType operator()(const std::string &value) const {
    int64_t result;
    bool    ok = absl::SimpleAtoi(value, &result);
    if (!ok) {
      throw std::runtime_error("");
    }
    return result;
  }

  IntType operator()(const absl::any & /*value*/) const {
    return 0;
  }
};

template <>
struct VisitorAs<float> {
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value ||
                                                           std::is_enum<T>::value>::type>
  float operator()(T value) const {
    return value;
  }

  float operator()(const std::string &value) const {
    float result;

    bool ok = absl::SimpleAtof(value, &result);
    if (!ok) {
      throw std::runtime_error("");
    }
    return result;
  }

  float operator()(const absl::any & /*value*/) const {
    return 0.f;
  }
};

template <>
struct VisitorAs<double> {
  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value ||
                                                           std::is_floating_point<T>::value ||
                                                           std::is_enum<T>::value>::type>
  double operator()(T value) const {
    return value;
  }

  double operator()(const std::string &value) const {
    double result;

    bool ok = absl::SimpleAtod(value, &result);
    if (!ok) {
      throw std::runtime_error("");
    }
    return result;
  }

  double operator()(const absl::any & /*value*/) const {
    return 0.f;
  }
};

template <typename TargetType>
TargetType ToIntOrFloat(const Var &v) {
  if (absl::holds_alternative<int16_t>(v)) {
    return absl::get<int16_t>(v);
  } else if (absl::holds_alternative<uint16_t>(v)) {
    return absl::get<uint16_t>(v);
  } else if (absl::holds_alternative<int32_t>(v)) {
    return static_cast<TargetType>(absl::get<int32_t>(v));
  } else if (absl::holds_alternative<uint32_t>(v)) {
    return static_cast<TargetType>(absl::get<uint32_t>(v));
  } else if (absl::holds_alternative<int64_t>(v)) {
    return static_cast<TargetType>(absl::get<int64_t>(v));
  } else if (absl::holds_alternative<uint64_t>(v)) {
    return static_cast<TargetType>(absl::get<uint64_t>(v));
  } else if (absl::holds_alternative<double>(v)) {
    return static_cast<TargetType>(absl::get<double>(v));
  } else if (absl::holds_alternative<bool>(v)) {
    return absl::get<bool>(v);
  } else if (absl::holds_alternative<float>(v)) {
    return static_cast<TargetType>(absl::get<float>(v));
  } else if (absl::holds_alternative<std::string>(v)) {
    return static_cast<TargetType>(std::stof(absl::get<std::string>(v)));
  } else if (absl::holds_alternative<Monostate>(v) || absl::holds_alternative<absl::any>(v)) {
    return 0;
  }
  return 0;
}

}  // namespace detail
}  // namespace spiderweb
