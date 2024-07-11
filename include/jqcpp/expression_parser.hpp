#pragma once
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace jqcpp {

namespace expr {

enum class TokenType {
  Dot,
  LeftBracket,
  RightBracket,
  Identifier,
  Number,
  Pipe,
  Comma,
  LeftParen,
  RightParen,
  Semicolon,
  QuestionMark,
  Star,
  At,
  Dollar,
  String,
  Equals,
  Plus,
  Minus,
  Multiply,
  Divide,
  Modulo,
  EqualEqual,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  And,
  Or,
  Not,
  Colon,
  End
};

struct Token {
  TokenType type;
  std::string value;
  Token(TokenType t, std::string v = "") : type(t), value(std::move(v)) {}
};

class ExpressionTokenizer {
public:
  std::vector<Token> tokenize(const std::string &expr) {
    std::vector<Token> tokens;
    it = expr.begin();
    end = expr.end();

    while (it != end) {
      tokens.push_back(next_token());
    }

    tokens.push_back(Token(TokenType::End));
    return tokens;
  }

private:
  std::string::const_iterator it;
  std::string::const_iterator end;

  Token next_token() {
    skip_whitespace();
    if (it == end) {
      return Token(TokenType::End);
    }

    char c = *it;
    switch (c) {
    case '.':
      ++it;
      return Token(TokenType::Dot);
    case '[':
      ++it;
      return Token(TokenType::LeftBracket);
    case ']':
      ++it;
      return Token(TokenType::RightBracket);
    case ':':
      ++it;
      return Token(TokenType::Colon);
    case ',':
      ++it;
      return Token(TokenType::Comma);
    case '+':
      ++it;
      return Token(TokenType::Plus);
    case '-':
      ++it;
      return Token(TokenType::Minus);
    }

    if (std::isalpha(c) || c == '_') {
      return parse_identifier();
    }

    if (std::isdigit(c)) {
      return parse_number();
    }

    throw std::runtime_error("Unexpected character" + std::string(1, c));
  }

  void skip_whitespace() {
    while (it != end && std::isspace(*it)) {
      ++it;
    }
  }
  Token parse_identifier() {
    std::string value;
    while (it != end && (std::isalnum(*it) || *it == '_')) {
      value += *it++;
    }
    return Token(TokenType::Identifier, value);
  }

  Token parse_number() {
    std::string value;
    while (it != end && std::isdigit(*it)) {
      value += *it++;
    }
    return Token(TokenType::Number, value);
  }
};
} // namespace expr

} // namespace jqcpp
