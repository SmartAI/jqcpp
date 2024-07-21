#include "jqcpp/jq_lex.hpp"
#include "jqcpp/jq_parser.hpp"
#include "jqcpp/json_value.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace jqcpp {

// Read JSON input from a stream
std::string read_json_input(std::istream &input = std::cin);

// Print help information
void print_help(std::ostream &output = std::cout);

// Main function to run jqcpp
int run_jqcpp(int argc, char *argv[], std::istream &input = std::cin,
              std::ostream &output = std::cout);

class JQInterpreter {
  jqcpp::expr::JQParser filter;

public:
  JQInterpreter() = delete;
  JQInterpreter(const JQInterpreter &) = delete;
  JQInterpreter(JQInterpreter &&) = delete;
  JQInterpreter &operator=(const JQInterpreter &) = delete;
  JQInterpreter &operator=(JQInterpreter &&) = delete;

  explicit JQInterpreter(const std::string &filter_string);
  json::JSONValue execute(const json::JSONValue &input);
  std::string execute(const std::string &input);
};

} // namespace jqcpp
