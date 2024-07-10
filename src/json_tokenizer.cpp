#include "jqcpp/json_tokenizer.hpp"
#include <vector>

namespace jqcpp {
std::vector<Token> JSONTokenizer::tokenize(const std::string &json_string) {
  std::vector<Token> tokens;
  // initialize the iterators
  it = json_string.begin();
  end = json_string.end();

  // begin parsing
  while (it != end) {
    auto token = next_token();
    // the end of input
    if (token.type == TokenType::EndOfInput) {
      break;
    }
    tokens.push_back(token);
  }
  return tokens;
}

Token JSONTokenizer::next_token() {
  skip_whitespace();
  if (it == end) {
    // end of input
    return Token(TokenType::EndOfInput, "");
  }
  char c = peek();
  switch (c) {
  case '{':
    get();
    return Token(TokenType::LeftBrace, "{");
  case '}':
    get();
    return Token(TokenType::RightBrace, "}");
  case '[':
    get();
    return Token(TokenType::LeftBracket, "[");
  case ']':
    get();
    return Token(TokenType::RightBracket, "]");
  case ',':
    get();
    return Token(TokenType::Comma, ",");
  case ':':
    get();
    return Token(TokenType::Colon, ":");
  case '"':
    return parse_string();
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return parse_number();
  case 't':
    return parse_true();
  case 'f':
    return parse_false();
  case 'n':
    return parse_null();
  default:
    throw JSONTokenizerError("Invalid input");
  }
}

void JSONTokenizer::skip_whitespace() {
  while ((it != end) && std::isspace(*it)) {
    ++it;
  }
}

Token JSONTokenizer::parse_null() {
  std::string value;
  std::string_view expected = "null";
  for (char c : expected) {
    if (get() != c) {
      throw JSONTokenizerError("Invalid token: expected 'null'");
    }
    value += c;
  }
  return Token(TokenType::Null, value);
}

Token JSONTokenizer::parse_false() {
  std::string value;
  std::string_view expected = "false";
  for (char c : expected) {
    if (get() != c) {
      throw JSONTokenizerError("Invalid token: expected 'false'");
    }
    value += c;
  }
  return Token(TokenType::False, value);
}

Token JSONTokenizer::parse_true() {
  std::string value;
  std::string_view expected = "true";
  for (char c : expected) {
    if (get() != c) {
      throw JSONTokenizerError("Invalid token: expected 'true'");
    }
    value += c;
  }
  return Token(TokenType::True, value);
}

Token JSONTokenizer::parse_number() {
  std::string value;
  if (peek() == '-') {
    value += get();
  }
  if (peek() == '0') {
    value += get();
  } else if (is_digit(peek())) {
    while (it != end && is_digit(peek())) {
      value += get();
    }
  } else {
    throw JSONTokenizerError("Invalid number format");
  }

  // fraction part
  if (peek() == '.') {
    value += get();
    // at least on digit
    if (!is_digit(peek())) {
      throw JSONTokenizerError(
          "Invalid number format: digit expected after dot");
    }
    while (it != end && is_digit(peek())) {
      value += get();
    }
  }

  // exponent part
  if (peek() == 'e' || peek() == 'E') {
    value += get();
    if (peek() == '+' || peek() == '-') {
      value += get();
    }
    if (!is_digit(peek())) {
      throw JSONTokenizerError("Invalid number format: digit expected");
    }
    while (it != end && is_digit(peek())) {
      value += get();
    }
  }
  return Token(TokenType::Number, value);
}

Token JSONTokenizer::parse_string() {
  std::string value;
  // skip leading "
  get();
  while (it != end) {
    char c = get();
    if (c == '"') {
      // end of string
      return Token(TokenType::String, value);
    } else if (c == '\\') {
      if (it == end) {
        throw JSONTokenizerError("Unexpected termination");
      }
      char esc = get();
      switch (esc) {
      case '"':
      case '\\':
      case '/':
        value += esc;
        break;
      case 'b':
        value += '\b';
        break;
      case 'f':
        value += '\f';
        break;
      case 'n':
        value += '\n';
        break;
      case 'r':
        value += '\r';
        break;
      case 't':
        value += '\t';
        break;
      case 'u': {

        // 4 hex digits
        std::string hex;
        for (int i = 0; i < 4; ++i) {
          if (it == end || !is_hex_digit(peek())) {
            throw JSONTokenizerError("Invalid Unicode sequence");
          }
          hex += get();
        }
        value += "\\u" + hex;
        break;
      }
      default:
        throw JSONTokenizerError("Invalid string sequence");
      }

    } else {
      value += c;
    }
  }
  // unterminated string sequence
  throw JSONTokenizerError("Unterminated string");
}

} // namespace jqcpp
