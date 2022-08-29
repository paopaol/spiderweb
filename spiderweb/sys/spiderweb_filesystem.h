#ifndef SPIDERWEB_FILESYSTEM_H
#define SPIDERWEB_FILESYSTEM_H

#include "ghc/filesystem.hpp"

namespace spiderweb {
namespace sys {
namespace fs = ::ghc::filesystem;

inline void make_clang_tidy_happy(const fs::filesystem_error& err) {
  (void)err;
}

}  // namespace sys
}  // namespace spiderweb

#endif
