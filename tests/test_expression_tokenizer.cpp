#include "jqcpp/expression_tokenizer.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp::expr;

TEST_CASE("ExpressionTokenizer basic functionality", "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Empty input") {
    auto tokens = tokenizer.tokenize("");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::End);
  }

  SECTION("Simple identifier") {
    auto tokens = tokenizer.tokenize("abc");
    CHECK(tokens.size() == 2);
    CHECK(tokens[0].type == TokenType::Identifier);
    CHECK(tokens[0].value == "abc");
    CHECK(tokens[1].type == TokenType::End);
  }

  SECTION("Number") {
    auto tokens = tokenizer.tokenize("123.45");
    CHECK(tokens.size() == 2);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "123.45");
    CHECK(tokens[1].type == TokenType::End);
  }

  SECTION("String") {
    auto tokens = tokenizer.tokenize("\"hello world\"");
    CHECK(tokens.size() == 2);
    CHECK(tokens[0].type == TokenType::String);
    CHECK(tokens[0].value == "hello world");
    CHECK(tokens[1].type == TokenType::End);
  }

  SECTION("Operators") {
    auto tokens = tokenizer.tokenize("+ - * / % == != < <= > >=");
    CHECK(tokens.size() == 12);
    CHECK(tokens[0].type == TokenType::Plus);
    CHECK(tokens[1].type == TokenType::Minus);
    CHECK(tokens[2].type == TokenType::Multiply);
    CHECK(tokens[3].type == TokenType::Divide);
    CHECK(tokens[4].type == TokenType::Modulo);
    CHECK(tokens[5].type == TokenType::EqualEqual);
    CHECK(tokens[6].type == TokenType::NotEqual);
    CHECK(tokens[7].type == TokenType::Less);
    CHECK(tokens[8].type == TokenType::LessEqual);
    CHECK(tokens[9].type == TokenType::Greater);
    CHECK(tokens[10].type == TokenType::GreaterEqual);
    CHECK(tokens[11].type == TokenType::End);
  }
}

TEST_CASE("ExpressionTokenizer complex expressions", "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Complex expression") {
    auto tokens = tokenizer.tokenize(".foo[123] | .bar == \"test\"");
    CHECK(tokens.size() == 11);
    CHECK(tokens[0].type == TokenType::Dot);
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "foo");
    CHECK(tokens[2].type == TokenType::LeftBracket);
    CHECK(tokens[3].type == TokenType::Number);
    CHECK(tokens[3].value == "123");
    CHECK(tokens[4].type == TokenType::RightBracket);
    CHECK(tokens[5].type == TokenType::Pipe);
    CHECK(tokens[6].type == TokenType::Dot);
    CHECK(tokens[7].type == TokenType::Identifier);
    CHECK(tokens[7].value == "bar");
    CHECK(tokens[8].type == TokenType::EqualEqual);
    CHECK(tokens[9].type == TokenType::String);
    CHECK(tokens[9].value == "test");
    CHECK(tokens[10].type == TokenType::End);
  }

  SECTION("Minus and Negative") {
    auto tokens = tokenizer.tokenize(".b-2*2");
    CHECK(tokens.size() == 7);
  }

  SECTION("Minus and Negative") {
    auto tokens = tokenizer.tokenize(".b-2*2");
    CHECK(tokens.size() == 7); // Including End token
    CHECK(tokens[0].type == TokenType::Dot);
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "b");
    CHECK(tokens[2].type == TokenType::Minus);
    CHECK(tokens[3].type == TokenType::Number);
    CHECK(tokens[3].value == "2");
    CHECK(tokens[4].type == TokenType::Multiply);
    CHECK(tokens[5].type == TokenType::Number);
    CHECK(tokens[5].value == "2");
    CHECK(tokens[6].type == TokenType::End);
  }

  SECTION("Minus as operator and negative sign") {
    auto tokens = tokenizer.tokenize("5 - -3 * -2");
    CHECK(tokens.size() == 8); // Including End token
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "5");
    CHECK(tokens[1].type == TokenType::Minus);
    CHECK(tokens[2].type == TokenType::Minus);
    CHECK(tokens[3].type == TokenType::Number);
    CHECK(tokens[3].value == "3");
    CHECK(tokens[4].type == TokenType::Multiply);
    CHECK(tokens[5].type == TokenType::Minus);
    CHECK(tokens[6].type == TokenType::Number);
    CHECK(tokens[6].value == "2");
    CHECK(tokens[7].type == TokenType::End);
  }

  SECTION("Minus in different contexts") {
    auto tokens = tokenizer.tokenize("(-5 + 3) - 2");
    CHECK(tokens.size() == 9); // Including End token
    CHECK(tokens[0].type == TokenType::LeftParen);
    CHECK(tokens[1].type == TokenType::Minus);
    CHECK(tokens[2].type == TokenType::Number);
    CHECK(tokens[2].value == "5");
    CHECK(tokens[3].type == TokenType::Plus);
    CHECK(tokens[4].type == TokenType::Number);
    CHECK(tokens[4].value == "3");
    CHECK(tokens[5].type == TokenType::RightParen);
    CHECK(tokens[6].type == TokenType::Minus);
    CHECK(tokens[7].type == TokenType::Number);
    CHECK(tokens[7].value == "2");
    CHECK(tokens[8].type == TokenType::End);
  }
}

