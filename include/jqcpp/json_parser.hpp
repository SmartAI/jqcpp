#pragma once
#include "json_tokenizer.hpp"
#include "json_value.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace jqcpp {
class JSONParser {
public:
  JSONValue parse(const std::vector<Token> &tokens);

private:
  JSONValue parse_value(const std::vector<Token> &tokens, std::size_t &index);
};

class JSONParserError : public std::runtime_error {
public:
  JSONParserError(const std::string &message) : std::runtime_error(message) {}
};

} // namespace jqcpp
