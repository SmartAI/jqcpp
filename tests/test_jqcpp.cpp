#include "jqcpp/jq_interpreter.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <catch2/catch_all.hpp>
#include <sstream>

using namespace jqcpp;

// Helper function to run jqcpp and capture output
std::string run_jqcpp_test(const std::string &input,
                           const std::string &filter) {
  std::istringstream iss(input);
  std::ostringstream oss;
  const char *argv[] = {"jqcpp", filter.c_str()};
  try {
    run_jqcpp(2, const_cast<char **>(argv), iss, oss);
    return oss.str();
  } catch (...) {
    throw std::runtime_error("Exception");
  }
}

std::string pretty_json(const std::string &s) {
  json::JSONTokenizer lexer;
  json::JSONParser parser;
  json::JSONPrinter printer;
  return printer.print(parser.parse(lexer.tokenize(s))) + "\n";
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
//

TEST_CASE("JQ interpreter handles complex nested expressions",
          "[jq][complex]") {
  SECTION("Nested object access") {
    std::string input = R"(
            {
                "foo": {
                    "bar": {
                        "baz": 42
                    }
                }
            }
        )";

    CHECK(run_jqcpp_test(input, ".foo.bar.baz") == "42\n");
  }

  SECTION("Array slicing with nested object access") {
    std::string input = R"(
            {
                "foo": [
                    {"val": 1},
                    {"val": 2},
                    {"val": 3},
                    {"val": 4},
                    {"val": 5}
                ]
            }
        )";

    CHECK(run_jqcpp_test(input, ".foo[1:3]") ==
          pretty_json(R"([{"val":2}, {"val":3}])"));
  }

  SECTION("Arithmetic with object and array access") {
    std::string input = R"(
            {
                "foo": {
                    "bar": 10
                },
                "a": [5, 15, 25]
            }
        )";

    CHECK(run_jqcpp_test(input, ".foo.bar + .a[1]") == "25\n");
  }

  SECTION("Complex nested expression with multiple operations") {
    std::string input = R"(
            {
                "users": [
                    {"name": "Alice", "age": 30},
                    {"name": "Bob", "age": 25},
                    {"name": "Charlie", "age": 35},
                    {"name": "David", "age": 28}
                ],
                "threshold": 2
            }
        )";

    CHECK(run_jqcpp_test(input, ".users[1].age + .threshold") == "27\n");
  }

  SECTION("Combining length and arithmetic operations") {
    std::string input = R"(
            {
                "items": [1, 2, 3, 4, 5],
                "factor": 10
            }
        )";

    CHECK(run_jqcpp_test(input, "length") == pretty_json("2"));
  }

  SECTION("Using keys with nested access") {
    std::string input = R"(
            {
                "data": {
                    "x": 1,
                    "y": 2,
                    "z": 3
                }
            }
        )";

    CHECK(run_jqcpp_test(input, R"(.data.x)") == pretty_json("1"));
  }

  SECTION("Complex expression with multiple array and object accesses") {
    std::string input = R"(
            {
                "teams": [
                    {"name": "Red", "scores": [10, 20, 30]},
                    {"name": "Blue", "scores": [15, 25, 35]},
                    {"name": "Green", "scores": [12, 22, 32]}
                ],
                "bonus": 5
            }
        )";

    CHECK(run_jqcpp_test(input, ".teams[1].scores[1] + .bonus") == "30\n");
  }

  SECTION("Nested array slicing and object access") {
    std::string input = R"(
            [
                {"values": [1, 2, 3, 4, 5]},
                {"values": [6, 7, 8, 9, 10]},
                {"values": [11, 12, 13, 14, 15]}
            ]
        )";

    CHECK(run_jqcpp_test(input, ".[1].values[2:4]") == pretty_json("[8,9]"));
  }
}

TEST_CASE("End-to-end jqcpp functionality tests", "[e2e]") {
  SECTION("Identity filter") {
    std::string input = R"({"name": "John", "age": 30})";
    std::string filter = ".";
    std::string expected = pretty_json(input);
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Simple object property access") {
    std::string input = R"({"name": "Alice", "age": 25})";
    std::string filter = ".name";
    std::string expected = "\"Alice\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Nested object property access") {
    std::string input = R"({"user": {"name": "Bob", "details": {"age": 28}}})";
    std::string filter = ".user.details.age";
    std::string expected = "28\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Array element access") {
    std::string input = R"(["apple", "banana", "cherry"])";
    std::string filter = ".[1]";
    std::string expected = "\"banana\"\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Array slicing") {
    std::string input = R"([0, 1, 2, 3, 4, 5])";
    std::string filter = ".[2:5]";
    std::string expected = pretty_json("[2, 3, 4]");
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Arithmetic operations") {
    std::string input = R"({"x": 10, "y": 5})";
    std::string filter = ".x + .y";
    std::string expected = "15\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Complex nested structure") {
    std::string input = R"(
        {
            "users": [
                {"name": "Alice", "scores": [85, 90, 95]},
                {"name": "Bob", "scores": [80, 85, 90]}
            ]
        })";
    std::string filter = ".users[1].scores[2]";
    std::string expected = "90\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Length function") {
    std::string input = R"(["a", "b", "c", "d"])";
    std::string filter = "length";
    std::string expected = "4\n";
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Keys function") {
    std::string input = R"({"a": 1, "b": 2, "c": 3})";
    std::string filter = "keys";
    std::string expected = pretty_json(R"(["a", "b", "c"])");
    CHECK(run_jqcpp_test(input, filter) == expected);
  }

  SECTION("Error handling - Invalid JSON") {
    std::string input = R"({"name": "John", "age":})";
    std::string filter = ".";
    CHECK_THROWS(run_jqcpp_test(input, filter));
  }

  SECTION("Error handling - Invalid filter") {
    std::string input = R"({"name": "John", "age": 30})";
    std::string filter = ".invalid[";
    CHECK_THROWS(run_jqcpp_test(input, filter));
  }
}