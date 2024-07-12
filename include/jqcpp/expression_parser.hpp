#pragma once
#include "expression_tokenizer.hpp"
#include "json_value.hpp"
#include <memory>
#include <vector>

using jqcpp::json::JSONValue;

namespace jqcpp::expr {

class Expression {
public:
  virtual ~Expression() = default;
  virtual const JSONValue &evaluate(const JSONValue &input) const = 0;
};

class Literal : public Expression {
  JSONValue value;

public:
  Literal(JSONValue v) : value(std::move(v)) {}
  const JSONValue &evaluate(const JSONValue &input) const override {
    return value;
  }
};

class Identifier : public Expression {
  std::string name;

public:
  Identifier(std::string n) : name(std::move(n)) {}
  const JSONValue &evaluate(const JSONValue &input) const override;
};

class ObjectAccess : public Expression {
  std::unique_ptr<Expression> object;
  std::string key;
  mutable JSONValue result;

public:
  ObjectAccess(std::unique_ptr<Expression> obj, std::string k)
      : object(std::move(obj)), key(std::move(k)) {}
  const JSONValue &evaluate(const JSONValue &input) const override;
};

class ArrayAccess : public Expression {
  std::unique_ptr<Expression> array;
  std::unique_ptr<Expression> index;
  mutable JSONValue result;

public:
  ArrayAccess(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx)
      : array(std::move(arr)), index(std::move(idx)) {}
  const JSONValue &evaluate(const JSONValue &input) const override;
};

class ArraySlice : public Expression {
  std::unique_ptr<Expression> array;
  std::unique_ptr<Expression> start;
  std::unique_ptr<Expression> end;
  mutable JSONValue result;

public:
  ArraySlice(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> s,
             std::unique_ptr<Expression> e)
      : array(std::move(arr)), start(std::move(s)), end(std::move(e)) {}
  const JSONValue &evaluate(const JSONValue &input) const override;
};

class BinaryOp : public Expression {
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  TokenType op;
  mutable JSONValue result;

public:
  BinaryOp(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r,
           TokenType o)
      : left(std::move(l)), right(std::move(r)), op(o) {}
  const JSONValue &evaluate(const JSONValue &input) const override;
};

class Parser {
public:
  std::unique_ptr<Expression> parse(const std::vector<Token> &tokens);

private:
  std::vector<Token>::const_iterator current;
  std::vector<Token>::const_iterator end;

  std::unique_ptr<Expression> parseExpression();
  std::unique_ptr<Expression> parseTerm();
  std::unique_ptr<Expression> parseFactor();
  std::unique_ptr<Expression> parsePrimary();

  bool match(TokenType type);
  bool check(TokenType type) const;
  Token advance();
  Token peek() const;
  bool isAtEnd() const;
};

} // namespace jqcpp::expr
