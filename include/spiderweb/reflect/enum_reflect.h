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

#define REFLECT_NUM_OF_ARGS(...) VISIT_STRUCT_PP_NARG(__VA_ARGS__)

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
