#ifndef SPIDERWEB_REFLECT_ENUM_H
#define SPIDERWEB_REFLECT_ENUM_H

#include <type_traits>

#include "macro_map.h"

namespace spiderweb {
namespace reflect {

#define REFLECT_REMOVE_PARENTHESES(A) REFLECT_REMOVE_PARENTHESES_IMPL A
#define REFLECT_REMOVE_PARENTHESES_IMPL(...) __VA_ARGS__
#define REFLECT_NUM_OF_ARGS(...) VISIT_STRUCT_PP_NARG(__VA_ARGS__)

#define reflect_enum_make_declare_alias_enum_pair(enum_value, alias_value) \
  {alias_value, Type::enum_value},
#define reflect_enum_expand_alias_enum_pair(pair) reflect_enum_make_declare_alias_enum_pair pair

#define reflect_enum_make_declare_enum_alias_pair(enum_value, alias_value) \
  {Type::enum_value, alias_value},

#define reflect_enum_expand_enum_alias_pair(pair) reflect_enum_make_declare_enum_alias_pair pair

template <typename T, typename EnumType,
          typename = typename std ::enable_if<std ::is_enum<EnumType>::value>::type>
T ToAliasType(EnumType value);

#define REFLECT_ENUM(EnumType, AliasType, ...)                                       \
  static_assert(!std::is_pointer<AliasType>::value, "can not be pointer");           \
  namespace spiderweb {                                                              \
  namespace reflect {                                                                \
  template <typename = typename std::enable_if<std::is_enum<EnumType>::value>::type> \
  inline EnumType FromAliasType(const AliasType &value, EnumType defaultValue) {     \
    using Type = EnumType;                                                           \
    static const std::map<AliasType, EnumType> _map{                                 \
        MACRO_MAP(reflect_enum_expand_alias_enum_pair, __VA_ARGS__)};                \
    const auto it = _map.find(value);                                                \
    return it == _map.end() ? defaultValue : it->second;                             \
  }                                                                                  \
  template <>                                                                        \
  inline AliasType ToAliasType(EnumType value) {                                     \
    using Type = EnumType;                                                           \
    static const std::map<EnumType, AliasType> _map{                                 \
        MACRO_MAP(reflect_enum_expand_enum_alias_pair, __VA_ARGS__)};                \
    return _map.at(value);                                                           \
  }                                                                                  \
  }                                                                                  \
  }
}  // namespace reflect
}  // namespace spiderweb
#endif
