// jq_ast_node.hpp
#pragma once
#include "jq_ast_visitor.hpp"
#include <memory>
#include <string>

namespace jqcpp {

enum class ASTNodeType {
  Identity,
  Field,
  ArrayIndex,
  ArraySlice,
  ObjectAccess,
  ObjectIterator,
  Addition,
  Subtraction,
  Length,
  Keys,
  Pipe,
  Literal
};

class ASTNode {
public:
  ASTNodeType type;
  std::string value;
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;

  ASTNode(ASTNodeType t, std::string v = "",
          std::unique_ptr<ASTNode> l = nullptr,
          std::unique_ptr<ASTNode> r = nullptr)
      : type(t), value(std::move(v)), left(std::move(l)), right(std::move(r)) {}

  virtual ~ASTNode() = default;

  virtual json::JSONValue accept(ASTVisitor &visitor) const = 0;
};

class IdentityNode : public ASTNode {
public:
  IdentityNode() : ASTNode(ASTNodeType::Identity) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitIdentity(*this);
  }
};

class FieldNode : public ASTNode {
public:
  FieldNode(std::string fieldName)
      : ASTNode(ASTNodeType::Field, std::move(fieldName)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitField(*this);
  }
};

class ArrayIndexNode : public ASTNode {
public:
  ArrayIndexNode(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> index)
      : ASTNode(ASTNodeType::ArrayIndex, "", std::move(array),
                std::move(index)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitArrayIndex(*this);
  }
};

class ArraySliceNode : public ASTNode {
public:
  ArraySliceNode(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> start,
                 std::unique_ptr<ASTNode> end)
      : ASTNode(ASTNodeType::ArraySlice, "", std::move(array), nullptr) {
    if (start)
      this->left = std::move(start);
    if (end)
      this->right = std::move(end);
  }
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitArraySlice(*this);
  }
};

class ObjectAccessNode : public ASTNode {
public:
  ObjectAccessNode(std::unique_ptr<ASTNode> object,
                   std::unique_ptr<ASTNode> field)
      : ASTNode(ASTNodeType::ObjectAccess, "", std::move(object),
                std::move(field)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitObjectAccess(*this);
  }
};

class ObjectIteratorNode : public ASTNode {
public:
  ObjectIteratorNode(std::unique_ptr<ASTNode> object)
      : ASTNode(ASTNodeType::ObjectIterator, "", std::move(object)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitObjectIterator(*this);
  }
};

class AdditionNode : public ASTNode {
public:
  AdditionNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
      : ASTNode(ASTNodeType::Addition, "", std::move(left), std::move(right)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitAddition(*this);
  }
};

class SubtractionNode : public ASTNode {
public:
  SubtractionNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
      : ASTNode(ASTNodeType::Subtraction, "", std::move(left),
                std::move(right)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitSubtraction(*this);
  }
};

class LengthNode : public ASTNode {
public:
  LengthNode() : ASTNode(ASTNodeType::Length) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitLength(*this);
  }
};

class KeysNode : public ASTNode {
public:
  KeysNode() : ASTNode(ASTNodeType::Keys) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitKeys(*this);
  }
};

class PipeNode : public ASTNode {
public:
  PipeNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
      : ASTNode(ASTNodeType::Pipe, "", std::move(left), std::move(right)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitPipe(*this);
  }
};

class LiteralNode : public ASTNode {
public:
  LiteralNode(double value)
      : ASTNode(ASTNodeType::Literal, std::to_string(value)) {}
  json::JSONValue accept(ASTVisitor &visitor) const override {
    return visitor.visitLiteral(*this);
  }
};

} // namespace jqcpp
