#include "jqcpp/json_tokenizer.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp::json;

TEST_CASE("JSONTokenizer handles structural tokens", "[tokenizer]") {
  JSONTokenizer tokenizer;
  auto tokens = tokenizer.tokenize("{}[],:");

  CHECK(tokens.size() == 6);
  CHECK(tokens[0].type == TokenType::LeftBrace);
  CHECK(tokens[1].type == TokenType::RightBrace);
  CHECK(tokens[2].type == TokenType::LeftBracket);
  CHECK(tokens[3].type == TokenType::RightBracket);
  CHECK(tokens[4].type == TokenType::Comma);
  CHECK(tokens[5].type == TokenType::Colon);
}

TEST_CASE("JSONTokenizer handles strings", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Simple string") {
    auto tokens = tokenizer.tokenize(R"("Hello, World!")");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::String);
    CHECK(tokens[0].value == "Hello, World!");
  }

  SECTION("String with escape sequences") {
    auto tokens = tokenizer.tokenize(R"("\"\\\/\b\f\n\r\t")");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::String);
    CHECK(tokens[0].value == "\"\\/\b\f\n\r\t");
  }

  SECTION("String with Unicode escape") {
    auto tokens = tokenizer.tokenize(R"("\u00A9")");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::String);
    CHECK(tokens[0].value == "\\u00A9");
  }
}

TEST_CASE("JSONTokenizer handles numbers", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Integer") {
    auto tokens = tokenizer.tokenize("123");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "123");
  }

  SECTION("Negative integer") {
    auto tokens = tokenizer.tokenize("-456");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "-456");
  }

  SECTION("Fraction") {
    auto tokens = tokenizer.tokenize("123.456");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "123.456");
  }

  SECTION("Exponent") {
    auto tokens = tokenizer.tokenize("1e-10");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "1e-10");
  }

  SECTION("Fraction and exponent") {
    auto tokens = tokenizer.tokenize("123.456e+78");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "123.456e+78");
  }
}

TEST_CASE("JSONTokenizer handles keywords", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("True") {
    auto tokens = tokenizer.tokenize("true");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::True);
  }

  SECTION("False") {
    auto tokens = tokenizer.tokenize("false");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::False);
  }

  SECTION("Null") {
    auto tokens = tokenizer.tokenize("null");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Null);
  }
}

TEST_CASE("JSONTokenizer handles whitespace", "[tokenizer]") {
  JSONTokenizer tokenizer;
  auto tokens = tokenizer.tokenize(" \n\r\t{ \n\r\t} \n\r\t");

  CHECK(tokens.size() == 2);
  CHECK(tokens[0].type == TokenType::LeftBrace);
  CHECK(tokens[1].type == TokenType::RightBrace);
}

TEST_CASE("JSONTokenizer handles complex JSON", "[tokenizer]") {
  JSONTokenizer tokenizer;
  std::string json = R"({
        "name": "John Doe",
        "age": 30,
        "isStudent": false,
        "grades": [95.5, 80.0, 88.5],
        "address": {
            "street": "123 Main St",
            "city": "Anytown"
        },
        "phoneNumbers": [
            {"type": "home", "number": "555-1234"},
            {"type": "work", "number": "555-5678"}
        ]
    })";

  auto tokens = tokenizer.tokenize(json);

  CHECK(tokens.size() == 59);
}

TEST_CASE("JSONTokenizer handles error cases", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Unterminated string") {
    CHECK_THROWS_AS(tokenizer.tokenize("\"unterminated"), JSONTokenizerError);
  }

  SECTION("Invalid escape sequence") {
    CHECK_THROWS_AS(tokenizer.tokenize("\"\\x\""), JSONTokenizerError);
  }

  SECTION("Invalid number format") {
    CHECK_THROWS_AS(tokenizer.tokenize("12.34.56"), JSONTokenizerError);
  }

  SECTION("Invalid token") {
    CHECK_THROWS_AS(tokenizer.tokenize("invalid"), JSONTokenizerError);
  }

  SECTION("Incomplete true") {
    CHECK_THROWS_AS(tokenizer.tokenize("tru"), JSONTokenizerError);
  }

  SECTION("Incomplete false") {
    CHECK_THROWS_AS(tokenizer.tokenize("fals"), JSONTokenizerError);
  }

  SECTION("Incomplete null") {
    CHECK_THROWS_AS(tokenizer.tokenize("nul"), JSONTokenizerError);
  }
}
