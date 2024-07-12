#include "jqcpp/json_parser.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace jqcpp::json;

auto find_in_object(const JSONObject &obj, const std::string &key) {
  return std::find_if(obj.begin(), obj.end(),
                      [&key](const auto &pair) { return pair.first == key; });
}

TEST_CASE("JSONParser parses simple types", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  SECTION("Parse null") {
    auto tokens = tokenizer.tokenize("null");
    auto result = parser.parse(tokens);
    CHECK(result.is_null());
  }

  SECTION("Parse boolean") {
    auto tokens = tokenizer.tokenize("true");
    auto result = parser.parse(tokens);
    CHECK(result.is_bool());
    CHECK(result.get_bool() == true);

    tokens = tokenizer.tokenize("false");
    result = parser.parse(tokens);
    CHECK(result.is_bool());
    CHECK(result.get_bool() == false);
  }

  SECTION("Parse number") {
    auto tokens = tokenizer.tokenize("123.45");
    auto result = parser.parse(tokens);
    CHECK(result.is_number());
    CHECK_THAT(result.get_number(), Catch::Matchers::WithinAbs(123.45, 1e-10));
  }

  SECTION("Parse string") {
    auto tokens = tokenizer.tokenize("\"Hello, World!\"");
    auto result = parser.parse(tokens);
    CHECK(result.is_string());
    CHECK(result.get_string() == "Hello, World!");
  }
}

TEST_CASE("JSONParser parses arrays", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  SECTION("Parse empty array") {
    auto tokens = tokenizer.tokenize("[]");
    auto result = parser.parse(tokens);
    CHECK(result.is_array());
    CHECK(result.get_array().empty());
  }

  SECTION("Parse simple array") {
    auto tokens = tokenizer.tokenize("[1, 2, 3]");
    auto result = parser.parse(tokens);
    CHECK(result.is_array());
    const auto &arr = result.get_array();
    CHECK(arr.size() == 3);
    CHECK(arr[0].get_number() == 1);
    CHECK(arr[1].get_number() == 2);
    CHECK(arr[2].get_number() == 3);
  }

  SECTION("Parse mixed-type array") {
    auto tokens = tokenizer.tokenize(R"([1, "two", true, null])");
    auto result = parser.parse(tokens);
    CHECK(result.is_array());
    const auto &arr = result.get_array();
    CHECK(arr.size() == 4);
    CHECK(arr[0].get_number() == 1);
    CHECK(arr[1].get_string() == "two");
    CHECK(arr[2].get_bool() == true);
    CHECK(arr[3].is_null());
  }

  SECTION("Parse nested array") {
    auto tokens = tokenizer.tokenize("[[1, 2], [3, 4]]");
    auto result = parser.parse(tokens);
    CHECK(result.is_array());
    const auto &arr = result.get_array();
    CHECK(arr.size() == 2);
    CHECK(arr[0].is_array());
    CHECK(arr[1].is_array());
    CHECK(arr[0].get_array()[0].get_number() == 1);
    CHECK(arr[0].get_array()[1].get_number() == 2);
    CHECK(arr[1].get_array()[0].get_number() == 3);
    CHECK(arr[1].get_array()[1].get_number() == 4);
  }
}

TEST_CASE("JSONParser handles error cases", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  SECTION("Unexpected end of tokens") {
    auto tokens = tokenizer.tokenize("{");
    CHECK_THROWS_AS(parser.parse(tokens), JSONParserError);
  }

  SECTION("Unexpected token type") {
    auto tokens = tokenizer.tokenize("{]");
    CHECK_THROWS_AS(parser.parse(tokens), JSONParserError);
  }

  SECTION("Missing colon in object") {
    auto tokens = tokenizer.tokenize(R"({"key" "value"})");
    CHECK_THROWS_AS(parser.parse(tokens), JSONParserError);
  }

  SECTION("Trailing comma in array") {
    auto tokens = tokenizer.tokenize("[1, 2, 3,]");
    CHECK_THROWS_AS(parser.parse(tokens), JSONParserError);
  }

  SECTION("Trailing comma in object") {
    auto tokens = tokenizer.tokenize(R"({"a": 1, "b": 2,})");
    CHECK_THROWS_AS(parser.parse(tokens), JSONParserError);
  }
}

