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
  Strinig,
  Number,
  True,
  False,
  Null
};

struct Token {
  TokenType type;
  std::string value;
  Token(TokenType t, const std::string &v = "") : type(t), value(v) {}
};

class JSONTokenizer {
public:
  std::vector<std::string> tokenize(const std::string &json_string);

private:
  bool is_whilespace(char c);
  bool is_digit(char c);
  std::string parse_string(const std::string &json_string, size_t &i);
  std::string parse_number(const std::string &json_string, size_t &i);
};

class JSONTokenizerError : public std::runtime_error {
public:
  JSONTokenizerError(const std::string &message)
      : std::runtime_error(message) {}
};
} // namespace jqcpp
