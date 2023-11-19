#include "spiderweb/core/spiderweb_object.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"

class Sender : public spiderweb::Object {
 public:
  explicit Sender(spiderweb::Object *parent = nullptr) : spiderweb::Object(parent) {
  }

  ~Sender() override {
    std::cout << "Sender deleted" << '\n';
  }

  spiderweb::Notify<> a_event;
};

TEST(spiderweb_object, DeleteLater) {
  spiderweb::EventLoop loop;
  auto                *sender = new Sender;

  spiderweb::Object::Connect(sender, &Sender::a_event, &loop, [&]() {
    // should crash here under windows
    sender->DeleteLater();
    std::cout << "crash here ? " << '\n';

    loop.QueueTask([&]() { loop.Quit(); });
  });

  sender->a_event();

  loop.Exec();
}
