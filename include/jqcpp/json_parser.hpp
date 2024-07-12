#pragma once
#include "json_tokenizer.hpp"
#include "json_value.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace jqcpp::json {
class JSONParser {
public:
  JSONValue parse(const std::vector<Token> &tokens);

private:
  // parse methods
  JSONValue parse_value();
  JSONValue parse_object();
  JSONValue parse_array();
  void consume(TokenType expected_type);

  // iterators
  std::vector<Token>::const_iterator it;
  std::vector<Token>::const_iterator end;
};

class JSONParserError : public std::runtime_error {
public:
  JSONParserError(const std::string &message) : std::runtime_error(message) {}
};

} // namespace jqcpp::json
