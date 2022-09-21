#include <QCoreApplication>
#include <QTimer>

#include "EventLoopEPoll/eventdispatcher_epoll.h"
#include "benchmark/benchmark.h"

class MyObject : public QObject {
  Q_OBJECT

 public:
  MyObject(QObject *parent = nullptr) : QObject(parent) {
    qRegisterMetaType<std::string>("std::string");
  };

 signals:
  void signal(const std::string &a, int b, float c);
};

// static void BM_QtConnect(benchmark::State &state) {
//   const auto f = []() {};
//
//   int              argc = 1;
//   char            *argv[] = {(char *)"test"};
//   QCoreApplication app(argc, argv);
//   QTimer           timer;
//   for (auto _ : state) {
//     QObject::connect(&timer, &QTimer::timeout, &app, f);
//   }
// }
//
// BENCHMARK(BM_QtConnect);
//
// static void BM_TimerAdd(benchmark::State &state) {
//   int              argc = 1;
//   char            *argv[] = {(char *)"test"};
//   QCoreApplication app(argc, argv);
//
//   static int value = 0;
//   int        count = 0;
//   const auto f = [&]() {
//     ++value;
//     if (count == value) {
//       app.quit();
//     }
//   };
//
//   for (auto _ : state) {
//     auto *timer = new QTimer();
//
//     timer->setTimerType(Qt::TimerType::PreciseTimer);
//     timer->setSingleShot(true);
//     timer->setInterval(1);
//     QObject::connect(timer, &QTimer::timeout, &app, f);
//     ++count;
//     timer->start();
//   }
//   // printf("value %d count %d\n", value, count);
//
//   app.exec();
//   // printf("value %d count %d\n", value, count);
// }
//
// BENCHMARK(BM_TimerAdd);

static void BM_QtSignalCall(benchmark::State &state) {
  int              argc = 1;
  char            *argv[] = {(char *)"test"};
  QCoreApplication app(argc, argv);

  MyObject obj;

  uint64_t          called = 0;
  static const auto f = [&](const std::string &a, int b, float c) { called++; };
  QObject::connect(&obj, &MyObject::signal, &app, f);

  for (auto _ : state) {
    obj.signal("123", 2, float(3.33));
  }
}
BENCHMARK(BM_QtSignalCall);

#include "connect.moc"
