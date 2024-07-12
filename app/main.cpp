#include "jqcpp/expression_interpreter.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/json_value.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <exception>
#include <iostream>

using namespace jqcpp;

std::string read_json_input() {
  std::string line;
  std::string json;
  while (std::getline(std::cin, line)) {
    json += line + "\n";
  }
  return json;
}

void print_help() {
  std::cout << "Usage: jqcpp <filter>\n"
            << "Filters:\n"
            << "  .key           - Access object property\n"
            << "  .[index]       - Access array element\n"
            << "  .[start:end]   - Array slicing\n"
            << "  .key + value   - Addition operation\n"
            << "  .key - value   - Subtraction operation\n"
            << "  length         - Get length of array or string\n"
            << "  keys           - Get keys of an object\n";
}

int main(int argc, char *argv[]) {
  // read from user input and print out
  if (argc != 2) {
    print_help();
    return 1;
  }

  std::string jq_expr(argv[1]);

  std::string json_input = read_json_input();
  try {
    jqcpp::json::JSONTokenizer tokenizer;
    jqcpp::json::JSONParser parser;
    jqcpp::json::JSONPrinter printer;

    auto tokens = tokenizer.tokenize(json_input);
    auto json_value = parser.parse(tokens);

    // std::cout << printer.print(json_value) << "\n";
    jqcpp::expr::JQInterpreter interpreter(jq_expr);
    const jqcpp::json::JSONValue &result = interpreter.execute(json_value);
    std::cout << printer.print(result) << "\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
