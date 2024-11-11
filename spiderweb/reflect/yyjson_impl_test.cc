#include "gtest/gtest.h"
#include "spiderweb/reflect/json_node.h"
#include "spiderweb/reflect/json_reflect.h"

struct Student {
  bool                                 bool_value;
  uint64_t                             uint64_value;
  int64_t                              int64_value;
  int                                  int_value;
  float                                float_value;
  double                               double_value;
  std::string                          str_value;
  std::vector<int>                     intList;
  spiderweb::reflect::PlaceHolderValue placeholder;
};

// clang-format off
REFLECT_JSON(Student, 
        (bool_value), 
        (uint64_value, "uint64_value"),
        (int64_value, "int64_value"),
        (int_value, "int_value"),
        (float_value, "float_value"),
        (double_value, "double_value"),
        (str_value, "str_value"), 
        (intList, "intList")
)
// clang-format off

TEST(ReflectJson, FromJsonBool) {
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

  spiderweb::reflect::JsonDocumentReader reader(str);

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

struct TestPeople {
  std::string              name = "dd";
  int                      age = 0;
  float                    height = 0;
  std::vector<int>         intList;
  std::vector<std::string> stringList;

  bool operator==(const TestPeople &other) const {
    return name == other.name && height == other.height && intList == other.intList &&
           stringList == other.stringList;
  }
};

REFLECT_JSON_SIMPLE(TestPeople, name, age, height, intList, stringList)
// or below way
// REFLECT_JSON(TestPeople, (name, "name"), (age, "age"), (height, "height"), (intList,
// "intList"),
//              (stringList, "stringList"))

struct TestPeopleList {
  std::vector<TestPeople> peoples;
};
REFLECT_JSON(TestPeopleList, (peoples, "peoples"))

TEST(ReflectJson, FromJsonStruct) {
  const char *json = R"(
{
  "name": "xiaoming",
  "age": 33,
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  spiderweb::reflect::JsonDocumentReader serilizer(json);

  TestPeople people;

  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, 33);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(ReflectJson, FromJsonStructMissingSome) {
  const char *json = R"(
{
  "name": "xiaoming",
  "height": 1.74,
  "intList":[1,2,3,4],
  "stringList":["a","b","c"]
}
)";

  spiderweb::reflect::JsonDocumentReader serilizer(json);

  TestPeople people;
  people.age = -1;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.name, "xiaoming");
  EXPECT_EQ(people.age, -1);
  EXPECT_FLOAT_EQ(people.height, 1.74);
  EXPECT_EQ(people.intList, std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(people.stringList, std::vector<std::string>({"a", "b", "c"}));
}

TEST(ReflectJson, FromJsonArray) {
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

  spiderweb::reflect::JsonDocumentReader serilizer(json);

  TestPeopleList people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.peoples.size(), 4);
  EXPECT_EQ(people.peoples[0], (TestPeople{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[1], (TestPeople{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[2], (TestPeople{"name3", 55, 3.3, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people.peoples[3], (TestPeople{"name4", 66, 4.4, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(ReflectJson, FromJsonComplexArrayNoKey) {
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

  spiderweb::reflect::JsonDocumentReader serilizer(json);

  std::vector<TestPeople> people;
  serilizer.FromJson(&people);

  EXPECT_EQ(people.size(), 2);
  EXPECT_EQ(people[0], (TestPeople{"name1", 33, 1.1, {1, 2, 3, 4}, {"a", "b", "c"}}));
  EXPECT_EQ(people[1], (TestPeople{"name2", 44, 2.2, {1, 2, 3, 4}, {"a", "b", "c"}}));
}

TEST(ReflectJson, FromJsonSimpleArrayNoKey) {
  const char *json = R"(
[123, 456]
)";

  spiderweb::reflect::JsonDocumentReader serilizer(json);

  std::vector<int> intList;
  serilizer.FromJson(&intList);

  EXPECT_EQ(intList, std::vector<int>({123, 456}));
}

TEST(ReflectJson, serilizerComplexStruct) {
  Student st{};
  st.bool_value = true;
  st.uint64_value = 123;
  st.int64_value = -123;
  st.int_value = 33;
  st.float_value = -5.5;
  st.double_value = 99.999;
  st.str_value = "yyjson";
  st.intList = std::vector<int>({1, 2, 3});

  spiderweb::reflect::JsonDocumentWriter writer;

  writer.ToJson(&st);
  std::cout << writer.ToString() << '\n';
}

TEST(ReflectJson, serilizerPeoples) {
  TestPeopleList list;

  for (int i = 0; i < 3; ++i) {
    TestPeople people;

    people.age = i;
    people.height = 1.75;
    people.name = "your name";
    people.intList = std::vector<int>({1, 2, 3});
    people.stringList = std::vector<std::string>({"a", "b", "c"});

    list.peoples.push_back(people);
  }

  spiderweb::reflect::JsonDocumentWriter writer;

  writer.ToJson(&list);

  std::cout << writer.ToString() << '\n';
}

struct MyNode {
  int                     age;
  std::shared_ptr<MyNode> next;
};
REFLECT_JSON(MyNode, (age, "age"), (next, "next"))

TEST(ReflectJson, FromJsonRecursive) {
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

  spiderweb::reflect::JsonDocumentReader reader(str);

  MyNode root;
  reader.FromJson(&root);

  EXPECT_EQ(root.age, 123);
  EXPECT_TRUE(root.next);
  EXPECT_EQ(root.next->age, 666);
  EXPECT_TRUE(root.next->next);
  EXPECT_EQ(root.next->next->age, 777);
}

TEST(ReflectJson, ToJsonRecursive) {
  spiderweb::reflect::JsonDocumentWriter writer;

  MyNode root;

  root.age = 1;

  root.next.reset(new MyNode());
  root.next->age = 2;

  root.next->next.reset(new MyNode());
  root.next->next->age = 3;

  writer.ToJson(&root);

  std::cout << writer.ToString() << '\n';
}

struct ListNode {
  int                                    age;
  std::vector<std::shared_ptr<ListNode>> childs;
};
REFLECT_JSON(ListNode, (age, "age"), (childs, "childs"))

TEST(ReflectJson, ToJsonArrayRecursive) {
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

  spiderweb::reflect::JsonDocumentWriter writer;
  writer.ToJson(&root);

  std::cout << writer.ToString() << '\n';
}

TEST(ReflectJson, FromJsonArrayRecursive) {
  static const char *str = R"(
{
  "age": 1,
  "childs": [
    { "age": 2, "childs": null },
    { "age": 3, "childs": null }
  ]
}
    )";
  ListNode           root{};

  spiderweb::reflect::JsonDocumentReader writer(str);
  writer.FromJson(&root);

  EXPECT_EQ(root.age, 1);
  EXPECT_EQ(root.childs.size(), 2);

  EXPECT_EQ(root.childs[0]->age, 2);
  EXPECT_EQ(root.childs[1]->age, 3);
}

enum class EnumValue : uint8_t {
  kSuccess,
  kFailed,
};
REFLECT_ENUM(EnumValue, std::string, (kSuccess, "success"), (kFailed, "failed"))

struct Object {
  EnumValue value{EnumValue::kFailed};
};
REFLECT_JSON(Object, (value, "enumValue", std::string))

TEST(ReflectJson, ToJsonEnum) {
  Object obj;
  obj.value = EnumValue::kSuccess;
  spiderweb::reflect::JsonDocumentWriter writer;
  writer.ToJson(&obj);

  std::cout << writer.ToString() << '\n';
}

TEST(ReflectJson, FromJsonEnum) {
  static const char *xml = R"(
{
  "enumValue": "failed"
}
      )";

  Object obj;
  obj.value = EnumValue::kSuccess;

  spiderweb::reflect::JsonDocumentReader reader(xml);
  reader.FromJson(&obj);

  EXPECT_EQ(obj.value, EnumValue::kFailed);
}

enum class ErrorCode : uint8_t {
  kSuccess,
  kPassowdWrong,
  kUserNotFound,
};
REFLECT_ENUM(ErrorCode, std::string, (kSuccess, "ok"), (kPassowdWrong, "password"),
             (kUserNotFound, "user-not-found"))

struct LoginRequest {
  std::string user_name;
  std::string passwd;
};
REFLECT_JSON(LoginRequest, (user_name, "user_name"), (passwd, "password"))

struct LoginResponse {
  ErrorCode error;
};
REFLECT_JSON(LoginResponse, (error, "code", std::string))

struct ChangePasswdRequest {
  std::string user_name;
  std::string old_passwd;
  std::string new_passwd;
};

struct ChangePasswdResponse {
  ErrorCode error;
};

struct User {
  std::string user;
  std::string display;
};
REFLECT_JSON(User, (user), (display, "display"))

struct ListUsersRequest {
  int code;
};
REFLECT_JSON(ListUsersRequest, (code))

struct ListUsersResponse {
  std::vector<User> users;
};
REFLECT_JSON(ListUsersResponse, (users, "users"))

class RpcService {
 public:
  using MethodString = std::string;
  using Handler = std::function<void(const std::string &request, std::string &response)>;

  RpcService() = default;

  void Dispatch(const std::string &method, const std::string &request, std::string &response) {
    routes_[method](request, response);
  }

  template <typename Request, typename Response, typename T>
  void Register(const std::string &method, T *impl, void (T::*func)(const Request &, Response &)) {
    const auto handler = [=](const std::string &request, std::string &response) {
      Request                                req;
      Response                               resp;
      spiderweb::reflect::JsonDocumentReader reader(request.c_str());

      reader.FromJson(&req);

      (impl->*func)(req, resp);

      spiderweb::reflect::JsonDocumentWriter writer;

      writer.ToJson(&resp);
      response = writer.ToString();
    };

    routes_[method] = handler;
  }

 private:
  std::map<MethodString, Handler> routes_;
};

class UserService {
 public:
  explicit UserService(RpcService *service) {
    service->Register<LoginRequest, LoginResponse>("Login", this, &UserService::Login);
    service->Register<ListUsersRequest, ListUsersResponse>("UserList", this,
                                                           &UserService::UserList);
  }

  virtual ~UserService() = default;

  virtual void Login(const LoginRequest &req, LoginResponse &resp) {};

  virtual void UserList(const ListUsersRequest &, ListUsersResponse &resp) {
  }

  virtual void ChangePasswd(const ChangePasswdRequest &req, ChangePasswdResponse &resp) {
  }
};

class UserServiceImpl : public UserService {
 public:
  explicit UserServiceImpl(RpcService *service) : UserService(service) {
  }

  void Login(const LoginRequest &req, LoginResponse &resp) override {
    if (req.user_name != user_name_) {
      resp.error = ErrorCode::kUserNotFound;
      return;
    }
    resp.error = ErrorCode::kSuccess;
  }

  void UserList(const ListUsersRequest &, ListUsersResponse &resp) override {
    resp.users.push_back(User{"1", "用户1"});
    resp.users.push_back(User{"2", "用户2"});
  }

  void ChangePasswd(const ChangePasswdRequest &req, ChangePasswdResponse &resp) override {
    if (req.user_name != user_name_) {
      resp.error = ErrorCode::kUserNotFound;
      return;
    }

    if (req.old_passwd != passwd_) {
      resp.error = ErrorCode::kPassowdWrong;
      return;
    }

    passwd_ = req.new_passwd;
    resp.error = ErrorCode::kSuccess;
  }
  std::string user_name_ = "user";
  std::string passwd_ = "pass";
};

TEST(Service, run) {
  RpcService      service;
  UserServiceImpl impl(&service);

  std::string response;
  service.Dispatch("Login", R"---({"user_name":"abc", "password":"mypass"})---", response);
  puts(response.c_str());
  service.Dispatch("UserList", R"---({})---", response);
  puts(response.c_str());
}
