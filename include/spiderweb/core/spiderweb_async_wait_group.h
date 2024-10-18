#pragma once

#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {

class AsyncWaitGroup : public spiderweb::Object {
 public:
  explicit AsyncWaitGroup(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {
  }

  void Add(uint32_t total) {
    count_ = total;
  }

  void Clear() {
    count_ = 0;
  }

  void Done() {
    count_--;
    assert(count_ >= 0);

    if (count_ == 0) {
      spider_emit AllDone();
    }
  }

  spiderweb::Notify<> AllDone;

 private:
  uint32_t count_ = 0;
};
}  // namespace spiderweb
