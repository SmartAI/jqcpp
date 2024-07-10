#include "jqcpp/json_parser.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace jqcpp;

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