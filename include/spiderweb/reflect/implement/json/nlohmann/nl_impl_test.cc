#include "gtest/gtest.h"
#include "nlohmann_json_reflection.hpp"

struct People {
  std::string name;
  int age;
  float height;
  std::vector<int> intList;
  std::vector<std::string> stringList;

  bool operator==(const People &other) const {
    return name == other.name && height == other.height && intList == other.intList &&
           stringList == other.stringList;
  }
};
REFLECT_JSON(People, (name, "name"), (age, "age"), (height, "height"), (intList, "intList"),
             (stringList, "stringList"))

struct PeopleList {
  std::vector<People> peoples;
};
REFLECT_JSON(PeopleList, (peoples, "peoples"))

TEST(nlserilizer, FromJsonStruct) {
  const char *json = R"(
{
  "name": "xiaoming",
  "age": 33,
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  reflect::json::NlJsonReader serilizer(json);

  People people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, 33);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(nlserilizer, FromJsonStructMissingSome) {
  const char *json = R"(
{
  "name": "xiaoming",
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  reflect::json::NlJsonReader serilizer(json);

  People people;
  people.age = -1;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, -1);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(nlserilizer, FromJsonArray) {
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

  PeopleList people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.peoples.size(), 4);
  EXPECT_EQ(people.peoples[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[2], (People{"name3", 55, 3.3, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[3], (People{"name4", 66, 4.4, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(nlserilizer, FromJsonComplexArrayNoKey) {
  const char *json = R"(
[
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
  }
]
)";

  reflect::json::NlJsonReader serilizer(json);

  std::vector<People> people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.size(), 2);
  EXPECT_EQ(people[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(nlserilizer, FromJsonSimpleArrayNoKey) {
  const char *json = R"(
[123, 456]
)";

  reflect::json::NlJsonReader serilizer(json);

  std::vector<int> intList;
  serilizer.FromJson(&intList);

  EXPECT_EQ(intList, std::vector<int>({123, 456}));
}

struct SubStruct {
  int age;
};
REFLECT_JSON(SubStruct, (age, "age"))

struct Struct {
  SubStruct subStruct;
};
REFLECT_JSON(Struct, (subStruct, "subStruct"))

TEST(nlserilizer, FromJsonEmbedObject) {
  const char *json = R"(
{
  "subStruct": {
    "age": 333
  }
}
)";

  reflect::json::NlJsonReader serilizer(json);

  Struct s;
  serilizer.FromJson(&s);

  EXPECT_EQ(s.subStruct.age, 333);
}

TEST(nlserilizer, serilizer) {
  reflect::json::Meta<int, nlohmann::json> meta;

  {
    nlohmann::json json;
    int value = 33;
    meta.ToJson(&value, "value", json);

    EXPECT_EQ(json["value"].get<int>(), 33);
  }

  {
    nlohmann::json json;
    int value = 33;
    meta.ToJson(&value, json);

    EXPECT_EQ(json.get<int>(), 33);
  }
}

TEST(nlserilizer, serilizerArray) {
  {
    reflect::json::Meta<std::vector<int>, nlohmann::json> meta;
    nlohmann::json json;
    std::vector<int> value({1, 2, 3});
    meta.ToJson(&value, "value", json);

    EXPECT_EQ(json["value"].at(0), 1);
    EXPECT_EQ(json["value"].at(1), 2);
    EXPECT_EQ(json["value"].at(2), 3);
  }

  {
    reflect::json::Meta<std::vector<int>, nlohmann::json> meta;
    nlohmann::json json;
    std::vector<int> value({1, 2, 3});
    meta.ToJson(&value, json);

    EXPECT_EQ(json.at(0), 1);
    EXPECT_EQ(json.at(1), 2);
    EXPECT_EQ(json.at(2), 3);
  }
}

TEST(nlserilizer, serilizerSimpleStructMeta) {
  reflect::json::NlJsonReader serilizer;
  reflect::json::Meta<SubStruct, nlohmann::json> meta;
  SubStruct value;
  value.age = 12;
  nlohmann::json json;
  meta.ToJson(&value, json);
  std::cout << json << std::endl;
}

TEST(nlserilizer, serilizerSimpleStruct) {
  nlohmann::json json;
  SubStruct value;
  value.age = 12;

  reflect::json::Writer<nlohmann::json> serilizer(&json);
  serilizer.ToJson("kwy", &value);
  std::cout << json << std::endl;
}

TEST(nlserilizer, serilizerComplexStruct) {
  reflect::json::Meta<Struct, nlohmann::json> meta;
  Struct value;
  nlohmann::json json;
  meta.ToJson(&value, json);
  std::cout << json << std::endl;
}

TEST(nlserilizer, serilizerStruct) {
  People people;

  people.age = 123;
  people.height = 1.75;
  people.name = "your name";
  people.intList = std::vector<int>({1, 2, 3});
  people.stringList = std::vector<std::string>({"a", "b", "c"});

  reflect::json::NlJsonWriter serilizer;

  serilizer.ToJson(&people);
  std::cout << serilizer.root << std::endl;

  {
    reflect::json::NlJsonReader tmp(serilizer.root.dump().c_str());

    People peopleNew;

    tmp.FromJson(&peopleNew);

    EXPECT_EQ(peopleNew, people);
  }
}

TEST(nlserilizer, serilizerPeoples) {
  PeopleList list;

  for (int i = 0; i < 3; ++i) {
    People people;

    people.age = i;
    people.height = 1.75;
    people.name = "your name";
    people.intList = std::vector<int>({1, 2, 3});
    people.stringList = std::vector<std::string>({"a", "b", "c"});

    list.peoples.push_back(people);
  }

  reflect::json::NlJsonWriter serilizer;

  serilizer.ToJson(&list);

  std::cout << serilizer.ToString() << std::endl;
}

struct MyNode {
  int age;
  std::shared_ptr<MyNode> next;
};
REFLECT_JSON(MyNode, (age, "age"), (next, "next"))

TEST(nlserilizer, FromJsonRecursive) {
  static const char *str = R"(
{
  "age": 123,
  "next": {
    "age": 666,
    "next": {
      "age": 777
    }
  }
}
    )";

  reflect::json::NlJsonReader reader(str);

  MyNode root;
  reader.FromJson(&root);

  EXPECT_EQ(root.age, 123);
  EXPECT_TRUE(root.next);
  EXPECT_EQ(root.next->age, 666);
  EXPECT_TRUE(root.next->next);
  EXPECT_EQ(root.next->next->age, 777);
}

TEST(nlserilizer, ToJsonRecursive) {
  MyNode root;

  root.age = 1;

  root.next.reset(new MyNode());
  root.next->age = 2;

  root.next->next.reset(new MyNode());
  root.next->next->age = 3;

  reflect::json::NlJsonWriter writer;
  writer.ToJson(&root);
  std::cout << writer.ToString() << std::endl;
}

struct ListNode {
  int age;
  std::vector<std::shared_ptr<ListNode>> childs;
};
REFLECT_JSON(ListNode, (age, "age"), (childs, "childs"))

TEST(nlserilizer, ToJsonArrayRecursive) {
  ListNode root;

  root.age = 1;

  {
    std::unique_ptr<ListNode> child(new ListNode);
    child->age = 2;
    root.childs.push_back(std::move(child));
  }
  {
    std::unique_ptr<ListNode> child(new ListNode);
    child->age = 3;
    root.childs.push_back(std::move(child));
  }

  reflect::json::NlJsonWriter writer;
  writer.ToJson(&root);

  std::cout << writer.ToString() << std::endl;
}

TEST(nlserilizer, FromJsonArrayRecursive) {
  static const char *str = R"(
{
  "age": 1,
  "childs": [
    { "age": 2, "childs": null },
    { "age": 3, "childs": null }
  ]
}
    )";
  ListNode root{};

  reflect::json::NlJsonReader writer(str);
  writer.FromJson(&root);

  EXPECT_EQ(root.age, 1);
  EXPECT_EQ(root.childs.size(), 2);

  EXPECT_EQ(root.childs[0]->age, 2);
  EXPECT_EQ(root.childs[1]->age, 3);
}

enum class EnumValue {
  kSuccess,
  kFailed,
};
REFLECT_ENUM(EnumValue, std::string, (kSuccess, "success"), (kFailed, "failed"))

struct Object {
  EnumValue value{EnumValue::kFailed};
};
REFLECT_JSON(Object, (value, "enumValue", std::string))

TEST(nlserilizer, ToJsonEnum) {
  Object obj;
  obj.value = EnumValue::kSuccess;
  reflect::json::NlJsonWriter writer;
  writer.ToJson(&obj);

  std::cout << writer.ToString() << std::endl;
}

TEST(nlserilizer, FromJsonEnum) {
  static const char *xml = R"(
{
  "enumValue": "failed"
}
      )";

  Object obj;
  obj.value = EnumValue::kSuccess;

  reflect::json::NlJsonReader reader(xml);
  reader.FromJson(&obj);

  EXPECT_EQ(obj.value, EnumValue::kFailed);
}
