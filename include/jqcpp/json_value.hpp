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

  // disable copy
  JSONValue(const JSONValue &other) = delete;
  JSONValue &operator=(const JSONValue &other) = delete;

  // move
  JSONValue(JSONValue &&other) noexcept = default;
  JSONValue &operator=(JSONValue &&) = default;

  // helper functions for type checker
  // these functions are helpful in testing
  bool is_bool() const { return std::holds_alternative<bool>(value); }
  // all the numbers are converted to double
  bool is_number() const { return std::holds_alternative<double>(value); }
  bool is_string() const { return std::holds_alternative<std::string>(value); }
  // null
  bool is_null() const { return std::holds_alternative<std::nullptr_t>(value); }
  // getters
  bool get_bool() const { return std::get<bool>(value); }
  double get_number() const { return std::get<double>(value); }
  const std::string &get_string() const { return std::get<std::string>(value); }
};

} // namespace jqcpp
