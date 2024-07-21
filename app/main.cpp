#include "jqcpp/jq_interpreter.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  return jqcpp::run_jqcpp(argc, argv, std::cin, std::cout);
}
