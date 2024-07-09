#pragma once
#include <string>
#include <vector>
namespace jqcpp {
class JSONTokenizer {
public:
  std::vector<std::string> tokenize(const std::string &json_string);

private:
  bool is_whilespace(char c);
  bool is_digit(char c);
};
} // namespace jqcpp
