#include "jqcpp/json_parser.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp;

auto find_in_object(const JSONObject &obj, const std::string &key) {
  return std::find_if(obj.begin(), obj.end(),
                      [&key](const auto &pair) { return pair.first == key; });
}

TEST_CASE("JSONPrinter handles simple types correctly", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  SECTION("Null value") {
    auto json = parser.parse(JSONTokenizer().tokenize("null"));
    CHECK(printer.print(json) == "null");
  }

  SECTION("Boolean values") {
    auto json_true = parser.parse(JSONTokenizer().tokenize("true"));
    CHECK(printer.print(json_true) == "true");

    auto json_false = parser.parse(JSONTokenizer().tokenize("false"));
    CHECK(printer.print(json_false) == "false");
  }

  SECTION("Number values") {
    auto json_int = parser.parse(JSONTokenizer().tokenize("42"));
    CHECK(printer.print(json_int) == "42");

    auto json_float = parser.parse(JSONTokenizer().tokenize("3.14159"));
    CHECK(printer.print(json_float) == "3.14159");

    auto json_negative = parser.parse(JSONTokenizer().tokenize("-273.15"));
    CHECK(printer.print(json_negative) == "-273.15");
  }

  SECTION("String value") {
    auto json = parser.parse(JSONTokenizer().tokenize("\"Hello, World!\""));
    CHECK(printer.print(json) == "\"Hello, World!\"");
  }
}

TEST_CASE("JSONPrinter handles empty structures correctly", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  SECTION("Empty object") {
    auto json = parser.parse(JSONTokenizer().tokenize("{}"));
    CHECK(printer.print(json) == "{}");
  }

  SECTION("Empty array") {
    auto json = parser.parse(JSONTokenizer().tokenize("[]"));
    CHECK(printer.print(json) == "[]");
  }
}

TEST_CASE("JSONPrinter formats objects correctly", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  SECTION("Simple object") {
    std::string input = R"({"name":"John","age":30})";
    auto json = parser.parse(JSONTokenizer().tokenize(input));
    std::string expected = "{\n  \"name\": \"John\",\n  \"age\": 30\n}";
    CHECK(printer.print(json) == expected);
  }

  SECTION("Nested object") {
    std::string input = R"({"person":{"name":"John","age":30}})";
    auto json = parser.parse(JSONTokenizer().tokenize(input));
    std::string expected =
        "{\n  \"person\": {\n    \"name\": \"John\",\n    \"age\": 30\n  }\n}";
    CHECK(printer.print(json) == expected);
  }
}

TEST_CASE("JSONPrinter formats arrays correctly", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  SECTION("Simple array") {
    std::string input = "[1,2,3]";
    auto json = parser.parse(JSONTokenizer().tokenize(input));
    std::string expected = "[\n  1,\n  2,\n  3\n]";
    CHECK(printer.print(json) == expected);
  }

  SECTION("Array of objects") {
    std::string input = R"([{"x":1},{"y":2}])";
    auto json = parser.parse(JSONTokenizer().tokenize(input));
    std::string expected =
        "[\n  {\n    \"x\": 1\n  },\n  {\n    \"y\": 2\n  }\n]";
    CHECK(printer.print(json) == expected);
  }
}

TEST_CASE("JSONPrinter handles complex nested structures", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  std::string input = R"({
        "name": "John Doe",
        "age": 30,
        "isStudent": false,
        "grades": [95.5, 80.0, 88.5],
        "address": {
            "street": "123 Main St",
            "city": "Anytown",
            "zipCode": "12345"
        },
        "phoneNumbers": [
            {"type": "home", "number": "555-1234"},
            {"type": "work", "number": "555-5678"}
        ]
    })";

  auto json = parser.parse(JSONTokenizer().tokenize(input));
  std::string expected = R"({
  "name": "John Doe",
  "age": 30,
  "isStudent": false,
  "grades": [
    95.5,
    80,
    88.5
  ],
  "address": {
    "street": "123 Main St",
    "city": "Anytown",
    "zipCode": "12345"
  },
  "phoneNumbers": [
    {
      "type": "home",
      "number": "555-1234"
    },
    {
      "type": "work",
      "number": "555-5678"
    }
  ]
})";

  CHECK(printer.print(json) == expected);
}

TEST_CASE("JSONPrinter formats objects correctly simple", "[printer]") {
  JSONParser parser;
  JSONPrinter printer;

  SECTION("Nested object") {
    std::string input = R"({"person":{"name":"John","age":30}})";
    auto json = parser.parse(JSONTokenizer().tokenize(input));
    std::string output = printer.print(json);

    // Parse the output again to compare the structure, not the exact string
    auto reparsed = parser.parse(JSONTokenizer().tokenize(output));

    CHECK(json.is_object());
    CHECK(reparsed.is_object());

    const auto &original_obj = json.get_object();
    const auto &reparsed_obj = reparsed.get_object();

    auto original_person_it = find_in_object(original_obj, "person");
    CHECK(original_person_it != original_obj.end());
    CHECK(original_person_it->second.is_object());

    auto reparsed_person_it = find_in_object(reparsed_obj, "person");
    CHECK(reparsed_person_it != reparsed_obj.end());
    CHECK(reparsed_person_it->second.is_object());

    const auto &original_person = original_person_it->second.get_object();
    const auto &reparsed_person = reparsed_person_it->second.get_object();

    auto original_name_it = find_in_object(original_person, "name");
    CHECK(original_name_it != original_person.end());
    auto reparsed_name_it = find_in_object(reparsed_person, "name");
    CHECK(reparsed_name_it != reparsed_person.end());
    CHECK(original_name_it->second.get_string() ==
          reparsed_name_it->second.get_string());

    auto original_age_it = find_in_object(original_person, "age");
    CHECK(original_age_it != original_person.end());
    auto reparsed_age_it = find_in_object(reparsed_person, "age");
    CHECK(reparsed_age_it != reparsed_person.end());
    CHECK(original_age_it->second.get_number() ==
          reparsed_age_it->second.get_number());
  }
}
