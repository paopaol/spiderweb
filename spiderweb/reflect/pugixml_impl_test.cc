#include <map>

#include "gtest/gtest.h"
#include "spiderweb/reflect/xml_node.h"
#include "spiderweb/type/spiderweb_variant.h"

struct Child {
  int64_t                              id = 0;
  spiderweb::reflect::PlaceHolderValue placeholder;
};
REFLECT_XML(Child, REFLECT_XML_ATTR((id, "id")), (placeholder))

enum class Sex : uint8_t { kBoy, kGril };

REFLECT_ENUM(Sex, std::string, (kBoy, "boy"), (kGril, "gril"))
REFLECT_ENUM(Sex, int, (kBoy, 1), (kGril, 2))

struct People {
  int         age = 0;
  float       height = 0;
  std::string name;
  bool        attri_bool_value = false;
  int64_t     address = 0;
  Child       child;
  bool        child_bool_value = false;
  Sex         child_value_sex{Sex::kBoy};
  Sex         attr_value_sex{Sex::kBoy};
};
// clang-format off
REFLECT_XML(People,
            REFLECT_XML_ATTR(
                (age, "age"), 
                (height, "height"), 
                (name, "name"),
                (attri_bool_value, "attri_bool_value"), 
                (attr_value_sex, "sex", int)
            ),
            (address, "address"), 
            (child, "child"), 
            (child_bool_value, "child_bool_value"),
            (child_value_sex, "sex", std::string)
            )
// clang-format on

struct PeopleList {
  std::vector<People> peoples;
};

TEST(pugixml_impl, FromXml) {
  static const char *xml = R"(
  <people age="53" height="3.33" name="xiaoming" attri_bool_value="true">
      <address>345</address>
      <child id="123"></child>
      <child_bool_value>0</child_bool_value>
  </people>

  )";

  People people;

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  reader.Read("people", people);
  EXPECT_EQ(people.address, 345);
  EXPECT_EQ(people.age, 53);
  EXPECT_EQ(people.child.id, 123);
  EXPECT_EQ(people.attri_bool_value, true);
  EXPECT_EQ(people.child_bool_value, false);
}

TEST(pugixml_impl, FromXmlInnerHtml) {
  static const char *xml = R"(<address>345</address>)";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  int64_t value;
  reader.Read(value);
  EXPECT_EQ(value, 345);
}

TEST(pugixml_impl, FromXmlInnerHtmlArray) {
  static const char *xml = R"(
  <root>
    <address_list>
      <address>0</address>
      <address>1</address>
      <address>2</address>
    </address_list>
  </root>
  )";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  std::vector<int64_t> values;
  reader.Read("address", values, "root", "address_list");
  EXPECT_EQ(values.size(), 3);
  EXPECT_EQ(values[0], 0);
  EXPECT_EQ(values[1], 1);
  EXPECT_EQ(values[2], 2);
}

TEST(pugixml_impl, FromXmlArray) {
  static const char *xml = R"(
<root>
  <people age="53" height="3.33" name="xiaoming">
      <address>345</address>
      <child id="123"></child>
  </people>
  <people age="54" height="3.33" name="xiaoming">
      <address>345</address>
      <child id="123"></child>
  </people>
</root>
  )";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  std::vector<People> peoples;
  reader.Read("people", peoples, "root");
  EXPECT_EQ(peoples.size(), 2);
  EXPECT_EQ(peoples[0].age, 53);
  EXPECT_EQ(peoples[1].age, 54);
}

TEST(pugixml_impl, FromXmlByIndex) {
  static const char *xml = R"(
<root>
  <people age="53" height="3.33" name="xiaoming">
      <address>345</address>
      <child id="123"></child>
  </people>
  <people age="54" height="3.33" name="xiaoming">
      <address>345</address>
      <child id="123"></child>
  </people>
</root>
  )";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  People people;
  reader.Read("people", people, 1, "root");
  EXPECT_EQ(people.age, 54);
}

TEST(pugixml_impl, WriteStruct) {
  spiderweb::reflect::XmlDocumentWriter writer;

  People people;

  people.address = 123;
  people.age = 45;
  people.name = "xiaoming";
  people.attri_bool_value = true;
  people.child_bool_value = false;
  people.height = 1.75;
  people.child.id = 333;
  people.child_value_sex = Sex::kGril;
  people.attr_value_sex = Sex::kGril;
  writer.Write("people", people);

  std::cout << writer.ToString() << '\n';
}

TEST(pugixml_impl, WriteSimpleBool) {
  spiderweb::reflect::XmlDocumentWriter writer;

  int bool_value = false;
  writer.Write("bool", bool_value);

  std::cout << writer.ToString() << '\n';
}

