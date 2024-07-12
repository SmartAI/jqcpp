#include "jqcpp/expression_interpreter.hpp"
#include "jqcpp/expression_tokenizer.hpp"

namespace jqcpp::expr {

JQInterpreter::JQInterpreter(const std::string &jq_expression) {
  ExpressionTokenizer tokenizer;
  Parser parser;
  auto tokens = tokenizer.tokenize(jq_expression);
  ast = parser.parse(tokens);
}

const JSONValue &JQInterpreter::execute(const JSONValue &input) const {
  return ast->evaluate(input);
}

} // namespace jqcpp::expr
