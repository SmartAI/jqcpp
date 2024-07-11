#include "jqcpp/json_parser.hpp"
#include "jqcpp/json_tokenizer.hpp"
#include "jqcpp/json_value.hpp"

namespace jqcpp {
JSONValue JSONParser::parse(const std::vector<Token> &tokens) {
  if (tokens.empty()) {
    throw JSONParserError("Empty tokens");
  }

  it = tokens.begin();
  end = tokens.end();
  return parse_value();
}

JSONValue JSONParser::parse_value() {
  if (it == end) {
    throw JSONParserError("Unexpected end of tokens");
  }

  // get a new token
  switch (it->type) {
  case TokenType::Null:
    consume(TokenType::Null);
    return JSONValue(nullptr);
  case TokenType::True:
    consume(TokenType::True);
    return JSONValue(true);
  case TokenType::False:
    consume(TokenType::False);
    return JSONValue(false);
  case TokenType::Number: {
    double v = std::stod(it->value);
    consume(TokenType::Number);
    return JSONValue(v);
  }
  case TokenType::String: {
    std::string v = it->value;
    consume(TokenType::String);
    return JSONValue(std::move(v));
  }
  case TokenType::LeftBrace:
    return parse_object();
  case TokenType::LeftBracket:
    return parse_array();
  default:
    throw JSONParserError("Unrecognized token type");
  }
}

void JSONParser::consume(TokenType expected_type) {
  if (it == end) {
    throw JSONParserError("Unexpected end of token!");
  }
  if (it->type != expected_type) {
    throw JSONParserError("Unexpected tokey type");
  }
  ++it;
}

JSONValue JSONParser::parse_object() {
  // object is a map which has a key and a value
  // a object must start with "{"
  consume(TokenType::LeftBrace);
  JSONObject object;

  // a right brace is the end of the object
  if (it != end && it->type != TokenType::RightBrace) {
    do {
      // key: value
      // key is a string
      // value can be any other valid type
      // firstly parse the key
      if (it->type != TokenType::String) {
        throw JSONParserError("The key of object should be a string type");
      }
      std::string key = std::move(it->value);
      consume(TokenType::String);
      // should be colon :
      consume(TokenType::Colon);
      // the value
      jsonObjectInsert(object, key, parse_value());
      // the key: value ends when encounter a comma: ,
      // and then another object starts
    } while (it != end && it->type == TokenType::Comma && (++it, true));
  }
  // end of object
  consume(TokenType::RightBrace);
  // just move
  return JSONValue(std::move(object));
}

// parse a array
// e.g. [1, 2, 3, 4]
JSONValue JSONParser::parse_array() {
  // begin with [
  consume(TokenType::LeftBracket);
  // array is a vector of json value
  JSONArray arr;
  // the array ends if ] appears
  if (it != end && it->type != TokenType::RightBracket) {
    do {
      arr.push_back(parse_value());
    } while (it != end && it->type == TokenType::Comma && (++it, true));
  }
  // ends with ]
  consume(TokenType::RightBracket);
  return JSONValue(std::move(arr));
}

} // namespace jqcpp
