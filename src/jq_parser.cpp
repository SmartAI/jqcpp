#include "jqcpp/jq_parser.hpp"
#include "jqcpp/jq_lex.hpp"
#include <stdexcept>

namespace jqcpp {

std::unique_ptr<ASTNode> JQParser::parse(const std::vector<Token> &tokens) {
  current = tokens.begin();
  end = tokens.end();
  return parseExpression();
}

std::unique_ptr<ASTNode> JQParser::parseExpression() {
  auto node = parseTerm();
  while (match(TokenType::Pipe)) {
    auto right = parseTerm();
    node = std::make_unique<PipeNode>(std::move(node), std::move(right));
  }

  while (match(TokenType::Plus) || match(TokenType::Minus)) {
    TokenType op = std::prev(current)->type;
    auto right = parseTerm();
    if (op == TokenType::Plus) {
      node = std::make_unique<AdditionNode>(std::move(node), std::move(right));
    } else {
      node =
          std::make_unique<SubtractionNode>(std::move(node), std::move(right));
    }
  }

  return node;
}

std::unique_ptr<ASTNode> JQParser::parseTerm() {
  if (match(TokenType::Number)) {
    return std::make_unique<NumberLiteralNode>(
        std::stod(std::prev(current)->value));
  } else if (match(TokenType::Dot)) {
    if (isAtEnd() || peek().type == TokenType::Pipe) {
      return std::make_unique<IdentityNode>();
    }
    return parseFieldAccess(std::make_unique<IdentityNode>());
  } else if (match(TokenType::Length)) {
    return parseLength();
  } else if (match(TokenType::Keys)) {
    return parseKeys();
  } else if (match(TokenType::Plus) || match(TokenType::Minus)) {
    return parseAddition();
  }
  throw std::runtime_error("Not supported op");
}

std::unique_ptr<ASTNode>
JQParser::parseFieldAccess(std::unique_ptr<ASTNode> base) {
  if (match(TokenType::Identifier)) {
    auto field = std::make_unique<FieldNode>(std::prev(current)->value);
    auto node =
        std::make_unique<ObjectAccessNode>(std::move(base), std::move(field));

    if (match(TokenType::Dot)) {
      return parseFieldAccess(std::move(node));
    } else if (match(TokenType::LeftBracket)) {
      return parseArrayAccess(std::move(node));
    }

    return node;
  } else if (match(TokenType::LeftBracket)) {
    return parseArrayAccess(std::move(base));
  } else if (isAtEnd() || peek().type == TokenType::Pipe) {
    // This handles the case of a bare '.' (identity)
    return base;
  }

  throw std::runtime_error(
      "Expected identifier, '[', or end of expression after '.'");
}

std::unique_ptr<ASTNode>
JQParser::parseArrayAccess(std::unique_ptr<ASTNode> base) {
  if (match(TokenType::Colon)) {
    // 处理 [:something] 的情况
    return parseSlice(std::move(base), nullptr);
  }

  std::unique_ptr<ASTNode> start;
  if (match(TokenType::Number)) {
    start = std::make_unique<LiteralNode>(std::stod(std::prev(current)->value));
  }

  if (match(TokenType::Colon)) {
    return parseSlice(std::move(base), std::move(start));
  }

  if (match(TokenType::RightBracket)) {
    if (start) {
      // 这是一个简单的数组索引
      auto node =
          std::make_unique<ArrayIndexNode>(std::move(base), std::move(start));
      if (match(TokenType::Dot)) {
        return parseFieldAccess(std::move(node));
      } else if (match(TokenType::LeftBracket)) {
        return parseArrayAccess(std::move(node));
      }
      return node;
    } else {
      // 这是一个对象迭代器
      return std::make_unique<ObjectIteratorNode>(std::move(base));
    }
  }

  throw std::runtime_error("Expected number, ':', or ']' after '['");
}

std::unique_ptr<ASTNode> JQParser::parseSlice(std::unique_ptr<ASTNode> base,
                                              std::unique_ptr<ASTNode> start) {
  std::unique_ptr<ASTNode> end;

  if (!match(TokenType::RightBracket)) {
    if (match(TokenType::Number)) {
      end = std::make_unique<LiteralNode>(std::stod(std::prev(current)->value));
    }
    consume(TokenType::RightBracket, "Expected ']' after array slice");
  }

  auto node = std::make_unique<ArraySliceNode>(
      std::move(base), std::move(start), std::move(end));

  if (match(TokenType::Dot)) {
    return parseFieldAccess(std::move(node));
  } else if (match(TokenType::LeftBracket)) {
    return parseArrayAccess(std::move(node));
  }

  return node;
}

std::unique_ptr<ASTNode> JQParser::parseAddition() {
  auto node = parseSubtraction();
  while (match(TokenType::Plus)) {
    auto right = parseSubtraction();
    node = std::make_unique<AdditionNode>(std::move(node), std::move(right));
  }
  return node;
}

std::unique_ptr<ASTNode> JQParser::parseSubtraction() {
  auto node = parseTerm();
  while (match(TokenType::Minus)) {
    auto right = parseTerm();
    node = std::make_unique<SubtractionNode>(std::move(node), std::move(right));
  }
  return node;
}

std::unique_ptr<ASTNode> JQParser::parseLength() {
  return std::make_unique<LengthNode>();
}

std::unique_ptr<ASTNode> JQParser::parseKeys() {
  return std::make_unique<KeysNode>();
}

Token JQParser::peek() const { return *current; }

Token JQParser::advance() { return *current++; }

bool JQParser::match(TokenType type) {
  if (isAtEnd())
    return false;
  if (peek().type != type)
    return false;
  advance();
  return true;
}

void JQParser::consume(TokenType type, const std::string &message) {
  if (peek().type == type) {
    advance();
    return;
  }
  throw std::runtime_error(message);
}

bool JQParser::isAtEnd() const { return current->type == TokenType::End; }

} // namespace jqcpp
