#pragma once

#include "jqcpp/expression_tokenizer.hpp"
#include "jqcpp/json_value.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace jqcpp::expr {

class Expression;

class ExpressionVisitor {
public:
  virtual ~ExpressionVisitor() = default;
  virtual void visit(const class Identity &) = 0;
  virtual void visit(const class Identifier &) = 0;
  virtual void visit(const class UnaryOp &) = 0;
  virtual void visit(const class ObjectAccess &) = 0;
  virtual void visit(const class ArrayAccess &) = 0;
  virtual void visit(const class ArraySlice &) = 0;
  virtual void visit(const class BinaryOp &) = 0;
  virtual void visit(const class Literal &) = 0;
  virtual void visit(const class ObjectConstruction &) = 0;
  virtual void visit(const class ArrayConstruction &) = 0;
};

class Expression {
public:
  virtual ~Expression() = default;
  virtual json::JSONValue evaluate(const json::JSONValue &input) const = 0;
  virtual void accept(ExpressionVisitor &visitor) const = 0;
  virtual std::string toString() const = 0;
};

class UnaryOp : public Expression {
  std::unique_ptr<Expression> operand;
  TokenType op;

public:
  UnaryOp(std::unique_ptr<Expression> expr, TokenType o)
      : operand(std::move(expr)), op(o) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    auto value = operand->evaluate(input);
    if (op == TokenType::Minus) {
      if (value.is_number()) {
        return json::JSONValue(-value.get_number());
      }
      throw std::runtime_error("Cannot apply unary minus to non-number value");
    }
    throw std::runtime_error("Unsupported unary operation");
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    return "(" + Token(op).value + operand->toString() + ")";
  }
};

class Identifier : public Expression {
  std::string name;

public:
  Identifier(std::string n) : name(std::move(n)) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    if (name == ".") {
      return input.deepCopy();
    }
    throw std::runtime_error("Unhandled identifier: " + name);
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override { return "Identifier(" + name + ")"; }
};

class Identity : public Expression {
public:
  json::JSONValue evaluate(const json::JSONValue &input) const override {
    return input.deepCopy();
  }
  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }
  std::string toString() const override { return "."; }
};

class ObjectAccess : public Expression {
  std::unique_ptr<Expression> object;
  std::string key;

public:
  ObjectAccess(std::unique_ptr<Expression> obj, std::string k)
      : object(std::move(obj)), key(std::move(k)) {}
  json::JSONValue evaluate(const json::JSONValue &input) const override {
    const auto &obj = object->evaluate(input);
    if (!obj.is_object()) {
      throw std::runtime_error("Cannot access key of non-object value");
    }
    const auto &obj_value = obj.get_object();
    auto it =
        std::find_if(obj_value.begin(), obj_value.end(),
                     [this](const auto &pair) { return pair.first == key; });
    if (it == obj_value.end()) {
      throw std::runtime_error("Key not found: " + key);
    }
    return it->second.deepCopy();
  }
  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }
  std::string toString() const override {
    return object->toString() + "." + key;
  }
};

class ArrayAccess : public Expression {
  std::unique_ptr<Expression> array;
  std::unique_ptr<Expression> index;

public:
  ArrayAccess(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx)
      : array(std::move(arr)), index(std::move(idx)) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    const auto &arr = array->evaluate(input);
    if (!arr.is_array()) {
      throw std::runtime_error("Cannot access index of non-array value");
    }
    const auto &arr_value = arr.get_array();
    auto idx = static_cast<size_t>(index->evaluate(input).get_number());
    if (idx >= arr_value.size()) {
      throw std::runtime_error("Array index out of bounds");
    }
    return arr_value[idx].deepCopy();
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    return array->toString() + "[" + index->toString() + "]";
  }
};

class ArraySlice : public Expression {
  std::unique_ptr<Expression> array;
  std::unique_ptr<Expression> start;
  std::unique_ptr<Expression> end;

public:
  ArraySlice(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> s,
             std::unique_ptr<Expression> e)
      : array(std::move(arr)), start(std::move(s)), end(std::move(e)) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    const auto &arr = array->evaluate(input);
    if (!arr.is_array()) {
      throw std::runtime_error("Cannot slice non-array value");
    }
    const auto &arr_value = arr.get_array();
    size_t s =
        start ? static_cast<size_t>(start->evaluate(input).get_number()) : 0;
    size_t e = end ? static_cast<size_t>(end->evaluate(input).get_number())
                   : arr_value.size();
    s = std::min(s, arr_value.size());
    e = std::min(e, arr_value.size());

    std::vector<json::JSONValue> result;
    result.reserve(e - s);
    for (std::size_t i = 0; i < e - s; ++i) {
      result.push_back(arr_value[s + i].deepCopy());
    }
    return json::JSONValue(std::move(result));
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    return array->toString() + "[" + (start ? start->toString() : "") + ":" +
           (end ? end->toString() : "") + "]";
  }
};

