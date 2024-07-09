#include "jqcpp/json_tokenizer.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp;

TEST_CASE("JSONTokenizer handles structural tokens", "[tokenizer]") {
  JSONTokenizer tokenizer;
  auto tokens = tokenizer.tokenize("{}[],:");

  REQUIRE(tokens.size() == 6);
  REQUIRE(tokens[0].type == TokenType::LeftBrace);
  REQUIRE(tokens[1].type == TokenType::RightBrace);
  REQUIRE(tokens[2].type == TokenType::LeftBracket);
  REQUIRE(tokens[3].type == TokenType::RightBracket);
  REQUIRE(tokens[4].type == TokenType::Comma);
  REQUIRE(tokens[5].type == TokenType::Colon);
}

TEST_CASE("JSONTokenizer handles strings", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Simple string") {
    auto tokens = tokenizer.tokenize(R"("Hello, World!")");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::String);
    REQUIRE(tokens[0].value == "Hello, World!");
  }

  SECTION("String with escape sequences") {
    auto tokens = tokenizer.tokenize(R"("\"\\\/\b\f\n\r\t")");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::String);
    REQUIRE(tokens[0].value == "\"\\/\b\f\n\r\t");
  }

  SECTION("String with Unicode escape") {
    auto tokens = tokenizer.tokenize(R"("\u00A9")");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::String);
    REQUIRE(tokens[0].value ==
            "\\u00A9"); // Note: Actual Unicode handling not implemented
  }
}

TEST_CASE("JSONTokenizer handles numbers", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Integer") {
    auto tokens = tokenizer.tokenize("123");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "123");
  }

  SECTION("Negative integer") {
    auto tokens = tokenizer.tokenize("-456");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "-456");
  }

  SECTION("Fraction") {
    auto tokens = tokenizer.tokenize("123.456");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "123.456");
  }

  SECTION("Exponent") {
    auto tokens = tokenizer.tokenize("1e-10");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "1e-10");
  }

  SECTION("Fraction and exponent") {
    auto tokens = tokenizer.tokenize("123.456e+78");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "123.456e+78");
  }
}

TEST_CASE("JSONTokenizer handles keywords", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("True") {
    auto tokens = tokenizer.tokenize("true");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::True);
  }

  SECTION("False") {
    auto tokens = tokenizer.tokenize("false");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::False);
  }

  SECTION("Null") {
    auto tokens = tokenizer.tokenize("null");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::Null);
  }
}

TEST_CASE("JSONTokenizer handles whitespace", "[tokenizer]") {
  JSONTokenizer tokenizer;
  auto tokens = tokenizer.tokenize(" \n\r\t{ \n\r\t} \n\r\t");

  REQUIRE(tokens.size() == 2);
  REQUIRE(tokens[0].type == TokenType::LeftBrace);
  REQUIRE(tokens[1].type == TokenType::RightBrace);
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

  REQUIRE(tokens.size() == 46);
  // Further assertions can be added to check specific tokens
}

TEST_CASE("JSONTokenizer handles error cases", "[tokenizer]") {
  JSONTokenizer tokenizer;

  SECTION("Unterminated string") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("\"unterminated"), JSONTokenizerError);
  }

  SECTION("Invalid escape sequence") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("\"\\x\""), JSONTokenizerError);
  }

  SECTION("Invalid number format") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("12.34.56"), JSONTokenizerError);
  }

  SECTION("Invalid token") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("invalid"), JSONTokenizerError);
  }

  SECTION("Incomplete true") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("tru"), JSONTokenizerError);
  }

  SECTION("Incomplete false") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("fals"), JSONTokenizerError);
  }

  SECTION("Incomplete null") {
    REQUIRE_THROWS_AS(tokenizer.tokenize("nul"), JSONTokenizerError);
  }
}