TEST(pugixml_impl, WriteSimpleInt) {
  spiderweb::reflect::XmlDocumentWriter writer;

  int int_value = 123;
  writer.Write("int", int_value);

  std::cout << writer.ToString();
}

TEST(pugixml_impl, WriteSimpleArrayInt) {
  spiderweb::reflect::XmlDocumentWriter writer;

  // or list
  std::vector<int> values({1, 2, 3, 4});

  writer.Write("int", values, "intList", "child1", "child2");

  std::cout << writer.ToString() << '\n';
}

TEST(pugixml_impl, WriteComplexdArray) {
  spiderweb::reflect::XmlDocumentWriter writer;

  // or list
  std::vector<People> peoples;

  {
    People people;
    people.address = 123;
    people.age = 45;
    people.name = "xiaoming";
    people.attri_bool_value = true;
    people.child_bool_value = false;
    people.height = 1.75;
    people.child.id = 333;

    peoples.emplace_back(people);
  }
  {
    People people;
    people.address = 456;
    people.age = 20;
    people.name = "daming";
    people.attri_bool_value = true;
    people.child_bool_value = true;
    people.height = 1.66;
    people.child.id = 457;

    peoples.emplace_back(people);
  }

  writer.Write("people", peoples, "china", "hebei", "somepeoples");

  std::cout << writer.ToString() << '\n';
}

TEST(pugixml_impl, FromXmlSharedPtr) {
  static const char *xml = R"(
  <people age="53" height="3.33" name="xiaoming">
      <address>345</address>
      <child id="123"></child>
  </people>
  )";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  std::shared_ptr<People> people;
  reader.Read("people", people);

  EXPECT_TRUE(people != nullptr);
  EXPECT_EQ(people->address, 345);
  EXPECT_EQ(people->age, 53);
  EXPECT_EQ(people->child.id, 123);
  EXPECT_FLOAT_EQ(people->height, 3.33);
  EXPECT_EQ(people->name, "xiaoming");
  EXPECT_EQ(people->address, 345);
}

TEST(pugixml_impl, WriteComplexdArraySharedPtr) {
  spiderweb::reflect::XmlDocumentWriter writer;

  // or list
  std::vector<std::shared_ptr<People>> peoples;

  {
    std::shared_ptr<People> people = std::make_shared<People>();
    people->address = 123;
    people->age = 45;
    people->name = "xiaoming";
    people->attri_bool_value = true;
    people->child_bool_value = false;
    people->height = 1.75;
    people->child.id = 333;

    peoples.emplace_back(people);
  }
  {
    std::shared_ptr<People> people = std::make_shared<People>();
    people->address = 456;
    people->age = 20;
    people->name = "daming";
    people->attri_bool_value = true;
    people->child_bool_value = true;
    people->height = 1.66;
    people->child.id = 457;

    peoples.emplace_back(people);
  }

  writer.Write("people", peoples, "china", "hebei", "somepeoples");

  std::cout << writer.ToString() << '\n';
}

struct Element {
  int                   value;
  std::shared_ptr<Element> left;
  std::shared_ptr<Element> right;
};

// REFLECT_XML(Node, REFLECT_XML_ATTR(), (value, "value"), (left, "left"), (right,
// "right"))
namespace spiderweb {
namespace reflect {
template <typename T>
struct XmlMeta<T, Element> {
  using XmlValue = T;
  using struct_type = Element;
  static void Read(XmlValue &node, struct_type &result) {
    {
      reflect ::detail ::ReadTagImpl(node, "value", result.value);
    }
    {
      reflect ::detail ::ReadTagImpl(node, "left", result.left);
    }
    {
      reflect ::detail ::ReadTagImpl(node, "right", result.right);
    }
  }
  static void Write(XmlValue &node, const struct_type &result) {
    {
      reflect ::detail ::WriteTagImpl(node, "value", result.value);
    }
    {
      reflect ::detail ::WriteTagImpl(node, "left", result.left);
    }
    {
      reflect ::detail ::WriteTagImpl(node, "right", result.right);
    }
  }
};
}  // namespace reflect
}  // namespace spiderweb

TEST(pugixml_impl, WriteNodeTree) {
  auto root = std::make_shared<Element>();

  root->value = 5;

  root->left = std::make_shared<Element>();
  root->left->value = 4;

  root->right = std::make_shared<Element>();
  root->right->value = 6;

  spiderweb::reflect::XmlDocumentWriter writer;
  writer.Write("rootNode", root);
  std::cout << writer.ToString() << '\n';
}

