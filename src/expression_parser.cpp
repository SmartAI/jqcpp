#include "jqcpp/expression_parser.hpp"
#include "jqcpp/json_value.hpp"
#include <stdexcept>

using jqcpp::json::JSONArray;

namespace jqcpp::expr {

const JSONValue &Identifier::evaluate(const JSONValue &input) const {
  if (name == ".") {
    return input;
  }
  throw std::runtime_error("Identifier evaluation not implemented");
}

const JSONValue &ObjectAccess::evaluate(const JSONValue &input) const {
  const JSONValue &obj = object->evaluate(input);
  if (!obj.is_object()) {
    throw std::runtime_error("Attempted to access a non-object value");
  }
  const auto &obj_value = obj.get_object();
  auto it =
      std::find_if(obj_value.begin(), obj_value.end(),
                   [this](const auto &pair) { return pair.first == key; });
  if (it == obj_value.end()) {
    throw std::runtime_error("Key not found in object");
  }
  return it->second;
}

const JSONValue &ArrayAccess::evaluate(const JSONValue &input) const {
  const JSONValue &arr = array->evaluate(input);
  if (!arr.is_array()) {
    throw std::runtime_error("Attempted to access a non-array value");
  }
  const JSONValue &idx = index->evaluate(input);
  if (!idx.is_number()) {
    throw std::runtime_error("Array index must be a number");
  }
  int index = static_cast<int>(idx.get_number());
  const auto &arr_value = arr.get_array();
  if (index < 0 || index >= static_cast<int>(arr_value.size())) {
    throw std::runtime_error("Array index out of bounds");
  }
  return arr_value[index];
}

const JSONValue &ArraySlice::evaluate(const JSONValue &input) const {
  const JSONValue &arr = array->evaluate(input);
  if (!arr.is_array()) {
    throw std::runtime_error("Attempted to slice a non-array value");
  }
  const auto &arr_value = arr.get_array();
  int start_idx =
      start ? static_cast<int>(start->evaluate(input).get_number()) : 0;
  int end_idx = end ? static_cast<int>(end->evaluate(input).get_number())
                    : static_cast<int>(arr_value.size());

  start_idx = std::max(0, start_idx);
  end_idx = std::min(static_cast<int>(arr_value.size()), end_idx);

  JSONArray slice_result;
  // FIXME:
  // slice_result.reserve(end_idx - start_idx);
  // for (int i = start_idx; i < end_idx; ++i) {
  //   // Use emplace_back to construct the JSONValue in place
  //   slice_result.emplace_back(JSONValue(arr_value[i]));
  // }
  result = JSONValue(std::move(slice_result));
  return result;
}

const JSONValue &BinaryOp::evaluate(const JSONValue &input) const {
  const JSONValue &lhs = left->evaluate(input);
  const JSONValue &rhs = right->evaluate(input);

  switch (op) {
  case TokenType::Plus:
    if (lhs.is_number() && rhs.is_number()) {
      result = JSONValue(lhs.get_number() + rhs.get_number());
      return result;
    }
    throw std::runtime_error("Addition only supported for numbers");
  case TokenType::Minus:
    if (lhs.is_number() && rhs.is_number()) {
      result = JSONValue(lhs.get_number() - rhs.get_number());
      return result;
    }
    throw std::runtime_error("Subtraction only supported for numbers");
  // Implement other operations as needed
  default:
    throw std::runtime_error("Unsupported binary operation");
  }
}

std::unique_ptr<Expression> Parser::parse(const std::vector<Token> &tokens) {
  current = tokens.begin();
  end = tokens.end();
  return parseExpression();
}

std::unique_ptr<Expression> Parser::parseExpression() {
  auto expr = parseTerm();
  while (match(TokenType::Plus) || match(TokenType::Minus)) {
    TokenType op = std::prev(current)->type;
    auto right = parseTerm();
    expr = std::make_unique<BinaryOp>(std::move(expr), std::move(right), op);
  }
  return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
  auto expr = parseFactor();
  while (match(TokenType::Multiply) || match(TokenType::Divide)) {
    TokenType op = std::prev(current)->type;
    auto right = parseFactor();
    expr = std::make_unique<BinaryOp>(std::move(expr), std::move(right), op);
  }
  return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
  if (match(TokenType::Minus)) {
    auto right = parseFactor();
    return std::make_unique<BinaryOp>(std::make_unique<Literal>(JSONValue(0.0)),
                                      std::move(right), TokenType::Minus);
  }
  return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
  if (match(TokenType::Number)) {
    return std::make_unique<Literal>(
        JSONValue(std::stod(std::prev(current)->value)));
  }
  if (match(TokenType::String)) {
    return std::make_unique<Literal>(JSONValue(std::prev(current)->value));
  }
  if (match(TokenType::Identifier)) {
    return std::make_unique<Identifier>(std::prev(current)->value);
  }
  if (match(TokenType::Dot)) {
    if (match(TokenType::Identifier)) {
      return std::make_unique<ObjectAccess>(std::make_unique<Identifier>("."),
                                            std::prev(current)->value);
    }
    return std::make_unique<Identifier>(".");
  }
  if (match(TokenType::LeftBracket)) {
    auto expr = parseExpression();
    if (match(TokenType::Colon)) {
      auto end = parseExpression();
      match(TokenType::RightBracket);
      return std::make_unique<ArraySlice>(std::make_unique<Identifier>("."),
                                          std::move(expr), std::move(end));
    }
    match(TokenType::RightBracket);
    return std::make_unique<ArrayAccess>(std::make_unique<Identifier>("."),
                                         std::move(expr));
  }
  throw std::runtime_error("Unexpected token in expression");
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

Token Parser::advance() {
  if (isAtEnd()) {
    throw std::runtime_error("Cannot advance past the end of tokens.");
  }
  return *current++;
}

Token Parser::peek() const { return *current; }

bool Parser::isAtEnd() const { return current == end; }

} // namespace jqcpp::expr
