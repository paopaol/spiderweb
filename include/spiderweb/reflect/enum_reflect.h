#ifndef SPIDERWEB_REFLECT_ENUM_H
#define SPIDERWEB_REFLECT_ENUM_H

#include <type_traits>
#include <unordered_map>

#include "macro_map.h"

namespace spiderweb {
namespace reflect {

struct PlaceHolderValue {};

#define REFLECT_REMOVE_PARENTHESES_IMPL(...) __VA_ARGS__
#define REFLECT_REMOVE_PARENTHESES(A) REFLECT_REMOVE_PARENTHESES_IMPL A

#define REFLECT_NUM_OF_ARGS_IMPL_(                                                                 \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
    _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, \
    _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, N, ...)                       \
  N

#define REFLECT_NUM_OF_ARGS(...)                                                                 \
  REFLECT_NUM_OF_ARGS_IMPL_(0, __VA_ARGS__, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59,  \
                            58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,  \
                            41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25,  \
                            24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, \
                            6, 5, 4, 3, 2, 1, 0)

#define reflect_enum_make_declare_alias_enum_pair(enum_value, alias_value) \
  {alias_value, Type::enum_value},
#define reflect_enum_expand_alias_enum_pair(pair) reflect_enum_make_declare_alias_enum_pair pair

#define reflect_enum_make_declare_enum_alias_pair(enum_value, alias_value) \
  {Type::enum_value, alias_value},

#define reflect_enum_expand_enum_alias_pair(pair) reflect_enum_make_declare_enum_alias_pair pair

#define REFLECT_ENUM(EnumType, AliasType, ...)                                       \
  static_assert(!std::is_pointer<AliasType>::value, "can not be pointer");           \
  namespace spiderweb {                                                              \
  namespace reflect {                                                                \
  template <typename = typename std::enable_if<std::is_enum<EnumType>::value>::type> \
  inline EnumType MapFrom(const AliasType &value, EnumType defaultValue) {           \
    using Type = EnumType;                                                           \
    static const std::unordered_map<AliasType, EnumType> _map{                       \
        MACRO_MAP(reflect_enum_expand_alias_enum_pair, __VA_ARGS__)};                \
    const auto it = _map.find(value);                                                \
    return it == _map.end() ? defaultValue : it->second;                             \
  }                                                                                  \
  inline void MapTo(EnumType value, AliasType &to) {                                 \
    using Type = EnumType;                                                           \
    static const std::unordered_map<EnumType, AliasType> _map{                       \
        MACRO_MAP(reflect_enum_expand_enum_alias_pair, __VA_ARGS__)};                \
    to = _map.at(value);                                                             \
  }                                                                                  \
  }                                                                                  \
  }
}  // namespace reflect
}  // namespace spiderweb
#endif
