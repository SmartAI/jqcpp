#pragma once
#include "json_value.hpp"

#include <string>

namespace jqcpp::json {

/**
 * @class JSONPrinter
 * @brief pretty print a json object
 *
 */
class JSONPrinter {
public:
  std::string print(const JSONValue &value, int indent = 0);

private:
  std::string indent_string(int indent);
  std::string print_object(const JSONObject &obj, int indent);
  std::string print_array(const JSONArray &arr, int indent);
};

} // namespace jqcpp::json
