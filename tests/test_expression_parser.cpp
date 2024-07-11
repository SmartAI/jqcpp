#include "jqcpp/expression_parser.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp::expr;

TEST_CASE("ExpressionTokenizer tokenizes expressions correctly",
          "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Simple tokens") {
    auto tokens = tokenizer.tokenize(".[],:+-");
    REQUIRE(tokens.size() == 8); // Including End token
    REQUIRE(tokens[0].type == TokenType::Dot);
    REQUIRE(tokens[1].type == TokenType::LeftBracket);
    REQUIRE(tokens[2].type == TokenType::RightBracket);
    REQUIRE(tokens[3].type == TokenType::Comma);
    REQUIRE(tokens[4].type == TokenType::Colon);
    REQUIRE(tokens[5].type == TokenType::Plus);
    REQUIRE(tokens[6].type == TokenType::Minus);
  }

  SECTION("Identifiers and numbers") {
    auto tokens = tokenizer.tokenize("abc 123 _def45");
    REQUIRE(tokens.size() == 4); // Including End token
    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "abc");
    REQUIRE(tokens[1].type == TokenType::Number);
    REQUIRE(tokens[1].value == "123");
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].value == "_def45");
  }

  SECTION("Complex expression") {
    auto tokens = tokenizer.tokenize(".users[0].name");
    REQUIRE(tokens.size() == 6); // Including End token
    REQUIRE(tokens[0].type == TokenType::Dot);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].value == "users");
    REQUIRE(tokens[2].type == TokenType::LeftBracket);
    REQUIRE(tokens[3].type == TokenType::Number);
    REQUIRE(tokens[3].value == "0");
    REQUIRE(tokens[4].type == TokenType::RightBracket);
  }
}
