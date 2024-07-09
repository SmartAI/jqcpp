#pragma once
#include <stdexcept>
#include <string>
#include <vector>
namespace jqcpp {

enum class TokenType {
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
  Comma,
  Colon,
  String,
  Number,
  True,
  False,
  Null,
  EndOfInput
};

struct Token {
  TokenType type;
  std::string value;
  Token(TokenType t, const std::string &v = "") : type(t), value(v) {}
};

class JSONTokenizer {
public:
  std::vector<Token> tokenize(const std::string &json_string);

private:
  std::string::const_iterator it;
  std::string::const_iterator end;

  Token next_token();
  Token parse_string();
  Token parse_number();
  Token parse_true();
  Token parse_false();
  Token parse_null();
  void skip_whitespace();
  char peek() const { return (it != end) ? *it : '\0'; }
  char get() { return (it != end) ? *it++ : '\0'; }
  bool is_digit(char c) const { return c >= '0' && c <= '9'; }
  bool is_hex_digit(char c) const {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }
};

class JSONTokenizerError : public std::runtime_error {
public:
  JSONTokenizerError(const std::string &message)
      : std::runtime_error(message) {}
};
} // namespace jqcpp
