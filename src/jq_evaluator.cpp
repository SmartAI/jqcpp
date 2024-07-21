// jq_evaluator.cpp
#include "jqcpp/jq_evaluator.hpp"
#include "jqcpp/jq_ast_node.hpp"

namespace jqcpp {

json::JSONValue JQEvaluator::evaluate(const ASTNode &node,
                                      const json::JSONValue &input) {
  currentInput = &input;
  return node.accept(*this);
}

json::JSONValue JQEvaluator::visitIdentity(const ASTNode &node) {
  return currentInput->deepCopy();
}

json::JSONValue JQEvaluator::visitField(const ASTNode &node) {
  if (!currentInput->is_object()) {
    throw std::runtime_error("Cannot access field of non-object value");
  }
  return (*currentInput)[node.value].deepCopy();
}

json::JSONValue JQEvaluator::visitArrayIndex(const ASTNode &node) {
  if (!currentInput->is_array()) {
    throw std::runtime_error("Cannot access index of non-array value");
  }
  const auto &array = currentInput->get_array();
  size_t index = static_cast<size_t>(std::stoi(node.right->value));
  if (index >= array.size()) {
    throw std::runtime_error("Array index out of bounds");
  }
  return array[index].deepCopy();
}

json::JSONValue JQEvaluator::visitArraySlice(const ASTNode &node) {
  if (!currentInput->is_array()) {
    throw std::runtime_error("Cannot slice non-array value");
  }
  const auto &array = currentInput->get_array();
  size_t start =
      node.left ? static_cast<size_t>(std::stoi(node.left->value)) : 0;
  size_t end = node.right ? static_cast<size_t>(std::stoi(node.right->value))
                          : array.size();
  start = std::min(start, array.size());
  end = std::min(end, array.size());

  std::vector<json::JSONValue> values;
  for (int i = 0; i < end - start; ++i) {
    values.push_back(array[start + i].deepCopy());
  }
  return json::JSONValue(std::move(values));
}

json::JSONValue JQEvaluator::visitObjectAccess(const ASTNode &node) {
  auto result = node.left->accept(*this);
  currentInput = &result;
  return node.right->accept(*this);
}

json::JSONValue JQEvaluator::visitObjectIterator(const ASTNode &node) {
  if (!currentInput->is_object()) {
    throw std::runtime_error("Cannot iterate over non-object value");
  }
  const auto &obj = currentInput->get_object();
  json::JSONArray result;
  for (const auto &[key, value] : obj) {
    result.push_back(value.deepCopy());
  }
  return json::JSONValue(std::move(result));
}

json::JSONValue JQEvaluator::visitAddition(const ASTNode &node) {
  auto left = node.left->accept(*this);
  auto right = node.right->accept(*this);
  if (left.is_number() && right.is_number()) {
    return json::JSONValue(left.get_number() + right.get_number());
  }
  throw std::runtime_error("Addition is only supported for numbers");
}

json::JSONValue JQEvaluator::visitSubtraction(const ASTNode &node) {
  auto left = node.left->accept(*this);
  auto right = node.right->accept(*this);
  if (left.is_number() && right.is_number()) {
    return json::JSONValue(left.get_number() - right.get_number());
  }
  throw std::runtime_error("Subtraction is only supported for numbers");
}

json::JSONValue JQEvaluator::visitLength(const ASTNode &node) {
  if (currentInput->is_array()) {
    return json::JSONValue(
        static_cast<double>(currentInput->get_array().size()));
  } else if (currentInput->is_string()) {
    return json::JSONValue(
        static_cast<double>(currentInput->get_string().length()));
  } else if (currentInput->is_object()) {
    return json::JSONValue(
        static_cast<double>(currentInput->get_object().size()));
  }
  throw std::runtime_error(
      "Length is only supported for arrays, strings, and objects");
}

json::JSONValue JQEvaluator::visitKeys(const ASTNode &node) {
  if (!currentInput->is_object()) {
    throw std::runtime_error("Keys is only supported for objects");
  }
  const auto &obj = currentInput->get_object();
  json::JSONArray keys;
  for (const auto &[key, value] : obj) {
    keys.push_back(json::JSONValue(key));
  }
  return json::JSONValue(std::move(keys));
}

json::JSONValue JQEvaluator::visitPipe(const ASTNode &node) {
  auto leftResult = node.left->accept(*this);
  currentInput = &leftResult;
  return node.right->accept(*this);
}

json::JSONValue JQEvaluator::visitLiteral(const ASTNode &node) {
  return json::JSONValue(std::stod(node.value));
}
} // namespace jqcpp
