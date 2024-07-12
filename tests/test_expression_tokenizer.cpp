#include "jqcpp/expression_tokenizer.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp::expr;

TEST_CASE("ExpressionTokenizer basic functionality", "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Empty input") {
    auto tokens = tokenizer.tokenize("");
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::End);
  }

  SECTION("Simple identifier") {
    auto tokens = tokenizer.tokenize("abc");
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "abc");
    REQUIRE(tokens[1].type == TokenType::End);
  }

  SECTION("Number") {
    auto tokens = tokenizer.tokenize("123.45");
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].value == "123.45");
    REQUIRE(tokens[1].type == TokenType::End);
  }

  SECTION("String") {
    auto tokens = tokenizer.tokenize("\"hello world\"");
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == TokenType::String);
    REQUIRE(tokens[0].value == "hello world");
    REQUIRE(tokens[1].type == TokenType::End);
  }

  SECTION("Operators") {
    auto tokens = tokenizer.tokenize("+ - * / % == != < <= > >=");
    REQUIRE(tokens.size() == 12);
    REQUIRE(tokens[0].type == TokenType::Plus);
    REQUIRE(tokens[1].type == TokenType::Minus);
    REQUIRE(tokens[2].type == TokenType::Multiply);
    REQUIRE(tokens[3].type == TokenType::Divide);
    REQUIRE(tokens[4].type == TokenType::Modulo);
    REQUIRE(tokens[5].type == TokenType::EqualEqual);
    REQUIRE(tokens[6].type == TokenType::NotEqual);
    REQUIRE(tokens[7].type == TokenType::Less);
    REQUIRE(tokens[8].type == TokenType::LessEqual);
    REQUIRE(tokens[9].type == TokenType::Greater);
    REQUIRE(tokens[10].type == TokenType::GreaterEqual);
    REQUIRE(tokens[11].type == TokenType::End);
  }
}

// TEST_CASE("ExpressionTokenizer complex expressions", "[tokenizer]") {
//   ExpressionTokenizer tokenizer;
//
//   SECTION("Complex expression") {
//     auto tokens = tokenizer.tokenize(".foo[123] | .bar == \"test\"");
//     REQUIRE(tokens.size() == 10);
//     REQUIRE(tokens[0].type == TokenType::Dot);
//     REQUIRE(tokens[1].type == TokenType::Identifier);
//     REQUIRE(tokens[1].value == "foo");
//     REQUIRE(tokens[2].type == TokenType::LeftBracket);
//     REQUIRE(tokens[3].type == TokenType::Number);
//     REQUIRE(tokens[3].value == "123");
//     REQUIRE(tokens[4].type == TokenType::RightBracket);
//     REQUIRE(tokens[5].type == TokenType::Pipe);
//     REQUIRE(tokens[6].type == TokenType::Dot);
//     REQUIRE(tokens[7].type == TokenType::Identifier);
//     REQUIRE(tokens[7].value == "bar");
//     REQUIRE(tokens[8].type == TokenType::EqualEqual);
//     REQUIRE(tokens[9].type == TokenType::String);
//     REQUIRE(tokens[9].value == "test");
//   }
// }
//
// TEST_CASE("ExpressionTokenizer error handling", "[tokenizer]") {
//   ExpressionTokenizer tokenizer;
//
//   SECTION("Unterminated string") {
//     REQUIRE_THROWS_AS(tokenizer.tokenize("\"unterminated"), TokenizerError);
//   }
//
//   SECTION("Unknown operator") {
//     REQUIRE_THROWS_AS(tokenizer.tokenize("@"), TokenizerError);
//   }
// }
//
// TEST_CASE("ExpressionTokenizer special cases", "[tokenizer]") {
//   ExpressionTokenizer tokenizer;
//
//   SECTION("Keywords") {
//     auto tokens = tokenizer.tokenize("and or not null true false");
//     REQUIRE(tokens.size() == 7);
//     REQUIRE(tokens[0].type == TokenType::And);
//     REQUIRE(tokens[1].type == TokenType::Or);
//     REQUIRE(tokens[2].type == TokenType::Not);
//     REQUIRE(tokens[3].type == TokenType::Identifier);
//     REQUIRE(tokens[3].value == "null");
//     REQUIRE(tokens[4].type == TokenType::Identifier);
//     REQUIRE(tokens[4].value == "true");
//     REQUIRE(tokens[5].type == TokenType::Identifier);
//     REQUIRE(tokens[5].value == "false");
//   }
//
//   SECTION("Numbers with scientific notation") {
//     auto tokens = tokenizer.tokenize("1e10 2.5e-3");
//     REQUIRE(tokens.size() == 3);
//     REQUIRE(tokens[0].type == TokenType::Number);
//     REQUIRE(tokens[0].value == "1e10");
//     REQUIRE(tokens[1].type == TokenType::Number);
//     REQUIRE(tokens[1].value == "2.5e-3");
//   }
//
//   SECTION("Identifiers with underscores and dollars") {
//     auto tokens = tokenizer.tokenize("_var $var var_1");
//     REQUIRE(tokens.size() == 4);
//     REQUIRE(tokens[0].type == TokenType::Identifier);
//     REQUIRE(tokens[0].value == "_var");
//     REQUIRE(tokens[1].type == TokenType::Identifier);
//     REQUIRE(tokens[1].value == "$var");
//     REQUIRE(tokens[2].type == TokenType::Identifier);
//     REQUIRE(tokens[2].value == "var_1");
//   }
// }
//
// TEST_CASE("ExpressionTokenizer tokenizes expressions correctly",
//           "[tokenizer]") {
//   ExpressionTokenizer tokenizer;
//
//   SECTION("Simple tokens") {
//     auto tokens = tokenizer.tokenize(".[],:+-");
//     REQUIRE(tokens.size() == 8); // Including End token
//     REQUIRE(tokens[0].type == TokenType::Dot);
//     REQUIRE(tokens[1].type == TokenType::LeftBracket);
//     REQUIRE(tokens[2].type == TokenType::RightBracket);
//     REQUIRE(tokens[3].type == TokenType::Comma);
//     REQUIRE(tokens[4].type == TokenType::Colon);
//     REQUIRE(tokens[5].type == TokenType::Plus);
//     REQUIRE(tokens[6].type == TokenType::Minus);
//   }
//
//   SECTION("Identifiers and numbers") {
//     auto tokens = tokenizer.tokenize("abc 123 _def45");
//     REQUIRE(tokens.size() == 4); // Including End token
//     REQUIRE(tokens[0].type == TokenType::Identifier);
//     REQUIRE(tokens[0].value == "abc");
//     REQUIRE(tokens[1].type == TokenType::Number);
//     REQUIRE(tokens[1].value == "123");
//     REQUIRE(tokens[2].type == TokenType::Identifier);
//     REQUIRE(tokens[2].value == "_def45");
//   }
//
//   SECTION("Complex expression") {
//     auto tokens = tokenizer.tokenize(".users[0].name");
//     REQUIRE(tokens.size() == 6); // Including End token
//     REQUIRE(tokens[0].type == TokenType::Dot);
//     REQUIRE(tokens[1].type == TokenType::Identifier);
//     REQUIRE(tokens[1].value == "users");
//     REQUIRE(tokens[2].type == TokenType::LeftBracket);
//     REQUIRE(tokens[3].type == TokenType::Number);
//     REQUIRE(tokens[3].value == "0");
//     REQUIRE(tokens[4].type == TokenType::RightBracket);
//   }
// }
