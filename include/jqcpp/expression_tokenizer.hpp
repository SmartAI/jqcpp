#pragma once
#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace jqcpp {

namespace expr {

class TokenizerError : public std::runtime_error {
public:
  TokenizerError(const std::string &message) : std::runtime_error(message) {}
};

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

  char peek() const { return (it != end) ? *it : '\0'; }
  char peek_next() const { return (it + 1 != end) ? *(it + 1) : '\0'; }
  char get() { return (it != end) ? *it++ : '\0'; }

  Token process_identifier_state() {
    std::string value;
    while (it != end && is_identifier_part(peek())) {
      value += get();
    }
    // Check for keywords
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not},
        {"null", TokenType::Identifier}, // Treat null as a special identifier
        {"true", TokenType::Identifier}, // Treat true as a special identifier
        {"false", TokenType::Identifier} // Treat false as a special identifier
    };

    auto it = keywords.find(value);
    if (it != keywords.end()) {
      return Token(it->second, value);
    }
    return Token(TokenType::Identifier, value);
  }

  Token process_number_state() {
    std::string value;
    bool has_dot = false;
    bool has_e = false;

    while (it != end) {
      if (std::isdigit(peek())) {
        value += get();
      } else if (peek() == '.' && !has_dot && !has_e) {
        has_dot = true;
        value += get();
      } else if ((peek() == 'e' || peek() == 'E') && !has_e) {
        has_e = true;
        value += get();
        if (it != end && (peek() == '+' || peek() == '-')) {
          value += get();
        }
      } else {
        break;
      }
    }
    return Token(TokenType::Number, value);
  }

  Token process_string_state(char quote) {
    std::string value;
    get(); // skip the quote
    while (it != end && peek() != quote) {
      // escape
      if (peek() == '\\' && it + 1 != end) {
        get();
        switch (peek()) {
        case 'n':
          value += '\n';
          break;
        case 'r':
          value += '\r';
          break;
        case 't':
          value += '\t';
          break;
        case 'b':
          value += '\b';
          break;
        case 'f':
          value += '\f';
          break;
        default:
          value += peek();
        }
      } else {
        value += peek();
      }
      get();
    }
    if (it == end) {
      throw TokenizerError("Unterminated string");
    }
    get(); // skip ending quote
    return Token(TokenType::String, value);
  }
  Token process_operator_state() {
    static const std::unordered_map<std::string, TokenType> operators = {
        {".", TokenType::Dot},         {"|", TokenType::Pipe},
        {"(", TokenType::LeftParen},   {")", TokenType::RightParen},
        {"[", TokenType::LeftBracket}, {"]", TokenType::RightBracket},
        {",", TokenType::Comma},       {"=", TokenType::Equals},
        {";", TokenType::Semicolon},   {"?", TokenType::QuestionMark},
        {"@", TokenType::At},          {"$", TokenType::Dollar},
        {"+", TokenType::Plus},        {"-", TokenType::Minus},
        {"*", TokenType::Multiply},    {"/", TokenType::Divide},
        {"%", TokenType::Modulo},      {":", TokenType::Colon},
        {"==", TokenType::EqualEqual}, {"!=", TokenType::NotEqual},
        {"<", TokenType::Less},        {"<=", TokenType::LessEqual},
        {">", TokenType::Greater},     {">=", TokenType::GreaterEqual}};
    std::string op;
    op += get();
    if (it != end) {
      // determin whether is a valid two-characters op
      std::string potential_op = op + peek();
      if (operators.find(potential_op) != operators.end()) {
        op = potential_op;
        get();
      }
    }
    auto op_it = operators.find(op);
    if (op_it != operators.end()) {
      return Token(op_it->second, op);
    }
    throw TokenizerError("Unknow operator: " + op);
  }

  bool is_identifier_start(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
  }

  bool is_identifier_part(char c) const {
    return std::isalnum(c) || c == '_' || c == '$';
  }

  Token next_token() {
    skip_whitespace();
    if (it == end) {
      return Token(TokenType::End);
    }

    char c = peek();
    if (is_identifier_start(c)) {
      return process_identifier_state();
    }
    if (std::isdigit(c) || (c == '-') && std::isdigit(peek_next())) {
      return process_number_state();
    }
    if (c == '"' || c == '\'') {
      return process_string_state(c);
    }
    return process_operator_state();
  }

  void skip_whitespace() {
    while (it != end && std::isspace(peek())) {
      get();
    }
  }
};
} // namespace expr

} // namespace jqcpp
