#include "jqcpp/json_parser.hpp"

namespace jqcpp {
JSONValue JSONParser::parse(const std::vector<Token> &tokens) {
  std::size_t index = 0;
  return parse_value(tokens, index);
}

JSONValue JSONParser::parse_value(const std::vector<Token> &tokens,
                                  std::size_t &index) {
  if (index >= tokens.size()) {
    throw JSONParserError("Exception index");
  }

  // get a new token
  const auto &token = tokens[index++];
  switch (token.type) {
  case TokenType::Null:
    return JSONValue(nullptr);
  case TokenType::True:
    return JSONValue(true);
  case TokenType::False:
    return JSONValue(false);
  case TokenType::Number:
    return JSONValue(std::stod(token.value));
  case TokenType::String:
    return JSONValue(token.value);
  default:
    throw JSONParserError("Unrecognized token type");
  }
}

} // namespace jqcpp