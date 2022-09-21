#include <cstring>

#include "mystruct.h"
#include "reflect/reflect_xml.hpp"
#include "xml/pugixml/pugixml_xml_reflection.hpp"

static const char *xml = R"(
<bookstore>
  <book category="COOKING">
    <title lang="en">Everyday Italian</title>
    <author>Giada De Laurentiis</author>
    <year>2005</year>
    <price>30.00</price>
  </book>
  <book category="CHILDREN">
    <title lang="en">Harry Potter</title>
    <author>J K. Rowling</author>
    <year>2005</year>
    <price>29.99</price>
  </book>
  <book category="WEB">
    <title lang="en">XQuery Kick Start</title>
    <author>James McGovern</author>
    <author>Per Bothner</author>
    <author>Kurt Cagle</author>
    <author>James Linn</author>
    <author>Vaidyanathan Nagarajan</author>
    <year>2003</year>
    <price>49.99</price>
  </book>
  <book category="WEB">
    <title lang="en">Learning XML</title>
    <author>Erik T. Ray</author>
    <year>2003</year>
    <price>39.95</price>
  </book>
</bookstore>
)";

// clang-format off
REFLECT_ENUM(Category, std::string, 
        (kCooking, "COOKING"), 
        (kChildren, "CHILDREN"),
        (kWeb, "WEB"),
        (kUnkown, "")
)
REFLECT_XML(Title, 
        REFLECT_XML_ATTR(
            (lang, "lang")
       ),
 //must be nullptr here, 
 //
 //1. if nullptr, we read <title>'s innerHtml
 //2. if not nullptr(eg "somename"), we read child tag of <title> named <somename>
        (title, nullptr)
)
REFLECT_XML(Book, 
        REFLECT_XML_ATTR(
            (category, "category", std::string)
        ), 
        (title, "title"),
        (authores, "author"), 
        (year, "year"), 
        (price, "price")
)

REFLECT_XML(BookStore, 
        REFLECT_XML_ATTR(), 
        (books, "book"),
)
// clang-format on

int main(int /*argc*/, char ** /*argv*/) {
  // use it like this
  {
    reflect::PugiXmlReader reader(xml, strlen(xml));

    std::vector<Book> books;
    reader.Read("book", books, "bookstore");

    std::cout << books.size() << std::endl;
  }

  // or like this
  {
    reflect::PugiXmlReader reader(xml, strlen(xml));

    BookStore book_store;
    reader.Read("bookstore", book_store);

    std::cout << book_store.books.size() << std::endl;
  }
}
