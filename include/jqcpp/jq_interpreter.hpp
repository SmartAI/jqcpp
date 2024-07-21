// jq_interpreter.hpp
#pragma once
#include "jq_evaluator.hpp"
#include "jq_parser.hpp"
#include "json_value.hpp"
#include <string>

namespace jqcpp {

int run_jqcpp(int argc, char *argv[], std::istream &input,
              std::ostream &output);

class JQInterpreter {
  std::string expr_;

public:
  JQInterpreter(const std::string &expr) : expr_(expr) {}
  json::JSONValue execute(const std::string &jqExpression,
                          const json::JSONValue &input);
  json::JSONValue execute(const json::JSONValue &input) {
    return execute(expr_, input);
  }

private:
  JQParser parser;
  JQEvaluator evaluator;
};

} // namespace jqcpp
