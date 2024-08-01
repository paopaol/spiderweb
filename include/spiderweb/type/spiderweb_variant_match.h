#pragma once

#include "absl/types/variant.h"

namespace spiderweb {

template <typename Var, typename T>
void Match(const Var &var, const std::function<void(T)> &f) {
  if (absl::holds_alternative<T>(var)) {
    return f(absl::get<T>(var));
  }
}

template <typename T, typename... Args>
void Match(const absl::variant<T, Args...> &var, const std::function<void(T)> &f1,
           std::function<void(Args)> &&...f) {
  if (absl::holds_alternative<T>(var)) {
    return f1(absl::get<T>(var));
  }

  return Match(var, std::forward<std::function<void(Args)>>(f)...);
}
}  // namespace spiderweb
