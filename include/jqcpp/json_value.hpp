#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace jqcpp::json {
struct JSONValue;
using JSONArray = std::vector<JSONValue>;
// use vector instead of map for maintain the insertion order
// however, need be careful when insert new pair
// for the exist keys just update the values
using JSONObject = std::vector<std::pair<std::string, JSONValue>>;

inline void jsonObjectInsert(JSONObject &obj, const std::string &key,
                             JSONValue v);

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
  // object
  bool is_object() const {
    return std::holds_alternative<std::unique_ptr<JSONObject>>(value);
  }

  bool is_array() const {
    return std::holds_alternative<std::unique_ptr<JSONArray>>(value);
  }

  // array
  // getters
  bool get_bool() const { return std::get<bool>(value); }
  double get_number() const { return std::get<double>(value); }
  const std::string &get_string() const { return std::get<std::string>(value); }
  const JSONObject &get_object() const {
    if (!is_object()) {
      throw std::runtime_error("Not a JSONObject");
    }
    return *std::get<std::unique_ptr<JSONObject>>(value);
  }

  const JSONArray &get_array() const {
    if (!is_array()) {
      throw std::runtime_error("Not a JSON Array");
    }
    return *std::get<std::unique_ptr<JSONArray>>(value);
  }

  JSONValue deepCopy() const {
    if (is_string()) {
      return JSONValue(get_string());
    } else if (is_number()) {
      return JSONValue(get_number());
    } else if (is_bool()) {
      return JSONValue(get_bool());
    } else if (is_null()) {
      return JSONValue();
    } else if (is_array()) {
      JSONArray new_array;
      for (const auto &elem : get_array()) {
        new_array.push_back(elem.deepCopy());
      }
      return JSONValue(std::move(new_array));
    } else if (is_object()) {
      JSONObject new_object;
      for (const auto &[key, value] : get_object()) {
        jsonObjectInsert(new_object, key, value.deepCopy());
      }
      return JSONValue(std::move(new_object));
    }
    throw std::runtime_error("Unknown JSONValue type in deepCopy");
  }

  // override operations
  const JSONValue &operator[](std::size_t index) const;
  const JSONValue &operator[](const std::string &index) const;
};

inline void jsonObjectInsert(JSONObject &obj, const std::string &key,
                             JSONValue v) {
  auto it = std::find_if(obj.begin(), obj.end(), [&key](const auto &pair) {
    return pair.first == key;
  });
  if (it != obj.end()) {
    // found, update the value
    it->second = std::move(v);
  } else {
    obj.emplace_back(key, std::move(v));
  }
}

// Addition Operator
inline JSONValue operator+(const JSONValue &lhs, const JSONValue &rhs) {
  if (lhs.is_number() && rhs.is_number()) {
    return JSONValue(lhs.get_number() + rhs.get_number());
  }
  if (lhs.is_string() && rhs.is_string()) {
    return JSONValue(lhs.get_string() + rhs.get_string());
  }
  throw std::runtime_error("Invalid types for addition");
}

// Subtraction Operator
inline JSONValue operator-(const JSONValue &lhs, const JSONValue &rhs) {
  if (lhs.is_number() && rhs.is_number()) {
    return JSONValue(lhs.get_number() - rhs.get_number());
  }
  if (lhs.is_string() && rhs.is_string()) {
    const std::string &lhs_str = lhs.get_string();
    const std::string &rhs_str = rhs.get_string();
    size_t pos = lhs_str.find(rhs_str);
    if (pos != std::string::npos) {
      std::string result = lhs_str;
      result.erase(pos, rhs_str.length());
      return JSONValue(result);
    }
    return JSONValue(lhs_str); // return lhs if rhs is not found
  }
  throw std::runtime_error("Invalid types for subtraction");
}

// Length Function
inline std::size_t length(const JSONValue &json) {
  if (json.is_array()) {
    return json.get_array().size();
  }
  if (json.is_object()) {
    return json.get_object().size();
  }
  if (json.is_string()) {
    return json.get_string().length();
  }
  throw std::runtime_error("Invalid type for length");
}

// Keys Function
inline JSONArray keys(const JSONValue &json) {
  JSONArray result;
  if (json.is_object()) {
    for (const auto &pair : json.get_object()) {
      result.push_back(JSONValue(pair.first));
    }
    return result;
  }
  if (json.is_array()) {
    for (std::size_t i = 0; i < json.get_array().size(); ++i) {
      result.push_back(JSONValue(static_cast<double>(i)));
    }
    return result;
  }
  throw std::runtime_error("Invalid type for keys");
}

inline const JSONValue &JSONValue::operator[](std::size_t index) const {
  if (!is_array()) {
    throw std::runtime_error("Not an array");
  }
  const auto &array = get_array();
  if (index >= array.size()) {
    throw std::runtime_error("Array index out of bounds");
  }
  return array[index];
}

// Overload operator[] for object access
inline const JSONValue &JSONValue::operator[](const std::string &key) const {
  if (!is_object()) {
    throw std::runtime_error("Not an object");
  }
  const auto &object = get_object();
  auto it =
      std::find_if(object.begin(), object.end(),
                   [&key](const auto &pair) { return pair.first == key; });
  if (it == object.end()) {
    throw std::runtime_error("Object key not found");
  }
  return it->second;
}

} // namespace jqcpp::json