TEST(pugixml_impl, ReadNodeTree) {
  static const char *xml = R"(
<rootNode>
        <value>5</value>
        <left>
                <value>4</value>
        </left>
        <right>
                <value>6</value>
        </right>
</rootNode>
            )";

  std::shared_ptr<Element> tree;

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));
  reader.Read("rootNode", tree);

  EXPECT_TRUE(tree != nullptr);
  EXPECT_EQ(tree->value, 5);

  EXPECT_TRUE(tree->left != nullptr);
  EXPECT_EQ(tree->left->value, 4);

  EXPECT_TRUE(tree->right != nullptr);
  EXPECT_EQ(tree->right->value, 6);
  EXPECT_TRUE(tree->right->right == nullptr);
}

struct MaxSupportedStruct {
  int a1;
  int a2;
  int a3;
  int a4;
  int a5;
  int a6;
  int a7;
  int a8;
  int a9;
  int a10;
  int a11;
  int a12;
  int a13;
  int a14;
  int a15;
  int a16;
  int a17;
  int a18;
  int a19;
  int a20;
  int a21;
  int a22;
  int a23;
  int a24;
  int a25;
  int a26;
  int a27;
  int a28;
  int a29;
  int a30;
  int a31;
  int a32;
  int a33;
  int a34;
  int a35;
  int a36;
  int a37;
  int a38;
  int a39;
  int a40;
  int a41;
  int a42;
  int a43;
  int a44;
  int a45;
  int a46;
  int a47;
  int a48;
  int a49;
  int a50;
  int a51;
  int a52;
  int a53;
  int a54;
  int a55;
  int a56;
  int a57;
  int a58;
  int a59;
  int a60;
  int a61;
  int a62;
  int a63;
  int a64;
  int a65;
  int a66;
  int a67;
  int a68;
  int a69;
  int a70;
  int a71;

  spiderweb::reflect::PlaceHolderValue placeholder;
};

REFLECT_XML(MaxSupportedStruct,
            REFLECT_XML_ATTR((a1, "a1"), (a2, "a2"), (a3, "a3"), (a4, "a4"), (a5, "a5"), (a6, "a6"),
                             (a7, "a7"), (a8, "a8"), (a9, "a9"), (a10, "a10"), (a11, "a11"),
                             (a12, "a12"), (a13, "a13"), (a14, "a14"), (a15, "a15"), (a16, "a16"),
                             (a17, "a17"), (a18, "a18"), (a19, "a19"), (a20, "a20"), (a21, "a21"),
                             (a22, "a22"), (a23, "a23"), (a24, "a24"), (a25, "a25"), (a26, "a26"),
                             (a27, "a27"), (a28, "a28"), (a29, "a29"), (a30, "a30"), (a31, "a31"),
                             (a32, "a32"), (a33, "a33"), (a34, "a34"), (a35, "a35"), (a36, "a36"),
                             (a37, "a37"), (a38, "a38"), (a39, "a39"), (a40, "a40"), (a41, "a41"),
                             (a42, "a42"), (a43, "a43"), (a44, "a44"), (a45, "a45"), (a46, "a46"),
                             (a47, "a47"), (a48, "a48"), (a49, "a49"), (a50, "a50"), (a51, "a51"),
                             (a52, "a52"), (a53, "a53"), (a54, "a54"), (a55, "a55"), (a56, "a56"),
                             (a57, "a57"), (a58, "a58"), (a59, "a59"), (a60, "a60"), (a61, "a61"),
                             (a62, "a62"), (a63, "a63"), (a64, "a64"), (a65, "a65"), (a66, "a66"),
                             (a67, "a67"), (a68, "a68"), (a69, "a69"), (a70, "a70"), (a71, "a71")),
            (placeholder))

enum class ProperyType : uint8_t {
  kBool,
  kNumber,
  kString,
  kEnum,
  kColor,
  kVariables,
  kByteArray,
  kFloat,
  kRect,
  kAlignment,
  kFont,
};

REFLECT_ENUM(ProperyType, std::string, (kBool, "Bool"), (kNumber, "Number"), (kString, "String"),
             (kEnum, "Enum"), (kColor, "Color"), (kVariables, "Variables"),
             (kByteArray, "ByteArray"), (kFloat, "Float"), (kRect, "Rect"),
             (kAlignment, "Alignment"), (kFont, "Font"))

template <typename K, typename V>
class Map : public std::map<K, V> {
 public:
  explicit Map(const std::string &key) : key_tag_(key) {
  }

  const std::string &KeyTag() const {
    return key_tag_;
  }

 private:
  std::string key_tag_;
};

