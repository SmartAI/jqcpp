#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace jqcpp {
struct JSONValue;
using JSONArray = std::vector<JSONValue>;
using JSONObject = std::map<std::string, JSONValue>;

struct JSONValue {
  std::variant<std::string, double, bool, std::nullptr_t,
               std::unique_ptr<JSONArray>, std::unique_ptr<JSONObject>>
      value;

  // constructors
  JSONValue() : value(nullptr) {}
  JSONValue(std::string v) : value(std::move(v)) {}
  JSONValue(double v) : value(v) {}
  JSONValue(bool v) : value(v) {}
  JSONValue(std::nullptr_t v) : value(nullptr) {}
  JSONValue(JSONArray v) : value(std::make_unique<JSONArray>(std::move(v))) {}
  JSONValue(JSONObject v) : value(std::make_unique<JSONObject>(std::move(v))) {}

  // copy
  // TODO:
  JSONValue(const JSONValue &other) : value(nullptr) {}
  JSONValue &operator=(const JSONValue &other) {
    if (this != &other) {
      // TODO: do something
    }

    return *this;
  }

  // move
  JSONValue(JSONValue &&other) = default;
  JSONValue &operator=(JSONValue &&other) = default;
};

} // namespace jqcpp
