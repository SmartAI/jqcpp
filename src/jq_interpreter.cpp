// jq_interpreter.cpp
#include "jqcpp/jq_interpreter.hpp"
#include "jqcpp/jq_lex.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/pretty_printer.hpp"
#include <fstream>
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

void print_version(std::ostream &output) { output << "jqcpp version 1.0.0\n"; }

void print_help(std::ostream &output) {
  output << "Usage: jqcpp [options] <expression> [input-file]\n"
         << "Options:\n"
         << "  -h, --help     Display this help information\n"
         << "  -v, --version  Show version information\n"
         << "expression:\n"
         << "  .key           Access object property\n"
         << "  .[index]       Access array element\n"
         << "  .[start:end]   Array slicing\n"
         << "  .key + value   Addition operation\n"
         << "  .key - value   Subtraction operation\n"
         << "  length         Get length of array\n"
         << "  keys           Get keys of an object\n";
}

int run_jqcpp(int argc, char *argv[], std::istream &input,
              std::ostream &output) {
  if (argc < 2) {
    print_help(std::cerr);
    return 1;
  }
  std::string arg1 = argv[1];
  if (arg1 == "-h" || arg1 == "--help") {
    print_help(output);
    return 0;
  }
  if (arg1 == "-v" || arg1 == "--version") {
    print_version(output);
    return 0;
  }

  std::string expression = arg1;
  std::string input_file;
  if (argc > 2) {
    input_file = argv[2];
  }

  if (expression.empty()) {
    std::cerr << "Error: No expression provided\n";
    print_help(output);
  }

  std::string json_input;
  if (!input_file.empty()) {
    std::ifstream ifs(input_file);
    if (!ifs) {
      std::cerr << "Error: Cannot open file " << input_file << "\n";
      return 1;
    }
    json_input = read_json_input(ifs);
  } else {
    json_input = read_json_input(input);
  }

  try {
    // parse json object
    json::JSONTokenizer lexer;
    json::JSONParser parser;
    auto jvalue = parser.parse(lexer.tokenize(json_input));

    JQInterpreter interpreter(expression);
    auto result = interpreter.execute(jvalue);
    json::JSONPrinter printer;
    output << printer.print(result) << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    // throw std::runtime_error("run jqcpp failed: " + std::string(e.what()) +
    //                          "\n");
    return 1;
  }
}

} // namespace jqcpp