TEST_CASE("JSONParser parses objects", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  SECTION("Parse empty object") {
    auto tokens = tokenizer.tokenize("{}");
    auto result = parser.parse(tokens);
    CHECK(result.is_object());
    CHECK(result.get_object().empty());
  }

  SECTION("Parse simple object") {
    auto tokens = tokenizer.tokenize(R"({"name": "John", "age": 30})");
    auto result = parser.parse(tokens);
    CHECK(result.is_object());
    const auto &obj = result.get_object();
    CHECK(obj.size() == 2);
    auto name_it = find_in_object(obj, "name");
    REQUIRE(name_it != obj.end());
    CHECK(name_it->second.get_string() == "John");
    auto age_it = find_in_object(obj, "age");
    REQUIRE(age_it != obj.end());
    CHECK(age_it->second.get_number() == 30);
  }

  SECTION("Parse nested object") {
    auto tokens =
        tokenizer.tokenize(R"({"person": {"name": "John", "age": 30}})");
    auto result = parser.parse(tokens);
    CHECK(result.is_object());
    const auto &obj = result.get_object();
    CHECK(obj.size() == 1);
    auto person_it = find_in_object(obj, "person");
    REQUIRE(person_it != obj.end());
    CHECK(person_it->second.is_object());
    const auto &person = person_it->second.get_object();
    auto name_it = find_in_object(person, "name");
    REQUIRE(name_it != person.end());
    CHECK(name_it->second.get_string() == "John");
    auto age_it = find_in_object(person, "age");
    REQUIRE(age_it != person.end());
    CHECK(age_it->second.get_number() == 30);
  }
}

TEST_CASE("JSONParser handles complex nested structures", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  std::string json = R"({
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

  auto tokens = tokenizer.tokenize(json);
  auto result = parser.parse(tokens);

  CHECK(result.is_object());
  const auto &obj = result.get_object();

  auto name_it = find_in_object(obj, "name");
  REQUIRE(name_it != obj.end());
  CHECK(name_it->second.get_string() == "John Doe");

  auto age_it = find_in_object(obj, "age");
  REQUIRE(age_it != obj.end());
  CHECK(age_it->second.get_number() == 30);

  auto isStudent_it = find_in_object(obj, "isStudent");
  REQUIRE(isStudent_it != obj.end());
  CHECK(isStudent_it->second.get_bool() == false);

  auto grades_it = find_in_object(obj, "grades");
  REQUIRE(grades_it != obj.end());
  CHECK(grades_it->second.is_array());
  const auto &grades = grades_it->second.get_array();
  CHECK(grades.size() == 3);
  CHECK_THAT(grades[0].get_number(), Catch::Matchers::WithinAbs(95.5, 1e-10));

  auto address_it = find_in_object(obj, "address");
  REQUIRE(address_it != obj.end());
  CHECK(address_it->second.is_object());
  const auto &address = address_it->second.get_object();
  auto street_it = find_in_object(address, "street");
  REQUIRE(street_it != address.end());
  CHECK(street_it->second.get_string() == "123 Main St");
  auto zipCode_it = find_in_object(address, "zipCode");
  REQUIRE(zipCode_it != address.end());
  CHECK(zipCode_it->second.get_string() == "12345");

  auto phoneNumbers_it = find_in_object(obj, "phoneNumbers");
  REQUIRE(phoneNumbers_it != obj.end());
  CHECK(phoneNumbers_it->second.is_array());
  const auto &phoneNumbers = phoneNumbers_it->second.get_array();
  CHECK(phoneNumbers.size() == 2);
  auto type_it = find_in_object(phoneNumbers[0].get_object(), "type");
  REQUIRE(type_it != phoneNumbers[0].get_object().end());
  CHECK(type_it->second.get_string() == "home");
  auto number_it = find_in_object(phoneNumbers[1].get_object(), "number");
  REQUIRE(number_it != phoneNumbers[1].get_object().end());
  CHECK(number_it->second.get_string() == "555-5678");
}
