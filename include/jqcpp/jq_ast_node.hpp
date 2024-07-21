#pragma once
#include <memory>
#include <string>

namespace jqcpp::expr {

enum class ASTNodeType {
  IDENTIFIER,
  ARRAY_INDEX,
  ARRAY_SLICE,
  OBJECT_ACCESS,
  OBJECT_ITERATOR,
  ADDITION,
  SUBTRACTION,
  LENGTH,
  KEYS,
  LITERAL,
  PIPE
};

class ASTNode {
public:
  ASTNodeType type;

  // For nodes with children
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;

  // For literal values
  std::string identifier;
  double number_value; // For numeric literals
  // Add more fields as needed

  // Constructors for various types of nodes
  ASTNode(ASTNodeType type) : type(type) {}
  ASTNode(ASTNodeType type, std::unique_ptr<ASTNode> left,
          std::unique_ptr<ASTNode> right)
      : type(type), left(std::move(left)), right(std::move(right)) {}

  ASTNode(ASTNodeType type, const std::string &identifier)
      : type(type), identifier(identifier) {}

  ASTNode(ASTNodeType type, double number_value)
      : type(type), number_value(number_value) {}
};

} // namespace jqcpp::expr
