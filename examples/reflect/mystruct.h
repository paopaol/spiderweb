#pragma once

#include <string>
#include <vector>

enum class Category {
  kCooking,
  kChildren,
  kWeb,
  kUnkown,
};

struct Title {
  std::string lang;
  std::string title;
};

struct Book {
  Category category{Category::kUnkown};
  std::vector<std::string> authores;
  Title title;
  int year;
  float price;
};

struct BookStore {
  std::vector<Book> books;
};
