#include "spiderweb/core/spiderweb_tree.h"

#include <unordered_map>

#include "absl/memory/memory.h"
#include "gtest/gtest.h"

class Element {
 public:
  using UPtr = std::unique_ptr<Element>;

  virtual ~Element() = default;

 protected:
  void SetName(const std::string &name) {
    name_ = name;
  }

  const std::string &Name() const {
    return name_;
  }

  void SetDisplay(const std::string &display) {
    display_ = display;
  }

  const std::string Display() const {
    return display_;
  }

  // enum class Kind : uint8_t {
  //   kRoot,
  //   kType,
  //   kField,
  //   kPodkField,
  // };
  //
  // Kind        node_type;
  // uint32_t    id;
  // std::string type_name;
  // std::string name;
 private:
  // uint32_t    id_;
  std::string name_;
  std::string display_;
};

class RootElement : public Element {};

class TypeElement : public Element {
 public:
  explicit TypeElement(const std::string &name, bool expandable = true) : expandable_(expandable) {
    SetName(name);
  }

  TypeElement(const TypeElement &other) : Element(other) {
    SetName(other.Name());
    SetDisplay(other.Display());
  }

  bool IsExpandable() const {
    return expandable_;
  }

 private:
  bool expandable_ = true;
  ;
};

class FieldElement : public Element {
 public:
  explicit FieldElement(const std::string &type_name, const std::string &name)
      : type_name_(type_name), name_(name) {
  }

 private:
  std::string type_name_;
  std::string name_;
};

class UnExpandableElement : public Element {};

static const TypeElement kFloat("float", false);
static const TypeElement kDouble("double", false);
static const TypeElement kInt32("int32", false);
static const TypeElement kString("string", false);

class TypeSystem {
 public:
  using Iterator = spiderweb::TreeIterator<Element::UPtr>;

  using TypeTree = spiderweb::Tree<Element::UPtr>;
  using LevelWalker = spiderweb::TreeWalker<Element::UPtr, spiderweb::TreeWalkPolicy::kLevelOrder>;

  TypeSystem() {
    DeclareType(absl::make_unique<TypeElement>(kFloat));
    DeclareType(absl::make_unique<TypeElement>(kDouble));
    DeclareType(absl::make_unique<TypeElement>(kInt32));
    DeclareType(absl::make_unique<TypeElement>(kString));
  }

  void DeclareType(const std::string &new_type_name) {
    DeclareType(absl::make_unique<TypeElement>(new_type_name));
  }

  void DeclareType(TypeElement::UPtr new_type) {
    tree_.Root().AddChild(std::move(new_type));
  }

  void RemoveDeclareType();

  // void AddDeclareFiled(const std::string &type_name, std::unique_ptr<FieldElement> field_name) {
  //   LevelWalker walker;
  //
  //   auto type_iter = walker.FindIf(tree_.Root(), [](const Iterator &iter) {});
  // }

  void RemoveField();

 private:
  TypeTree tree_;
};

