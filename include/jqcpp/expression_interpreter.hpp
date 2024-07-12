#pragma once
#include "expression_parser.hpp"
#include "json_value.hpp"

using jqcpp::json::JSONValue;

namespace jqcpp::expr {

class JQInterpreter {
  std::unique_ptr<Expression> ast;

public:
  JQInterpreter(const std::string &jq_expression);
  const JSONValue &execute(const JSONValue &input) const;
};

} // namespace jqcpp::expr
