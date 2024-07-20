#include "jqcpp/expression_interpreter.hpp"
#include "jqcpp/jqcpp.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <catch2/catch_all.hpp>
#include <sstream>

using namespace jqcpp;
using namespace jqcpp::expr;

// Helper function to run jqcpp and capture output
std::string run_jqcpp_test(const std::string &input,
                           const std::string &filter) {
  std::istringstream iss(input);
  std::ostringstream oss;
  const char *argv[] = {"jqcpp", filter.c_str()};
  run_jqcpp(2, const_cast<char **>(argv), iss, oss);
  return oss.str();
}

TEST_CASE("JSON Parsing", "[json]") {
  json::JSONTokenizer tokenizer;
  json::JSONParser parser;

  SECTION("Parse simple JSON") {
    std::string input = R"({"name": "John", "age": 30})";
    auto tokens = tokenizer.tokenize(input);
    auto result = parser.parse(tokens);
    CHECK(result.is_object());
    const auto &obj = result.get_object();
    CHECK(obj.size() == 2);
    CHECK(obj[0].first == "name");
    CHECK(obj[0].second.get_string() == "John");
    CHECK(obj[1].first == "age");
    CHECK(obj[1].second.get_number() == 30);
  }

  SECTION("Parse nested JSON") {
    std::string input =
        R"({"person": {"name": "John", "age": 30}, "scores": [95, 87, 92]})";
    auto tokens = tokenizer.tokenize(input);
    auto result = parser.parse(tokens);
    CHECK(result.is_object());
    const auto &obj = result.get_object();
    CHECK(obj.size() == 2);
    CHECK(obj[0].first == "person");
    CHECK(obj[0].second.is_object());
    CHECK(obj[1].first == "scores");
    CHECK(obj[1].second.is_array());
  }
}

TEST_CASE("Identity filter", "[filter]") {
  std::string input = R"({"name": "John", "age": 30})";
  std::string filter = ".";
  std::string expected = R"({
  "name": "John",
  "age": 30
}
)";
  CHECK(run_jqcpp_test(input, filter) == expected);
}

TEST_CASE("Object property access", "[filter]") {
  std::string input = R"({"name": "John", "age": 30})";

  SECTION("Simple property access") {
    std::string filter = ".name";
    std::string expected = "\"John\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Nested property access") {
    input = R"({"person": {"name": "John", "age": 30}})";
    std::string filter = ".person.name";
    std::string expected = "\"John\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }
}

TEST_CASE("Array element access", "[filter]") {
  std::string input = R"(["a", "b", "c"])";

  SECTION("Access by index") {
    std::string filter = ".[1]";
    std::string expected = "\"b\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  // TODO: out of bound return a null
  SECTION("Access out of bounds") {
    std::string filter = ".[10]";
    std::string expected = "null\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }
}

TEST_CASE("Array slicing", "[filter]") {
  std::string input = R"([0, 1, 2, 3, 4])";

  SECTION("Simple slice") {
    std::string filter = ".[1:3]";
    std::string expected = R"([
  1,
  2
]
)";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Slice to end") {
    std::string filter = ".[2:]";
    std::string expected = R"([
  2,
  3,
  4
]
)";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Slice from start") {
    std::string filter = ".[:3]";
    std::string expected = R"([
  0,
  1,
  2
]
)";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }
}

TEST_CASE("Arithmetic operations", "[filter]") {
  std::string input = R"({"a": 5, "b": 3})";

  SECTION("Addition") {
    std::string filter = ".a + .b";
    std::string expected = "8\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Subtraction") {
    std::string filter = ".a - .b";
    std::string expected = "2\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }
}

// TEST_CASE("Length function", "[filter]") {
//   SECTION("Array length") {
//     std::string input = R"([1, 2, 3, 4, 5])";
//     std::string filter = "length";
//     std::string expected = "5\n";
//     CHECK(run_jqcpp_test(input, filter) == expected);
//   }
//
//   SECTION("String length") {
//     std::string input = R"("Hello, World!")";
//     std::string filter = "length";
//     std::string expected = "13\n";
//     CHECK(run_jqcpp_test(input, filter) == expected);
//   }
//
//   SECTION("Object length") {
//     std::string input = R"({"a": 1, "b": 2, "c": 3})";
//     std::string filter = "length";
//     std::string expected = "3\n";
//     CHECK(run_jqcpp_test(input, filter) == expected);
//   }
// }
//
// TEST_CASE("Keys function", "[filter]") {
//   std::string input = R"({"a": 1, "b": 2, "c": 3})";
//   std::string filter = "keys";
//   std::string expected = R"([
//   "a",
//   "b",
//   "c"
// ]
// )";
//   CHECK(run_jqcpp_test(input, filter) == expected);
// }

TEST_CASE("Complex nested structure", "[filter]") {
  std::string input = R"(
{
  "name": "John Doe",
  "age": 30,
  "address": {
    "street": "123 Main St",
    "city": "Anytown"
  },
  "phones": [
    {"type": "home", "number": "555-1234"},
    {"type": "work", "number": "555-5678"}
  ]
}
)";

  SECTION("Nested object access") {
    std::string filter = ".address.city";
    std::string expected = "\"Anytown\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Array of objects access") {
    std::string filter = ".phones[1].number";
    std::string expected = "\"555-5678\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }
}

// TEST_CASE("Error handling", "[filter]") {
//   SECTION("Invalid JSON") {
//     std::string input = R"({"name": "John", "age": })";
//     std::string filter = ".";
//     CHECK_THROWS(run_jqcpp_test(input, filter));
//   }
//
//   SECTION("Invalid filter") {
//     std::string input = R"({"name": "John", "age": 30})";
//     std::string filter = ".invalid[]";
//     CHECK_THROWS(run_jqcpp_test(input, filter));
//   }
// }

// TEST_CASE("Multiple outputs", "[filter]") {
//   std::string input = R"(["a", "b", "c"])";
//   std::string filter = ".[]";
//   std::string expected = R"("a"
// "b"
// "c"
// )";
//   CHECK(run_jqcpp_test(input, filter) == expected);
// }

// TEST_CASE("Pipe operator", "[filter]") {
//   std::string input = R"({"a": [1, 2, 3], "b": [4, 5, 6]})";
//   std::string filter = ".a | map(. * 2)";
//   std::string expected = R"([
//   2,
//   4,
//   6
// ]
// )";
//   CHECK(run_jqcpp_test(input, filter) == expected);
// }

// TEST_CASE("Object construction", "[filter]") {
//   std::string input = R"({"name": "John", "age": 30})";
//   std::string filter = "{name: .name, is_adult: .age >= 18}";
//   std::string expected = R"({
//   "name": "John",
//   "is_adult": true
// }
// )";
//   CHECK(run_jqcpp_test(input, filter) == expected);
// }

// TEST_CASE("Array construction", "[filter]") {
//   std::string input = R"({"a": 1, "b": 2, "c": 3})";
//   std::string filter = "[.a, .b, .c]";
//   std::string expected = R"([
//   1,
//   2,
//   3
// ]
// )";
//   CHECK(run_jqcpp_test(input, filter) == expected);
// }
//
// TEST_CASE("Conditional logic", "[filter]") {
//   std::string input = R"({"temperature": 25})";
//   std::string filter = "if .temperature > 20 then \"warm\" else \"cool\"
//   end"; std::string expected = "\"warm\"\n"; CHECK(run_jqcpp_test(input,
//   filter) == expected);
// }
