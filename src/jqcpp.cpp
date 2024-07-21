#include "jqcpp/jqcpp.hpp"
#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/json_value.hpp"
#include "jqcpp/pretty_printer.hpp"

namespace jqcpp {

JQInterpreter::JQInterpreter(const std::string &input_string)
    : filter(input_string) {
  filter.parse();
}

json::JSONValue JQInterpreter::execute(const json::JSONValue &input) {
  return filter.evaluate(input);
}

std::string JQInterpreter::execute(const std::string &input) {
  json::JSONTokenizer tokenizer;
  json::JSONParser parser;
  json::JSONPrinter printer;

  auto tokens = tokenizer.tokenize(input);
  auto jsonvalue = parser.parse(tokens);
  auto result = execute(jsonvalue);
  return printer.print(result);
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
    std::cout << result << "\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

} // namespace jqcpp
