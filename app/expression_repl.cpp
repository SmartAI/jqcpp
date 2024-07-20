#include <iostream>
#include <jqcpp/expression_interpreter.hpp>

std::string read_json_input(std::istream &input) {
  std::string line;
  if (std::getline(input, line)) {
    return line;
  }
  return "q";
}

int main(int argc, char *argv[]) {
  while (true) {
    std::string filter = read_json_input(std::cin);
    if (filter == "q") {
      break;
    }
    jqcpp::expr::JQInterpreter interpreter(filter);
    std::cout << interpreter.prettyPrint() << "\n";
  };
  return 0;
}
