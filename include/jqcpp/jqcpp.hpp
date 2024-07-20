#pragma once
#include <iostream>
#include <string>

namespace jqcpp {

// Read JSON input from a stream
std::string read_json_input(std::istream &input = std::cin);

// Print help information
void print_help(std::ostream &output = std::cout);

// Main function to run jqcpp
int run_jqcpp(int argc, char *argv[], std::istream &input = std::cin,
              std::ostream &output = std::cout);

} // namespace jqcpp
