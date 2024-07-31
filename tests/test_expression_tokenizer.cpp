#include "jqcpp/jq_lex.hpp"
#include <catch2/catch_all.hpp>

using namespace jqcpp;

TEST_CASE("JQLexer basic functionality", "[tokenizer]") {
  JQLexer tokenizer;

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
}

TEST_CASE("JQLexer complex expressions", "[tokenizer]") { JQLexer tokenizer; }

TEST_CASE("JQLexer error handling", "[tokenizer]") {
  JQLexer tokenizer;

  SECTION("Unterminated string") {
    CHECK_THROWS_AS(tokenizer.tokenize("\"unterminated"), TokenizerError);
  }
}

TEST_CASE("JQLexer special cases", "[tokenizer]") {
  JQLexer tokenizer;

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

TEST_CASE("JQLexer tokenizes expressions correctly", "[tokenizer]") {
  JQLexer tokenizer;

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
