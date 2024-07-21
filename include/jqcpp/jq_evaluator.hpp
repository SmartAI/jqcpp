// jq_evaluator.hpp
#pragma once

#include "jqcpp/jq_ast_visitor.hpp"
#include "jqcpp/json_value.hpp"
#include <stack>

namespace jqcpp {

class JQEvaluator : public ASTVisitor {
public:
  json::JSONValue evaluate(const ASTNode &node, const json::JSONValue &input);

  json::JSONValue visitIdentity(const IdentityNode &node) override;
  json::JSONValue visitField(const FieldNode &node) override;
  json::JSONValue visitArrayIndex(const ArrayIndexNode &node) override;
  json::JSONValue visitArraySlice(const ArraySliceNode &node) override;
  json::JSONValue visitObjectAccess(const ObjectAccessNode &node) override;
  json::JSONValue visitObjectIterator(const ObjectIteratorNode &node) override;
  json::JSONValue visitAddition(const AdditionNode &node) override;
  json::JSONValue visitSubtraction(const SubtractionNode &node) override;
  json::JSONValue visitLength(const LengthNode &node) override;
  json::JSONValue visitKeys(const KeysNode &node) override;
  json::JSONValue visitPipe(const PipeNode &node) override;
  json::JSONValue visitLiteral(const LiteralNode &node) override;
  json::JSONValue visitNumberLiteral(const NumberLiteralNode &node) override;

private:
  std::stack<const json::JSONValue *> contextStack;

  const json::JSONValue &currentContext() const { return *contextStack.top(); }

  void pushContext(const json::JSONValue &context) {
    contextStack.push(&context);
  }

  void popContext() { contextStack.pop(); }
};

} // namespace jqcpp