TEST_CASE("ExpressionTokenizer error handling", "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Unterminated string") {
    CHECK_THROWS_AS(tokenizer.tokenize("\"unterminated"), TokenizerError);
  }
}

TEST_CASE("ExpressionTokenizer special cases", "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Keywords") {
    auto tokens = tokenizer.tokenize("and or not null true false");
    CHECK(tokens.size() == 7);
    CHECK(tokens[0].type == TokenType::And);
    CHECK(tokens[1].type == TokenType::Or);
    CHECK(tokens[2].type == TokenType::Not);
    CHECK(tokens[3].type == TokenType::Identifier);
    CHECK(tokens[3].value == "null");
    CHECK(tokens[4].type == TokenType::Identifier);
    CHECK(tokens[4].value == "true");
    CHECK(tokens[5].type == TokenType::Identifier);
    CHECK(tokens[5].value == "false");
  }

  SECTION("Numbers with scientific notation") {
    auto tokens = tokenizer.tokenize("1e10 2.5e-3");
    CHECK(tokens.size() == 3);
    CHECK(tokens[0].type == TokenType::Number);
    CHECK(tokens[0].value == "1e10");
    CHECK(tokens[1].type == TokenType::Number);
    CHECK(tokens[1].value == "2.5e-3");
  }

  SECTION("Identifiers with underscores and dollars") {
    auto tokens = tokenizer.tokenize("_var $var var_1");
    CHECK(tokens.size() == 4);
    CHECK(tokens[0].type == TokenType::Identifier);
    CHECK(tokens[0].value == "_var");
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "$var");
    CHECK(tokens[2].type == TokenType::Identifier);
    CHECK(tokens[2].value == "var_1");
  }
}

TEST_CASE("ExpressionTokenizer tokenizes expressions correctly",
          "[tokenizer]") {
  ExpressionTokenizer tokenizer;

  SECTION("Simple tokens") {
    auto tokens = tokenizer.tokenize(".[],:+-");
    CHECK(tokens.size() == 8); // Including End token
    CHECK(tokens[0].type == TokenType::Dot);
    CHECK(tokens[1].type == TokenType::LeftBracket);
    CHECK(tokens[2].type == TokenType::RightBracket);
    CHECK(tokens[3].type == TokenType::Comma);
    CHECK(tokens[4].type == TokenType::Colon);
    CHECK(tokens[5].type == TokenType::Plus);
    CHECK(tokens[6].type == TokenType::Minus);
  }

  SECTION("Identifiers and numbers") {
    auto tokens = tokenizer.tokenize("abc 123 _def45");
    CHECK(tokens.size() == 4); // Including End token
    CHECK(tokens[0].type == TokenType::Identifier);
    CHECK(tokens[0].value == "abc");
    CHECK(tokens[1].type == TokenType::Number);
    CHECK(tokens[1].value == "123");
    CHECK(tokens[2].type == TokenType::Identifier);
    CHECK(tokens[2].value == "_def45");
  }

  SECTION("Complex expression") {
    auto tokens = tokenizer.tokenize(".users[0].name");
    CHECK(tokens.size() == 8); // Including End token
    CHECK(tokens[0].type == TokenType::Dot);
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "users");
    CHECK(tokens[2].type == TokenType::LeftBracket);
    CHECK(tokens[3].type == TokenType::Number);
    CHECK(tokens[3].value == "0");
    CHECK(tokens[4].type == TokenType::RightBracket);
    CHECK(tokens[5].type == TokenType::Dot);
    CHECK(tokens[6].type == TokenType::Identifier);
    CHECK(tokens[6].value == "name");
  }
}