class BinaryOp : public Expression {
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  TokenType op;

public:
  BinaryOp(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r,
           TokenType o)
      : left(std::move(l)), right(std::move(r)), op(o) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    auto l = left->evaluate(input);
    auto r = right->evaluate(input);

    switch (op) {
    case TokenType::Plus:
      if (l.is_number() && r.is_number()) {
        return json::JSONValue(l.get_number() + r.get_number());
      }
      // Handle string concatenation or other types if needed
      break;
    case TokenType::Minus:
      if (l.is_number() && r.is_number()) {
        return json::JSONValue(l.get_number() - r.get_number());
      }
      break;
    case TokenType::Multiply:
      if (l.is_number() && r.is_number()) {
        return json::JSONValue(l.get_number() * r.get_number());
      }
      break;
    case TokenType::Divide:
      if (l.is_number() && r.is_number()) {
        if (r.get_number() == 0)
          throw std::runtime_error("Division by zero");
        return json::JSONValue(l.get_number() / r.get_number());
      }
      break;
    default:
      throw std::runtime_error("Unsupported binary operation");
    }
    throw std::runtime_error("Invalid operand types for binary operation");
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    return "(" + left->toString() + " " + Token(op).value + " " +
           right->toString() + ")";
  }
};

class Literal : public Expression {
  json::JSONValue value;

public:
  Literal(json::JSONValue v) : value(std::move(v)) {}
  json::JSONValue evaluate(const json::JSONValue &) const override {
    return value.deepCopy();
  }
  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }
  std::string toString() const override {
    std::ostringstream oss;
    if (value.is_null()) {
      oss << "null";
    } else if (value.is_bool()) {
      oss << (value.get_bool() ? "true" : "false");
    } else if (value.is_number()) {
      oss << value.get_number();
    } else if (value.is_string()) {
      oss << '"' << value.get_string() << '"';
    } else if (value.is_array()) {
      oss << "[array]";
    } else if (value.is_object()) {
      oss << "{object}";
    } else {
      oss << "unknown";
    }
    return oss.str();
  }
};

class ObjectConstruction : public Expression {
  std::vector<std::pair<std::string, std::unique_ptr<Expression>>> elements;

public:
  ObjectConstruction(
      std::vector<std::pair<std::string, std::unique_ptr<Expression>>> elems)
      : elements(std::move(elems)) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    json::JSONObject result;
    for (const auto &[key, expr] : elements) {
      result.emplace_back(key, expr->evaluate(input));
    }
    return json::JSONValue(std::move(result));
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < elements.size(); ++i) {
      if (i > 0)
        oss << ", ";
      oss << elements[i].first << ": " << elements[i].second->toString();
    }
    oss << "}";
    return oss.str();
  }
};

class ArrayConstruction : public Expression {
  std::vector<std::unique_ptr<Expression>> elements;

public:
  ArrayConstruction(std::vector<std::unique_ptr<Expression>> elems)
      : elements(std::move(elems)) {}

  json::JSONValue evaluate(const json::JSONValue &input) const override {
    std::vector<json::JSONValue> result;
    for (const auto &expr : elements) {
      result.push_back(expr->evaluate(input));
    }
    return json::JSONValue(std::move(result));
  }

  void accept(ExpressionVisitor &visitor) const override {
    visitor.visit(*this);
  }

  std::string toString() const override {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < elements.size(); ++i) {
      if (i > 0)
        oss << ", ";
      oss << elements[i]->toString();
    }
    oss << "]";
    return oss.str();
  }
};

class Parser {
  std::vector<Token>::const_iterator current;
  std::vector<Token>::const_iterator end;

public:
  std::unique_ptr<Expression> parse(const std::vector<Token> &tokens) {
    current = tokens.begin();
    end = tokens.end();
    return parseExpression();
  }

private:
  std::unique_ptr<Expression> parseObjectConstruction() {
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> elements;

    if (!check(TokenType::RightBrace)) {
      do {
        std::string key;
        if (match(TokenType::String)) {
          key = std::prev(current)->value;
        } else if (match(TokenType::Identifier)) {
          key = std::prev(current)->value;
        } else {
          throw std::runtime_error(
              "Expected string or identifier as object key");
        }

        consume(TokenType::Colon, "Expect ':' after object key");
        auto value = parseExpression();
        elements.emplace_back(std::move(key), std::move(value));
      } while (match(TokenType::Comma));
    }

    consume(TokenType::RightBrace, "Expect '}' after object construction");
    return std::make_unique<ObjectConstruction>(std::move(elements));
  }

