#ifndef INTERNAL_THREAD_CHECK_H
#define INTERNAL_THREAD_CHECK_H

#define CALL_THREAD_CHECK(name) \
  assert(std::this_thread::get_id() == ThreadId() && "can not call " #name " In other thread")

#endif
