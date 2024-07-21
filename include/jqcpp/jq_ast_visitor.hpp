// jq_ast_visitor.hpp
#pragma once
#include "json_value.hpp"

namespace jqcpp {

class ASTNode;
class IdentityNode;
class FieldNode;
class ArrayIndexNode;
class ArraySliceNode;
class ObjectAccessNode;
class ObjectIteratorNode;
class AdditionNode;
class SubtractionNode;
class LengthNode;
class KeysNode;
class PipeNode;
class LiteralNode;

class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;
  virtual json::JSONValue visitIdentity(const IdentityNode &node) = 0;
  virtual json::JSONValue visitField(const FieldNode &node) = 0;
  virtual json::JSONValue visitArrayIndex(const ArrayIndexNode &node) = 0;
  virtual json::JSONValue visitArraySlice(const ArraySliceNode &node) = 0;
  virtual json::JSONValue visitObjectAccess(const ObjectAccessNode &node) = 0;
  virtual json::JSONValue
  visitObjectIterator(const ObjectIteratorNode &node) = 0;
  virtual json::JSONValue visitAddition(const AdditionNode &node) = 0;
  virtual json::JSONValue visitSubtraction(const SubtractionNode &node) = 0;
  virtual json::JSONValue visitLength(const LengthNode &node) = 0;
  virtual json::JSONValue visitKeys(const KeysNode &node) = 0;
  virtual json::JSONValue visitPipe(const PipeNode &node) = 0;
  virtual json::JSONValue visitLiteral(const LiteralNode &node) = 0;
};

} // namespace jqcpp
