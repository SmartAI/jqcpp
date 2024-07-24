// jq_evaluator.cpp
#include "jqcpp/jq_evaluator.hpp"
#include "jqcpp/jq_ast_node.hpp"

namespace jqcpp {

json::JSONValue JQEvaluator::evaluate(const ASTNode &node,
                                      const json::JSONValue &input) {
  contextStack.push(&input);
  auto result = node.accept(*this);
  contextStack.pop();
  return result;
}

json::JSONValue JQEvaluator::visitIdentity(const IdentityNode &node) {
  return currentContext().deepCopy();
}

json::JSONValue JQEvaluator::visitField(const FieldNode &node) {
  if (!currentContext().is_object()) {
    throw std::runtime_error("Cannot access field of non-object value");
  }
  return currentContext()[node.value].deepCopy();
}

json::JSONValue JQEvaluator::visitArrayIndex(const ArrayIndexNode &node) {

  auto leftResult = node.left->accept(*this);

  if (!leftResult.is_array()) {
    throw std::runtime_error("Cannot access index of non-array value");
  }

  const auto &array = leftResult.get_array();

  // 评估右侧（索引）表达式
  auto indexResult = node.right->accept(*this);

  // 确保索引是一个数字
  if (!indexResult.is_number()) {
    throw std::runtime_error("Array index must be a number");
  }

  size_t index = static_cast<size_t>(indexResult.get_number());

  if (index >= array.size()) {
    return json::JSONValue(nullptr);
  }

  return array[index].deepCopy();
}

json::JSONValue JQEvaluator::visitArraySlice(const ArraySliceNode &node) {

  auto array_node = node.array->accept(*this);

  if (!array_node.is_array()) {
    throw std::runtime_error("Cannot slice non-array value");
  }

  const auto &array = array_node.get_array();
  size_t start = 0;
  size_t end = array.size();

  if (node.start) {
    auto startValue = node.start->accept(*this);
    start = static_cast<size_t>(startValue.get_number());
  }
  if (node.end) {
    auto endValue = node.end->accept(*this);
    end = static_cast<size_t>(endValue.get_number());
  }

  start = std::min(start, array.size());
  end = std::min(end, array.size());

  std::vector<json::JSONValue> values;
  for (size_t i = start; i < end; ++i) {
    values.push_back(array[i].deepCopy());
  }

  return json::JSONValue(std::move(values));
}

json::JSONValue JQEvaluator::visitObjectAccess(const ObjectAccessNode &node) {
  auto result = node.left->accept(*this);
  contextStack.push(&result);
  auto finalResult = node.right->accept(*this);
  contextStack.pop();
  return finalResult;
}

json::JSONValue
JQEvaluator::visitObjectIterator(const ObjectIteratorNode &node) {
  auto leftResult = node.left->accept(*this);
  if (!leftResult.is_object()) {
    throw std::runtime_error("Cannot iterate over non-object value");
  }
  const auto &obj = leftResult.get_object();
  json::JSONArray result;
  for (const auto &[key, value] : obj) {
    result.push_back(value.deepCopy());
  }
  return json::JSONValue(std::move(result));
}

json::JSONValue JQEvaluator::visitAddition(const AdditionNode &node) {
  auto left = node.left->accept(*this);
  auto right = node.right->accept(*this);
  if (left.is_number() && right.is_number()) {
    return json::JSONValue(left.get_number() + right.get_number());
  }
  throw std::runtime_error("Addition is only supported for numbers");
}

json::JSONValue JQEvaluator::visitSubtraction(const SubtractionNode &node) {
  auto left = node.left->accept(*this);
  auto right = node.right->accept(*this);
  if (left.is_number() && right.is_number()) {
    return json::JSONValue(left.get_number() - right.get_number());
  }
  throw std::runtime_error("Subtraction is only supported for numbers");
}

json::JSONValue JQEvaluator::visitLength(const LengthNode &node) {
  if (currentContext().is_array()) {
    return json::JSONValue(
        static_cast<double>(currentContext().get_array().size()));
  } else if (currentContext().is_string()) {
    return json::JSONValue(
        static_cast<double>(currentContext().get_string().length()));
  } else if (currentContext().is_object()) {
    return json::JSONValue(
        static_cast<double>(currentContext().get_object().size()));
  }
  throw std::runtime_error(
      "Length is only supported for arrays, strings, and objects");
}

json::JSONValue JQEvaluator::visitKeys(const KeysNode &node) {
  if (!currentContext().is_object()) {
    throw std::runtime_error("Keys is only supported for objects");
  }
  const auto &obj = currentContext().get_object();
  json::JSONArray keys;
  for (const auto &[key, value] : obj) {
    keys.push_back(json::JSONValue(key));
  }
  return json::JSONValue(std::move(keys));
}

json::JSONValue JQEvaluator::visitPipe(const PipeNode &node) {
  auto leftResult = node.left->accept(*this);
  contextStack.push(&leftResult);
  auto finalResult = node.right->accept(*this);
  contextStack.pop();
  return finalResult;
}

json::JSONValue JQEvaluator::visitLiteral(const LiteralNode &node) {
  return json::JSONValue(std::stod(node.value));
}

json::JSONValue JQEvaluator::visitNumberLiteral(const NumberLiteralNode &node) {
  return json::JSONValue(std::move(node.value));
}

} // namespace jqcpp
