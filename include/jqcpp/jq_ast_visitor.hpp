// jq_ast_visitor.hpp
#pragma once
#include "json_value.hpp"

namespace jqcpp {

class ASTNode;

class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;
  virtual json::JSONValue visitIdentity(const ASTNode &node) = 0;
  virtual json::JSONValue visitField(const ASTNode &node) = 0;
  virtual json::JSONValue visitArrayIndex(const ASTNode &node) = 0;
  virtual json::JSONValue visitArraySlice(const ASTNode &node) = 0;
  virtual json::JSONValue visitObjectAccess(const ASTNode &node) = 0;
  virtual json::JSONValue visitObjectIterator(const ASTNode &node) = 0;
  virtual json::JSONValue visitAddition(const ASTNode &node) = 0;
  virtual json::JSONValue visitSubtraction(const ASTNode &node) = 0;
  virtual json::JSONValue visitLength(const ASTNode &node) = 0;
  virtual json::JSONValue visitKeys(const ASTNode &node) = 0;
  virtual json::JSONValue visitPipe(const ASTNode &node) = 0;
  virtual json::JSONValue visitLiteral(const ASTNode &node) = 0;
};

} // namespace jqcpp
