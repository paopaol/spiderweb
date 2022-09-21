#include "gtest/gtest.h"
#include "yyjson_json_reflection.hpp"

struct Student {
  bool bool_value;
  uint64_t uint64_value;
  int64_t int64_value;
  int int_value;
  float float_value;
  double double_value;
  std::string str_value;
  std::vector<int> intList;
};
REFLECT_JSON(Student, (bool_value, "bool_value"), (uint64_value, "uint64_value"),
             (int64_value, "int64_value"), (int_value, "int_value"), (float_value, "float_value"),
             (double_value, "double_value"), (str_value, "str_value"), (intList, "intList"))

TEST(yyserilizer, FromJsonBool) {
  static const char *str = R"(
{
  "bool_value": true,
  "uint64_value":1234,
  "int64_value":-100,
  "int_value": -4,
  "float_value":-2.34,
  "double_value":3.33,
  "str_value":"yyjson",
  "intList":[1,2,3]
}
  )";

  Student st;

  reflect::json::YyJsonReader reader(str);

  reader.FromJson(&st);

  EXPECT_EQ(st.bool_value, true);
  EXPECT_EQ(st.uint64_value, 1234);
  EXPECT_EQ(st.int64_value, -100);
  EXPECT_EQ(st.int_value, -4);
  EXPECT_FLOAT_EQ(st.float_value, -2.34);
  EXPECT_FLOAT_EQ(st.double_value, 3.33);
  EXPECT_EQ(st.str_value, "yyjson");
  EXPECT_EQ(st.intList, std::vector<int>({1, 2, 3}));
}

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

TEST(yyserilizer, FromJsonStruct) {
  const char *json = R"(
{
  "name": "xiaoming",
  "age": 33,
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  reflect::json::YyJsonReader serilizer(json);

  People people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, 33);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(yyserilizer, FromJsonStructMissingSome) {
  const char *json = R"(
{
  "name": "xiaoming",
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  reflect::json::YyJsonReader serilizer(json);

  People people;
  people.age = -1;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, -1);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(yyserilizer, FromJsonArray) {
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

  reflect::json::YyJsonReader serilizer(json);

  PeopleList people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.peoples.size(), 4);
  EXPECT_EQ(people.peoples[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[2], (People{"name3", 55, 3.3, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[3], (People{"name4", 66, 4.4, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(yyserilizer, FromJsonComplexArrayNoKey) {
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

  reflect::json::YyJsonReader serilizer(json);

  std::vector<People> people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.size(), 2);
  EXPECT_EQ(people[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(yyserilizer, FromJsonSimpleArrayNoKey) {
  const char *json = R"(
[123, 456]
)";

  reflect::json::YyJsonReader serilizer(json);

  std::vector<int> intList;
  serilizer.FromJson(&intList);

  EXPECT_EQ(intList, std::vector<int>({123, 456}));
}

TEST(yyserilizer, serilizerComplexStruct) {
  Student st{};
  st.bool_value = true;
  st.uint64_value = 123;
  st.int64_value = -123;
  st.int_value = 33;
  st.float_value = -5.5;
  st.double_value = 99.999;
  st.str_value = "yyjson";
  st.intList = std::vector<int>({1, 2, 3});

  reflect::json::YyJsonWriter writer;

  writer.ToJson(&st);
  std::cout << writer.ToString() << std::endl;
}

TEST(yyserilizer, serilizerPeoples) {
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

  reflect::json::YyJsonWriter writer;

  writer.ToJson(&list);

  std::cout << writer.ToString() << std::endl;
}

struct MyNode {
  int age;
  std::shared_ptr<MyNode> next;
};
REFLECT_JSON(MyNode, (age, "age"), (next, "next"))

TEST(yyserilizer, FromJsonRecursive) {
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

  reflect::json::YyJsonReader reader(str);

  MyNode root;
  reader.FromJson(&root);

  EXPECT_EQ(root.age, 123);
  EXPECT_TRUE(root.next);
  EXPECT_EQ(root.next->age, 666);
  EXPECT_TRUE(root.next->next);
  EXPECT_EQ(root.next->next->age, 777);
}

TEST(yyserilizer, ToJsonRecursive) {
  reflect::json::YyJsonWriter writer;

  MyNode root;

  root.age = 1;

  root.next.reset(new MyNode());
  root.next->age = 2;

  root.next->next.reset(new MyNode());
  root.next->next->age = 3;

  writer.ToJson(&root);

  std::cout << writer.ToString() << std::endl;
}

struct ListNode {
  int age;
  std::vector<std::shared_ptr<ListNode>> childs;
};
REFLECT_JSON(ListNode, (age, "age"), (childs, "childs"))

TEST(yyserilizer, ToJsonArrayRecursive) {
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

  reflect::json::YyJsonWriter writer;
  writer.ToJson(&root);

  std::cout << writer.ToString() << std::endl;
}

TEST(yyserilizer, FromJsonArrayRecursive) {
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

  reflect::json::YyJsonReader writer(str);
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

TEST(yyserilizer, ToJsonEnum) {
  Object obj;
  obj.value = EnumValue::kSuccess;
  reflect::json::YyJsonWriter writer;
  writer.ToJson(&obj);

  std::cout << writer.ToString() << std::endl;
}

TEST(yyserilizer, FromJsonEnum) {
  static const char *xml = R"(
{
  "enumValue": "failed"
}
      )";

  Object obj;
  obj.value = EnumValue::kSuccess;

  reflect::json::YyJsonReader reader(xml);
  reader.FromJson(&obj);

  EXPECT_EQ(obj.value, EnumValue::kFailed);
}
