
#pragma once
#include "jq_ast_node.hpp"
#include "jq_lex.hpp"
#include "json_value.hpp"
#include <memory>
#include <vector>

namespace jqcpp::expr {

class JQParser {
public:
  explicit JQParser(const std::vector<Token> &tokens)
      : tokens(tokens), pos(0), ast(nullptr) {}

  explicit JQParser(const std::string &filter_string) : pos(0), ast(nullptr) {
    jqcpp::expr::ExpressionTokenizer tokenizer;
    tokens = tokenizer.tokenize(filter_string);
  }

  void parse() { ast = parseFilter(); }

  json::JSONValue evaluate(const json::JSONValue &json);

private:
  std::vector<Token> tokens;
  size_t pos;
  std::unique_ptr<ASTNode> ast;

  std::unique_ptr<ASTNode> parseFilter();
  std::unique_ptr<ASTNode> parseTerm();
  std::unique_ptr<ASTNode> parseExpression();
  std::unique_ptr<ASTNode> parseArrayIndex();
  std::unique_ptr<ASTNode> parseArraySlice();
  std::unique_ptr<ASTNode> parseIdentifier();
  std::unique_ptr<ASTNode> parseLiteral();
  std::unique_ptr<ASTNode> parsePipe();

  std::unique_ptr<ASTNode> parseField();
  std::unique_ptr<ASTNode> parseArrayIndexOrSliceOrObjectAccessOrIterator();

  json::JSONValue evaluateNode(const ASTNode *node,
                               const json::JSONValue &json);
};

} // namespace jqcpp::expr
