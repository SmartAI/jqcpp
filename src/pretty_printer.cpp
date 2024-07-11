/**
 * @file
 * @brief
 */

#include "jqcpp/pretty_printer.hpp"
#include "jqcpp/json_value.hpp"
#include <iomanip>
#include <sstream>

namespace jqcpp {

/**
 * @brief pretty print a json object
 *
 * @param value
 * @param indent
 * @return
 */
std::string JSONPrinter::print(const JSONValue &value, int indent) {
  std::string out;
  if (value.is_null()) {
    return "null";
  } else if (value.is_bool()) {
    return value.get_bool() ? "true" : "false";
  } else if (value.is_number()) {
    std::ostringstream oss;
    oss << std::setprecision(15) << value.get_number();
    return oss.str();
  } else if (value.is_string()) {
    return "\"" + value.get_string() + "\"";
  } else if (value.is_array()) {
    return print_array(value.get_array(), indent);
  } else if (value.is_object()) {
    return print_object(value.get_object(), indent);
  }
  return "";
}

/**
 * @brief get a indent string, the default indent space for each
 * level is 2
 *
 * @param indent indent level
 * @return
 */
std::string JSONPrinter::indent_string(int indent) {
  return std::string(indent * 2, ' ');
}

/**
 * @brief pretty print out a object
 *
 * @param obj
 * @param indent
 * @return a string
 * {
 *    "a": 1,
 *    "b": 2
 * }
 */
std::string JSONPrinter::print_object(const JSONObject &obj, int indent) {
  if (obj.empty()) {
    return "{}";
  }
  std::ostringstream oss;
  // print {
  oss << "{\n";
  // output each key:value
  bool first = true;
  for (const auto &[key, value] : obj) {
    //"key": value,
    // if not the first key:value
    // need to output a newline
    if (!first) {
      oss << ",\n";
    }
    first = false;
    oss << indent_string(indent + 1) << "\"" << key
        << "\": " << print(value, indent + 1);
  }
  // after output all the key:values
  // output the }
  oss << "\n" << indent_string(indent) << "}";

  return oss.str();
}

std::string JSONPrinter::print_array(const JSONArray &arr, int indent) {
  if (arr.empty()) {
    return "[]";
  }

  std::ostringstream oss;
  oss << "[\n";
  bool first = true;
  for (const auto &value : arr) {
    if (!first) {
      oss << ",\n";
    }
    first = false;
    oss << indent_string(indent + 1) << print(value, indent + 1);
  }
  oss << "\n" << indent_string(indent) << "]";
  return oss.str();
}

} // namespace jqcpp
