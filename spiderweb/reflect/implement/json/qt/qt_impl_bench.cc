#include <QList>

#include "benchmark/benchmark.h"
#include "qt_json_reflection.hpp"

struct People {
  QString name;
  int age;
  float height;
  QList<int> intList;
  QList<QString> stringList;

  bool operator==(const People &other) const {
    return name == other.name && height == other.height && intList == other.intList &&
           stringList == other.stringList;
  }
};

struct PeopleList {
  QList<People> peoples;
};

REFLECT_JSON(People, (name, "name"), (age, "age"), (height, "height"), (intList, "intList"),
             (stringList, "stringList"))
REFLECT_JSON(PeopleList, (peoples, "peoples"))

static void BM_copy(benchmark::State &state) {
  QList<People> peoples;
  for (auto _ : state) {
    People people;
    peoples.push_back(people);
  }
}
BENCHMARK(BM_copy);

static void BM_FromJson(benchmark::State &state) {
  const char *json = R"(
{
  "peoples": [
    {
      "name": "name1",
      "age": 33,
      "height": 1.1,
      "intList": [1, 2, 3, 4],
      "stringList": ["a", "b", "c"]
    },
    {
      "name": "name2",
      "age": 44,
      "height": 2.2,
      "intList": [1, 2, 3, 4],
      "stringList": ["a", "b", "c"]
    },
    {
      "name": "name3",
      "age": 55,
      "height": 3.3,
      "intList": [1, 2, 3, 4],
      "stringList": ["a", "b", "c"]
    },
    {
      "name": "name4",
      "age": 66,
      "height": 4.4,
      "intList": [1, 2, 3, 4],
      "stringList": ["a", "b", "c"]
    }
  ]
}
)";
  reflect::json::QJsonReader serilizer(json);
  for (auto _ : state) {
    PeopleList people;

    serilizer.FromJson(&people);
  }
}
// Register the function as a benchmark
BENCHMARK(BM_FromJson);
