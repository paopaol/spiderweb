#pragma once

#define SPIDERWEB_VERIFY(exp, action) \
  do {                                \
    if (!(exp)) {                     \
      action;                         \
    }                                 \
  } while (0);
