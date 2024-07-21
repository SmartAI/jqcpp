
#include "jqcpp/jq_parser.hpp"
#include "jqcpp/json_value.hpp"
#include <stdexcept>

namespace jqcpp::expr {

// Forward declarations
json::JSONValue applyAddition(const json::JSONValue &lhs,
                              const json::JSONValue &rhs);
json::JSONValue applySubtraction(const json::JSONValue &lhs,
                                 const json::JSONValue &rhs);

std::unique_ptr<ASTNode> JQParser::parseFilter() {
  auto node = parseTerm();
  while (pos < tokens.size() && tokens[pos].type == TokenType::Pipe) {
    pos++; // Skip '|'
    auto rightNode = parseTerm();
    node = std::make_unique<ASTNode>(ASTNodeType::PIPE, std::move(node),
                                     std::move(rightNode));
  }
  return node;
}

std::unique_ptr<ASTNode> JQParser::parseTerm() {
  // This function should be implemented to handle different terms in your
  // grammar. For simplicity, assume it just parses an identifier or literal.
  if (tokens[pos].type == TokenType::Identifier) {
    return parseIdentifier();
  }
  if (tokens[pos].type == TokenType::Number) {
    return parseLiteral();
  }
  if (tokens[pos].type == TokenType::LeftBracket) {
    return parseArrayIndex();
  }
  if (tokens[pos].type == TokenType::LeftParen) {
    return parseArraySlice();
  }
  throw std::runtime_error("Unexpected token");
}

std::unique_ptr<ASTNode> JQParser::parseArrayIndex() {
  pos++; // Skip '['
  auto indexNode = parseTerm();
  if (tokens[pos].type != TokenType::RightBracket) {
    throw std::runtime_error("Expected ']'");
  }
  pos++; // Skip ']'
  return std::make_unique<ASTNode>(ASTNodeType::ARRAY_INDEX,
                                   std::move(indexNode), nullptr);
}

std::unique_ptr<ASTNode> JQParser::parseArraySlice() {
  pos++; // Skip '['
  auto startNode = parseTerm();
  std::unique_ptr<ASTNode> endNode;
  if (tokens[pos].type == TokenType::Colon) {
    pos++; // Skip ':'
    endNode = parseTerm();
  }
  if (tokens[pos].type != TokenType::RightBracket) {
    throw std::runtime_error("Expected ']'");
  }
  pos++; // Skip ']'
  return std::make_unique<ASTNode>(ASTNodeType::ARRAY_SLICE,
                                   std::move(startNode), std::move(endNode));
}

std::unique_ptr<ASTNode> JQParser::parseIdentifier() {
  std::string identifier = tokens[pos].value;
  pos++;
  return std::make_unique<ASTNode>(ASTNodeType::IDENTIFIER, identifier);
}

std::unique_ptr<ASTNode> JQParser::parseLiteral() {
  double number_value = std::stod(tokens[pos].value);
  pos++;
  return std::make_unique<ASTNode>(ASTNodeType::LITERAL, number_value);
}

json::JSONValue JQParser::evaluate(const json::JSONValue &json) {
  if (!ast) {
    throw std::runtime_error("AST is not initialized. Call parse() first.");
  }
  return evaluateNode(ast.get(), json);
}

json::JSONValue JQParser::evaluateNode(const ASTNode *node,
                                       const json::JSONValue &json) {
  if (!node) {
    throw std::runtime_error("Null AST node");
  }

  switch (node->type) {
  case ASTNodeType::IDENTIFIER:
    return json[node->identifier].deepCopy();
  case ASTNodeType::ARRAY_INDEX: {
    const auto &array = json.get_array();
    if (node->number_value < 0 ||
        static_cast<size_t>(node->number_value) >= array.size()) {
      throw std::runtime_error("Array index out of bounds");
    }
    return array[static_cast<size_t>(node->number_value)].deepCopy();
  }
  case ASTNodeType::ARRAY_SLICE: {
    const auto &array = json.get_array();
    int start = node->left ? static_cast<int>(node->left->number_value) : 0;
    int end = node->right ? static_cast<int>(node->right->number_value)
                          : array.size();
    start = std::min(start, static_cast<int>(array.size()));
    end = std::min(end, static_cast<int>(array.size()));
    // json::JSONArray slice(array.begin() + start, array.begin() + end);
    std::vector<json::JSONValue> slice;
    for (int i = 0; i < end - start; ++i) {
      slice.push_back(array[start + i].deepCopy());
    }
    return json::JSONValue(std::move(slice));
  }
  case ASTNodeType::OBJECT_ACCESS:
    return json[node->identifier].deepCopy();
  case ASTNodeType::OBJECT_ITERATOR: {
    const auto &array = json.get_array();
    json::JSONArray result;
    for (const auto &item : array) {
      result.push_back(item.deepCopy());
    }
    return json::JSONValue(std::move(result));
  }
  case ASTNodeType::ADDITION:
    return applyAddition(evaluateNode(node->left.get(), json),
                         evaluateNode(node->right.get(), json));
  case ASTNodeType::SUBTRACTION:
    return applySubtraction(evaluateNode(node->left.get(), json),
                            evaluateNode(node->right.get(), json));
  case ASTNodeType::LENGTH:
    return json::JSONValue(static_cast<double>(json.get_array().size()));
  case ASTNodeType::KEYS: {
    const auto &obj = json.get_object();
    json::JSONArray keys;
    for (const auto &pair : obj) {
      keys.push_back(json::JSONValue(pair.first));
    }
    return json::JSONValue(std::move(keys));
  }
  case ASTNodeType::LITERAL:
    return json::JSONValue(node->number_value);
  case ASTNodeType::PIPE:
    throw std::runtime_error("PIPE is not supported");
    // return evaluateNode(node->right.get(),
    //                     evaluateNode(node->left.get(), json));
  default:
    throw std::runtime_error("Unknown ASTNode type");
  }
}

json::JSONValue applyAddition(const json::JSONValue &lhs,
                              const json::JSONValue &rhs) {
  if (lhs.is_number() && rhs.is_number()) {
    return json::JSONValue(lhs.get_number() + rhs.get_number());
  }
  throw std::runtime_error("Addition is only supported for numbers");
}

json::JSONValue applySubtraction(const json::JSONValue &lhs,
                                 const json::JSONValue &rhs) {
  if (lhs.is_number() && rhs.is_number()) {
    return json::JSONValue(lhs.get_number() - rhs.get_number());
  }
  throw std::runtime_error("Subtraction is only supported for numbers");
}

} // namespace jqcpp::expr