namespace spiderweb {
namespace reflect {
namespace detail {
template <typename XmlValue, typename KeyType, typename ValueType,
          template <typename, typename> class Map>
inline bool ReadTagImpl(XmlValue &node, const char *name, Map<KeyType, ValueType> &value) {
  using MetaType = reflect::XmlMeta<XmlValue, ValueType>;

  node.ForEachChilds(name, [&](const XmlValue &child) {
    ValueType result{};
    MetaType::Read(child, result);

    std::string key_tag;
    child.GetAttribute(value.KeyTag().c_str(), key_tag);
    value[std::move(key_tag)] = std::move(result);
  });
  return true;
}

}  // namespace detail
}  // namespace reflect
}  // namespace spiderweb

struct Property;
using PropertyPtr = std::shared_ptr<Property>;

struct Property {
  std::string                   name;
  ProperyType                   type;
  std::string                   var;
  Map<std::string, PropertyPtr> fields{"name"};
};

REFLECT_XML(Property, REFLECT_XML_ATTR((type, "type", std::string), (name, "name"), (var, "value")),
            (fields, "Property"))

struct Object {
  std::string type;
  std::string uuid;

  Map<std::string, PropertyPtr> fields{"name"};
};

REFLECT_XML(Object, REFLECT_XML_ATTR((type, "type"), (uuid, "uuid")), (fields, "Property"))

struct Public {
  std::string color;
  int         height;
  bool        showPoint;
  bool        transparent;
  int         width;

  std::vector<Object> objects;
};

REFLECT_XML(Public,
            REFLECT_XML_ATTR((color, "color"), (height, "height"), (width, "width"),
                             (showPoint, "showPoint"), (transparent, "transparent")),
            (objects, "Object"))

TEST(pugixml_impl, FromPublic) {
  static const char *xml = R"(
<Public color="#ffffff" containerdev="" height="1013" showPoint="0" transparent="1" width="1570">
    <Object type="2_display_widgets.label" uuid="{9a2c7b65-3367-4a5b-9833-596c8bbae6c1}">
        <Property name="objectName" type="ByteArray" value="label_1120"/>
        <Property name="lock" type="Bool" value=""/>
        <Property name="visiblegroup" type="Number" value=""/>
        <Property name="geometry" type="Rect">
            <Property name="x" type="Number" value="67"/>
            <Property name="y" type="Number" value="808"/>
            <Property name="Width" type="Number" value="81"/>
            <Property name="Height" type="Number" value="45"/>
        </Property>

        <Property name="z" type="Number" value="0"/>
        <Property name="type" tr="false" type="String" value="ProxyWidget"/>
        <Property name="transformOriginPoint" type="Transform">
            <Property name="rotate_x" type="Number" value="0"/>
            <Property name="rotate_y" type="Number" value="0"/>
        </Property>
        <Property name="rotation" type="Number" value="0"/>
        <Property name="frameShape" type="Enum" value="0"/>
        <Property name="frameShadow" type="Enum" value="16"/>
        <Property name="text" tr="false" type="String" value="11"/>
        <Property name="activated1" type="Bool" value=""/>
        <Property name="text0" tr="false" type="String" value="label1"/>
        <Property name="text1" tr="false" type="String" value="label2"/>

        <Property name="text2" tr="false" type="String" value=""/>
        <Property name="text3" tr="false" type="String" value=""/>
        <Property name="text4" tr="false" type="String" value=""/>
        <Property name="alignment" type="Alignment">
            <Property name="Horizonta" type="Enum" value="4"/>
            <Property name="Vertical" type="Enum" value="128"/>
        </Property>
        <Property name="font" type="Font">
            <Property name="Family" type="Enum" value="Cantarell"/>
            <Property name="Size" type="Number" value="18"/>
            <Property name="Bold" type="Bool" value="false"/>
            <Property name="Italic" type="Bool" value="false"/>
            <Property name="Underline" type="Bool" value="false"/>
            <Property name="Strikeout" type="Bool" value="false"/>
        </Property>
        <Property name="colorOfText" type="Color" value="#c7e6ff"/>
        <Property name="currentuser" type="Bool" value="false"/>
        <Property name="activated" type="Bool" value=""/>
        <Property name="variable" type="Variables" value="">
            <Property name="tag" type="ByteArray" value=""/>
            <Property name="rtdb_id" type="Number" value=""/>
            <Property name="sel_id" type="Number" value=""/>
            <Property name="desc" type="ByteArray" value=""/>
            <Property name="unit" type="ByteArray" value=""/>
        </Property>
        <Property name="value" type="Float" value="0"/>
        <Property name="quality" type="Number" value="-1"/>
    </Object>
</Public>

  )";

  spiderweb::reflect::XmlDocumentReader reader(xml, strlen(xml));

  Public root;
  reader.Read(root);
  int a = 1;
  printf("%d\n", a);
}
