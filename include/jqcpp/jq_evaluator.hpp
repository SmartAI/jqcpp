// jq_evaluator.hpp
#pragma once
#include "jq_ast_visitor.hpp"
#include "json_value.hpp"

namespace jqcpp {

class JQEvaluator : public ASTVisitor {
public:
  json::JSONValue evaluate(const ASTNode &node, const json::JSONValue &input);

  json::JSONValue visitIdentity(const ASTNode &node) override;
  json::JSONValue visitField(const ASTNode &node) override;
  json::JSONValue visitArrayIndex(const ASTNode &node) override;
  json::JSONValue visitArraySlice(const ASTNode &node) override;
  json::JSONValue visitObjectAccess(const ASTNode &node) override;
  json::JSONValue visitObjectIterator(const ASTNode &node) override;
  json::JSONValue visitAddition(const ASTNode &node) override;
  json::JSONValue visitSubtraction(const ASTNode &node) override;
  json::JSONValue visitLength(const ASTNode &node) override;
  json::JSONValue visitKeys(const ASTNode &node) override;
  json::JSONValue visitPipe(const ASTNode &node) override;
  json::JSONValue visitLiteral(const ASTNode &node) override;

private:
  const json::JSONValue *currentInput;
};

} // namespace jqcpp
