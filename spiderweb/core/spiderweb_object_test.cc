#include "spiderweb_object.h"

#include "core/spiderweb_eventloop.h"
#include "core/spiderweb_timer.h"
#include "gtest/gtest.h"

class Sender : public spiderweb::Object {
 public:
  Sender(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {}

  ~Sender() { std::cout << "Sender deleted" << std::endl; }

  spiderweb::EventSignal<void()> a_event;
};

TEST(spiderweb_object, DeleteLater) {
  spiderweb::EventLoop loop;
  auto                *sender = new Sender;

  spiderweb::Object::Connect(sender, &Sender::a_event, &loop, [&]() {
    // should crash here under windows
    sender->DeleteLater();
    std::cout << "crash here ? " << std::endl;

    loop.QueueTask([&]() { loop.Quit(); });
  });

  sender->a_event();

  loop.Exec();
}
