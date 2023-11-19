#include <QString>

#include "gtest/gtest.h"
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

TEST(qtserilizer, FromJsonStruct) {
  const char *json = R"(
{
  "name": "xiaoming",
  "age": 33,
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b", "c"]
}
)";

  reflect::json::QJsonReader serilizer(json);

  People people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, 33);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.stringList, QList<QString>({"a", "b", "c"}));
  EXPECT_EQ(people.intList, QList<int>({1, 2, 3, 4}));
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

  reflect::json::QJsonReader serilizer(json);

  People people;
  people.age = -1;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, -1);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, QList<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, QList<QString>({"a", "b", "c"}));
}

TEST(qtserilizer, FromJsonArray) {
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

  PeopleList people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.peoples.size(), 4);
  EXPECT_EQ(people.peoples[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[2], (People{"name3", 55, 3.3, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[3], (People{"name4", 66, 4.4, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(qtserilizer, FromJsonComplexArrayNoKey) {
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

  reflect::json::QJsonReader serilizer(json);

  QList<People> people;
  serilizer.FromJson(&people);
  EXPECT_EQ(people.size(), 2);
  EXPECT_EQ(people[0], (People{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people[1], (People{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(qtserilizer, FromJsonSimpleArrayNoKey) {
  const char *json = R"(
[123, 456]
)";

  reflect::json::QJsonReader serilizer(json);

  QList<int> intList;
  serilizer.FromJson(&intList);

  EXPECT_EQ(intList, QList<int>() << 123 << 456);
}

TEST(qtserilizer, FromJsonSimpleArrayNoKeyQtStringList) {
  const char *json = R"(
["123", "456"]
)";

  reflect::json::QJsonReader serilizer(json);

  QStringList stringList;
  serilizer.FromJson(&stringList);

  EXPECT_EQ(stringList, QStringList() << "123"
                                      << "456");
}

TEST(qtserilizer, serilizer) {
  reflect::json::Meta<int, QJsonValue> meta;

  {
    QJsonValue json;
    int value = 33;
    meta.ToJson(&value, "value", json);

    EXPECT_EQ(json["value"].toInt(), 33);
  }

  {
    QJsonValue json;
    int value = 33;
    meta.ToJson(&value, json);

    EXPECT_EQ(json.toInt(), 33);
  }
}

TEST(qtserilizer, serilizerArray) {
  {
    reflect::json::Meta<QList<int>, QJsonValue> meta;
    QJsonValue json;
    QList<int> value({1, 2, 3});
    meta.ToJson(&value, "value", json);

    EXPECT_EQ(json["value"].toArray().at(0), 1);
    EXPECT_EQ(json["value"].toArray().at(1), 2);
    EXPECT_EQ(json["value"].toArray().at(2), 3);
  }

  {
    reflect::json::Meta<QList<int>, QJsonValue> meta;
    QJsonValue json;
    QList<int> value({1, 2, 3});
    meta.ToJson(&value, json);

    EXPECT_EQ(json.toArray().at(0), 1);
    EXPECT_EQ(json.toArray().at(1), 2);
    EXPECT_EQ(json.toArray().at(2), 3);
  }
}

struct SubStruct {
  int age;
};
REFLECT_JSON(SubStruct, (age, "age"))

struct Struct {
  SubStruct subStruct;
};
REFLECT_JSON(Struct, (subStruct, "subStruct"))

TEST(qtserilizer, serilizerSimpleStructMeta) {
  reflect::json::Meta<SubStruct, QJsonValue> meta;
  SubStruct value;
  value.age = 12;
  QJsonValue json;
  meta.ToJson(&value, json);
  qDebug() << json;
}

TEST(qtserilizer, serilizerSimpleStruct) {
  QJsonValue json;
  SubStruct value;
  value.age = 12;

  reflect::json::Writer<QJsonValue> serilizer(&json);
  serilizer.ToJson("kwy", &value);
  qDebug() << json;
}

TEST(qtserilizer, serilizerComplexStruct) {
  reflect::json::Meta<Struct, QJsonValue> meta;
  Struct value;
  QJsonValue json;
  meta.ToJson(&value, json);
  qDebug() << json;
}

TEST(qtserilizer, serilizerStruct) {
  People people;

  people.age = 123;
  people.height = 1.75;
  people.name = "your name";
  people.intList = QList<int>({1, 2, 3});
  people.stringList = QList<QString>({"a", "b", "c"});

  reflect::json::QJsonWriter serilizer;

  serilizer.ToJson(&people);
  qDebug() << serilizer.root;

  {
    std::string s = serilizer.ToString().toStdString();
    reflect::json::QJsonReader tmp(s.c_str());

    People peopleNew;

    tmp.FromJson(&peopleNew);

    EXPECT_EQ(peopleNew, people);
  }
}

TEST(qtserilizer, serilizerPeoples) {
  PeopleList list;

  for (int i = 0; i < 3; ++i) {
    People people;

    people.age = i;
    people.height = 1.75;
    people.name = "your name";
    people.intList = QList<int>({1, 2, 3});
    people.stringList = QList<QString>({"a", "b", "c"});

    list.peoples.push_back(people);
  }

  reflect::json::QJsonWriter serilizer;

  serilizer.ToJson(&list);

  qDebug() << serilizer.ToString();
}

struct MyNode {
  int age;
  std::shared_ptr<MyNode> next;
};
REFLECT_JSON(MyNode, (age, "age"), (next, "next"))

TEST(qtserilizer, FromJsonRecursive) {
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

  reflect::json::QJsonReader reader(str);

  MyNode root;
  reader.FromJson(&root);

  EXPECT_EQ(root.age, 123);
  EXPECT_TRUE(root.next);
  EXPECT_EQ(root.next->age, 666);
  EXPECT_TRUE(root.next->next);
  EXPECT_EQ(root.next->next->age, 777);
}

TEST(qtserilizer, ToJsonRecursive) {
  MyNode root;

  root.age = 1;

  root.next.reset(new MyNode());
  root.next->age = 2;

  root.next->next.reset(new MyNode());
  root.next->next->age = 3;

  reflect::json::QJsonWriter writer;
  writer.ToJson(&root);
  qDebug() << qPrintable(writer.ToString());
}

struct ListNode {
  int age;
  std::vector<std::shared_ptr<ListNode>> childs;
};
REFLECT_JSON(ListNode, (age, "age"), (childs, "childs"))

TEST(qtserilizer, ToJsonArrayRecursive) {
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

  reflect::json::QJsonWriter writer;
  writer.ToJson(&root);

  qDebug() << qPrintable(writer.ToString());
}

TEST(qtserilizer, FromJsonArrayRecursive) {
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

  reflect::json::QJsonReader writer(str);
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

TEST(qtserilizer, ToJsonEnum) {
  Object obj;
  obj.value = EnumValue::kSuccess;
  reflect::json::QJsonWriter writer;
  writer.ToJson(&obj);

  qDebug() << writer.ToString();
}

TEST(qtserilizer, FromJsonEnum) {
  static const char *xml = R"(
{
  "enumValue": "failed"
}
      )";

  Object obj;
  obj.value = EnumValue::kSuccess;

  reflect::json::QJsonReader reader(xml);
  reader.FromJson(&obj);

  EXPECT_EQ(obj.value, EnumValue::kFailed);
}
