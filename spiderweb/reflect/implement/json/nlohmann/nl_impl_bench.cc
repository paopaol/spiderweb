#include "benchmark/benchmark.h"
#include "nlohmann_json_reflection.hpp"

struct People {
  std::string name;
  int age;
  float height;
  std::vector<int> intList;
  std::vector<std::string> stringList;
};

struct PeopleList {
  std::vector<People> peoples;
};

REFLECT_JSON(People, (name, "name"), (age, "age"), (height, "height"), (intList, "intList"),
             (stringList, "stringList"))
REFLECT_JSON(PeopleList, (peoples, "peoples"))

static void BM_copy(benchmark::State &state) {
  std::vector<People> peoples;
  for (auto _ : state) {
    peoples.emplace_back(People());
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
  reflect::json::NlJsonReader serilizer(json);
  for (auto _ : state) {
    PeopleList people;
    serilizer.FromJson(&people);
  }
}
BENCHMARK(BM_FromJson);

static void BM_FromJsonMenal(benchmark::State &state) {
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

  nlohmann::json root = nlohmann::json::parse(json);
  for (auto _ : state) {
    PeopleList people;

    const auto &list = root["peoples"];
    const int size = list.size();
    for (int i = 0; i < list.size(); ++i) {
      const auto &l = list[i];
      People p;
      p.age = l["age"];
      p.name = l["name"];
      p.height = l["height"];
      l["intList"].get_to(p.intList);
      l["stringList"].get_to(p.stringList);

      people.peoples.push_back(p);
    }
  }
}
BENCHMARK(BM_FromJsonMenal);

static void BM_ToJsonReflect(benchmark::State &state) {
  PeopleList people;
  for (int i = 0; i < 3; ++i) {
    People p;

    p.age = 123;
    p.name = "xiaoming";
    p.height = 12.4;
    p.intList = std::vector<int>({1, 2, 3});
    p.stringList = std::vector<std::string>({"a", "b", "c"});
    people.peoples.push_back(p);
  }

  for (auto _ : state) {
    reflect::json::NlJsonWriter writer;
    writer.ToJson(&people);
  }
}
BENCHMARK(BM_ToJsonReflect);

static void BM_ToJsonRow(benchmark::State &state) {
  for (auto _ : state) {
    nlohmann::json root;
    for (int i = 0; i < 3; ++i) {
      nlohmann::json p;
      p["age"] = 123;
      p["name"] = "xiaoming";
      p["height"] = 12.4;
      p["intList"] = std::vector<int>({1, 2, 3});
      p["stringList"] = std::vector<std::string>({"a", "b", "c"});
      root["peoples"].emplace_back(std::move(p));
    }
  }
}
BENCHMARK(BM_ToJsonRow);
