#include "spiderweb/type/spiderweb_variant.h"

namespace spiderweb {

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

}  // namespace spiderweb