namespace spiderweb {
class TreeTest : public testing::Test {
 public:
};

static uint32_t NextId() {
  static uint32_t id = 1;

  return id++;
}

TEST(test_suite_name, test_name) {
  TypeSystem system;

  system.DeclareType("Wind");

  // system.AddDeclareFiled("Wind", std::make_unique<FieldElement>("float", "power"));
  // system.AddFiled("Wind", new FieldElement("float", "speed"));
  //
  // system.DeclareType("Line");
  //
  // system.AddFiled("Line", new FieldElement("Wind", "w1"));
  // system.AddFiled("Line", new FieldElement("Wind", "w2"));
  //
  // system.AddFiled("Wind", new FieldElement("float", "shutdown"));
}

// struct TypeDeclare {
//   explicit TypeDeclare(const std::string &name) : type{Element::Kind::kType, NextId(), "", name}
//   {
//   }
//
//   void AddFiled(const std::string &field_type, const std::string &field_name) {
//     if (field_type == "float") {
//       fields.emplace_back(Element{Element::Kind::kPodkField, NextId(), field_type, field_name});
//       return;
//     }
//     fields.emplace_back(Element{Element::Kind::kField, NextId(), field_type, field_name});
//   }
//
//   Element              type;
//   std::vector<Element> fields;
// };
//
// class TypeTree {
//  public:
//   using ElementTree = Tree<Element>;
//   using LevelWalker = TreeWalker<Element, TreeWalkPolicy::kLevelOrder>;
//   using PreOrderWalker = TreeWalker<Element, TreeWalkPolicy::kPreOrder>;
//   using Iterator = TreeIterator<Element>;
//
//   TypeTree() : tree_(Element{Element::Kind::kType, 0, "Root"}) {
//   }
//
//   void DefineType(const TypeDeclare &declare) {
//     auto iter = tree_.Root().AddChild(declare.type);
//
//     for (const auto &field : declare.fields) {
//       iter.AddChild(CreateDefinitions(field));
//     }
//   }
//
//   ElementTree CreateDefinitions(const Element &node) {
//     if (node.node_type == Element::Kind::kPodkField) {
//       return ElementTree(Element{Element::Kind::kPodkField, NextId(), node.type_name,
//       node.name});
//     }
//
//     LevelWalker level_walker;
//
//     auto type_node = level_walker.FindIf(tree_.Root(), [&](const Iterator &iter) {
//       return iter->node_type == Element::Kind::kType && iter->name == node.type_name;
//     });
//
//     ElementTree definition(Element{Element::Kind::kField, NextId(), node.type_name, node.name});
//     std::unordered_map<Iterator, Iterator> iters;
//
//     PreOrderWalker order_walker;
//
//     order_walker.Walk(type_node, [&](uint32_t depth, const Iterator &iter) {
//       if (depth == 0) {
//         iters[iter] = definition.Root();
//       } else {
//         auto child = iters[iter.Parent()].AddChild(
//             Element{Element::Kind::kField, NextId(), iter->type_name, iter->name});
//         iters[iter] = child;
//       }
//       return TreeWalkStep::kContinueWalk;
//     });
//
//     return definition;
//   }
//
//   void Print() {
//     PreOrderWalker walker;
//
//     walker.Walk(tree_.Root(), [](uint32_t depth, Iterator &iter) {
//       printf("%*s %s %s(%d)\n", depth * 2, " ", iter->type_name.c_str(), iter->name.c_str(),
//              iter->id);
//       return TreeWalkStep::kContinueWalk;
//     });
//   }
//
//  private:
//   ElementTree tree_;
// };
//
// TEST_F(TreeTest, AddChild) {
//   TypeTree tree;
//
//   {
//     TypeDeclare Wind("Wind");
//
//     Wind.AddFiled("float", "power");
//     Wind.AddFiled("float", "speed");
//
//     for (int i = 0; i < 100000; ++i) {
//       Wind.AddFiled("float", std::to_string(i));
//     }
//
//     tree.DefineType(Wind);
//   }
//
//   {
//     TypeDeclare Line("Line");
//
//     Line.AddFiled("Wind", "w1");
//     Line.AddFiled("Wind", "w2");
//     Line.AddFiled("Wind", "w3");
//
//     tree.DefineType(Line);
//   }
//
//   {
//     TypeDeclare Station("Station");
//
//     Station.AddFiled("Line", "l1");
//     Station.AddFiled("Line", "l2");
//     Station.AddFiled("Line", "l3");
//
//     tree.DefineType(Station);
//   }
//
//   // tree.Print();
// }

class TreeWalkTest : public testing::Test {
 public:
  using IntTree = Tree<int>;
  using LevelWalker = TreeWalker<int, TreeWalkPolicy::kLevelOrder>;
  using PreOrderWalker = TreeWalker<int, TreeWalkPolicy::kPreOrder>;

  TreeWalkTest() : tree(0) {
  }

  void SetUp() override {
    auto _1 = tree.Root().AddChild(1);
    _1.AddChild(4);
    _1.AddChild(5);
    _1.AddChild(6);

    auto _2 = tree.Root().AddChild(2);
    _2.AddChild(7);
    _2.AddChild(8);
    _2.AddChild(9);

    auto _3 = tree.Root().AddChild(3);
    _3.AddChild(10);
    _3.AddChild(11);
    _3.AddChild(12);
  }

  IntTree tree;
};

TEST_F(TreeWalkTest, LevelOrderWalk) {
  std::vector<int> expect_printed = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  std::vector<int> printed;

  LevelWalker walker;

  walker.Walk(tree.Root(), [&](uint32_t, const TreeIterator<int> &iter) {
    printed.push_back(*iter.Get());
    return TreeWalkStep::kContinueWalk;
  });

  EXPECT_EQ(expect_printed, printed);
}

TEST_F(TreeWalkTest, PreOrderWalk) {
  std::vector<int> expect_printed = {0, 1, 4, 5, 6, 2, 7, 8, 9, 3, 10, 11, 12};
  std::vector<int> printed;

  PreOrderWalker walker;

  walker.Walk(tree.Root(), [&](uint32_t, const TreeIterator<int> &iter) {
    printed.push_back(*iter.Get());
    return TreeWalkStep::kContinueWalk;
  });

  EXPECT_EQ(expect_printed, printed);
}

// TEST_F(TreeTest, AddChild) {
//   Tree<int> tree;
//
//   tree.FindNode(10);
// }

}  // namespace spiderweb
