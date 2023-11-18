#include "spiderweb/type/spiderweb_variant.h"

#include "absl/strings/numbers.h"

namespace spiderweb {
namespace detail {

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
TargetType ToIntOrFloat(const Variant::Var &v, bool *ok = nullptr) {
  bool       success = true;
  TargetType result = 0;

  try {
    result = absl::visit(detail::VisitorAs<TargetType>(), v);
  } catch (const std::exception &e) {
    success = false;
  }

  if (ok) {
    *ok = success;
  }

  return result;
}

}  // namespace detail

std::string Variant::ToString() const {
  return absl::visit(detail::VisitorAs<std::string>(), v_);
}

int64_t Variant::ToInt(bool *ok) const {
  return detail::ToIntOrFloat<int64_t>(v_, ok);
}

uint64_t Variant::ToUint(bool *ok) const {
  return detail::ToIntOrFloat<uint64_t>(v_, ok);
}

bool Variant::ToBool() const {
  return detail::ToIntOrFloat<uint64_t>(v_);
}

float Variant::ToFloat(bool *ok) const {
  return detail::ToIntOrFloat<float>(v_, ok);
}

}  // namespace spiderweb