  std::unique_ptr<Expression> parseArrayConstruction() {
    std::vector<std::unique_ptr<Expression>> elements;

    if (!check(TokenType::RightBracket)) {
      do {
        elements.push_back(parseExpression());
      } while (match(TokenType::Comma));
    }

    consume(TokenType::RightBracket, "Expect ']' after array construction");
    return std::make_unique<ArrayConstruction>(std::move(elements));
  }

  std::unique_ptr<Expression> parseExpression() {
    auto expr = parseTerm();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
      TokenType op = std::prev(current)->type;
      auto right = parseTerm();
      expr = std::make_unique<BinaryOp>(std::move(expr), std::move(right), op);
    }
    return expr;
  }

  std::unique_ptr<Expression> parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::Multiply) || match(TokenType::Divide)) {
      TokenType op = std::prev(current)->type;
      auto right = parseFactor();
      expr = std::make_unique<BinaryOp>(std::move(expr), std::move(right), op);
    }
    return expr;
  }

  std::unique_ptr<Expression> parseFactor() {
    if (match(TokenType::Minus)) {
      auto expr = parseFactor();
      return std::make_unique<UnaryOp>(std::move(expr), TokenType::Minus);
    }
    return parsePrimary();
  }

  std::unique_ptr<Expression> parsePrimary() {
    std::unique_ptr<Expression> expr;

    if (match(TokenType::LeftParen)) {
      expr = parseExpression();
      consume(TokenType::RightParen, "Expect ')' after expression.");
    } else if (match(TokenType::LeftBrace)) {
      expr = parseObjectConstruction();
    } else if (match(TokenType::LeftBracket)) {
      expr = parseArrayConstruction();
    } else if (match(TokenType::Dot)) {
      expr = std::make_unique<Identity>();
      if (match(TokenType::Identifier)) {
        expr = std::make_unique<ObjectAccess>(std::move(expr),
                                              std::prev(current)->value);
      }
    } else if (match(TokenType::Number)) {
      expr = std::make_unique<Literal>(
          json::JSONValue(std::stod(std::prev(current)->value)));
    } else if (match(TokenType::String)) {
      expr =
          std::make_unique<Literal>(json::JSONValue(std::prev(current)->value));
    } else if (match(TokenType::Identifier)) {
      expr = std::make_unique<Identifier>(std::prev(current)->value);
    } else {
      throw std::runtime_error("Unexpected token in expression");
    }

    while (true) {
      if (match(TokenType::Dot)) {
        if (!match(TokenType::Identifier)) {
          throw std::runtime_error("Expected identifier after '.'");
        }
        expr = std::make_unique<ObjectAccess>(std::move(expr),
                                              std::prev(current)->value);
      } else if (match(TokenType::LeftBracket)) {
        auto index = parseExpression();
        if (match(TokenType::Colon)) {
          auto end =
              check(TokenType::RightBracket) ? nullptr : parseExpression();
          consume(TokenType::RightBracket, "Expect ']' after array slice");
          expr = std::make_unique<ArraySlice>(std::move(expr), std::move(index),
                                              std::move(end));
        } else {
          consume(TokenType::RightBracket, "Expect ']' after array index");
          expr =
              std::make_unique<ArrayAccess>(std::move(expr), std::move(index));
        }
      } else {
        break;
      }
    }

    return expr;
  }

  bool match(TokenType type) {
    if (check(type)) {
      advance();
      return true;
    }
    return false;
  }

  bool check(TokenType type) const {
    if (isAtEnd())
      return false;
    return peek().type == type;
  }

  Token advance() {
    if (!isAtEnd())
      return *current++;
    throw std::runtime_error("Unexpected end of input");
  }

  Token peek() const { return *current; }

  bool isAtEnd() const { return current == end; }

  void consume(TokenType type, const std::string &message) {
    if (check(type)) {
      advance();
      return;
    }
    throw std::runtime_error(message);
  }
};

class PrettyPrinter : public ExpressionVisitor {
  std::ostringstream oss;
  int indent = 0;

  void printIndent() { oss << std::string(indent * 2, ' '); }

public:
  void visit(const Identity &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const UnaryOp &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const Identifier &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const ObjectAccess &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const ArrayAccess &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const ArraySlice &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const BinaryOp &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const Literal &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const ObjectConstruction &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  void visit(const ArrayConstruction &expr) override {
    printIndent();
    oss << expr.toString() << "\n";
  }

  std::string getResult() const { return oss.str(); }
};

class JQInterpreter {
  std::unique_ptr<Expression> ast;

public:
  JQInterpreter(const std::string &jq_expression) {
    ExpressionTokenizer tokenizer;
    Parser parser;
    auto tokens = tokenizer.tokenize(jq_expression);
    ast = parser.parse(tokens);
  }

  json::JSONValue execute(const json::JSONValue &input) const {
    return ast->evaluate(input);
  }

  std::string prettyPrint() const {
    PrettyPrinter printer;
    ast->accept(printer);
    return printer.getResult();
  }
};

} // namespace jqcpp::expr
