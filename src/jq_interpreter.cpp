// jq_interpreter.cpp
#include "jqcpp/jq_interpreter.hpp"
#include "jqcpp/jq_lex.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <iostream>

namespace jqcpp {

json::JSONValue JQInterpreter::execute(const std::string &jqExpression,
                                       const json::JSONValue &input) {
  JQLexer lexer;
  auto tokens = lexer.tokenize(jqExpression);
  auto ast = parser.parse(tokens);
  return evaluator.evaluate(*ast, input);
}

std::string read_json_input(std::istream &input) {
  std::string line;
  std::string json;
  while (std::getline(input, line)) {
    json += line + "\n";
  }
  return json;
}

void print_help(std::ostream &output) {
  output << "Usage: jqcpp <filter>\n"
         << "Filters:\n"
         << "  .key           - Access object property\n"
         << "  .[index]       - Access array element\n"
         << "  .[start:end]   - Array slicing\n"
         << "  .key + value   - Addition operation\n"
         << "  .key - value   - Subtraction operation\n"
         << "  length         - Get length of array or string\n"
         << "  keys           - Get keys of an object\n";
}

int run_jqcpp(int argc, char *argv[], std::istream &input,
              std::ostream &output) {
  if (argc != 2) {
    print_help(output);
    return 1;
  }

  std::string jq_expr(argv[1]);
  std::string json_input = read_json_input(input);

  try {

    JQInterpreter interpreter(jq_expr);
    auto result = interpreter.execute(json_input);
    json::JSONPrinter printer;
    std::cout << printer.print(result) << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

} // namespace jqcpp
