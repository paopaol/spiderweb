#include "spiderweb_any_view.h"

#include "gtest/gtest.h"

TEST(AnyView, ToString) {
  absl::any var = 1.f;
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<const double>(2.f);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<uint64_t>(12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<int64_t>(-12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<uint32_t>(12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<int32_t>(-12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<uint16_t>(12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = static_cast<int16_t>(-12345);
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = std::string("123");
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  var = "const char *";
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";

  const std::string const_str = "const str";
  var = const_str;
  std::cout << spiderweb::type::AnyView::ToString(var) << "\n";
}

struct Struct {};

TEST(AnyView, ToType) {
  absl::any var = 123.5f;
  std::cout << spiderweb::type::AnyView::ToInt64(var) << "\n";

  var = true;
  std::cout << spiderweb::type::AnyView::ToInt64(var) << "\n";

  var = Struct{};
  std::cout << spiderweb::type::AnyView::ToInt64(var) << "\n";

  var = Struct{};
  std::cout << spiderweb::type::AnyView::ToBool(var) << "\n";
}
