#include "jqcpp/jq_interpreter.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_value.hpp"
#include <catch2/catch_all.hpp>

using jqcpp::JQInterpreter;
using jqcpp::json::JSONParser;
using jqcpp::json::JSONTokenizer;
using jqcpp::json::JSONValue;

TEST_CASE("Expression parser handles simple expressions", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;

  JSONValue input(
      parser.parse(tokenizer.tokenize("{\"name\": \"John\",\"age\": 30.0}")));
  SECTION("Identity") {
    JQInterpreter interpreter(".");
    const auto &result = interpreter.execute(input);
    CHECK(result.is_object());
    const auto &obj = result.get_object();
    CHECK(obj.size() == 2);
    CHECK(obj[0].first == "name");
    CHECK(obj[0].second.get_string() == "John");
    CHECK(obj[1].first == "age");
    CHECK(obj[1].second.get_number() == 30.0);
  }

  SECTION("Object access") {
    JQInterpreter interpreter(".name");
    const auto &result = interpreter.execute(input);
    CHECK(result.is_string());
    CHECK(result.get_string() == "John");
  }

  SECTION("Arithmetic") {
    JQInterpreter interpreter(".age + 5");
    const auto &result = interpreter.execute(input);
    CHECK(result.is_number());
    CHECK(result.get_number() == 35.0);
  }
}

TEST_CASE("Expression parser handles array operations", "[parser]") {
  JSONTokenizer tokenizer;
  JSONParser parser;
  JSONValue input(parser.parse(tokenizer.tokenize("[1.0,2.0,3.0]")));

  SECTION("Array access") {
    JQInterpreter interpreter(".[1]");
    const auto &result = interpreter.execute(input);
    CHECK(result.is_number());
    CHECK(result.get_number() == 2.0);
  }

  SECTION("Array slice") {
    JQInterpreter interpreter(".[1:3]");
    const auto &result = interpreter.execute(input);
    CHECK(result.is_array());
    const auto &arr = result.get_array();
    CHECK(arr.size() == 2);
    CHECK(arr[0].get_number() == 2.0);
    CHECK(arr[1].get_number() == 3.0);
  }
}
