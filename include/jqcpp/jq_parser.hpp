// jq_parser.hpp
#pragma once
#include "jq_ast_node.hpp"
#include "jq_lex.hpp"
#include <memory>
#include <vector>

namespace jqcpp {

class JQParser {
public:
  std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens);

private:
  std::vector<Token>::const_iterator current;
  std::vector<Token>::const_iterator end;

  std::unique_ptr<ASTNode> parseExpression();
  std::unique_ptr<ASTNode> parseTerm();
  std::unique_ptr<ASTNode> parseFieldAccess(std::unique_ptr<ASTNode> base);
  std::unique_ptr<ASTNode> parseArrayAccess(std::unique_ptr<ASTNode> base);
  std::unique_ptr<ASTNode> parseSlice(std::unique_ptr<ASTNode> base,
                                      std::unique_ptr<ASTNode> start);
  std::unique_ptr<ASTNode> parseObjectAccess(std::unique_ptr<ASTNode> base);
  std::unique_ptr<ASTNode> parseObjectIterator(std::unique_ptr<ASTNode> base);
  std::unique_ptr<ASTNode> parseAddition();
  std::unique_ptr<ASTNode> parseSubtraction();
  std::unique_ptr<ASTNode> parseLength();
  std::unique_ptr<ASTNode> parseKeys();

  Token peek() const;
  Token advance();
  bool match(TokenType type);
  void consume(TokenType type, const std::string &message);
  bool isAtEnd() const;
};

} // namespace jqcpp
