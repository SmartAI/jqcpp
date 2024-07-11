cmake_minimum_required(VERSION 3.14.0)
project(jqcpp VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE true)

include(${CMAKE_SOURCE_DIR}/Sanitizers.cmake)

find_package(Catch2  REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

# add_library(json_parser STATIC
#     src/json_tokenizer.cpp
#     src/json_parser.cpp
# )
# target_include_directories(json_parser PUBLIC include)
#

# JSON Tokenizer test
add_executable(test_json_tokenizer tests/test_json_tokenizer.cpp src/json_tokenizer.cpp)
target_link_libraries(test_json_tokenizer PRIVATE Catch2::Catch2WithMain)

# JSON Parser test
add_executable(test_json_parser tests/test_json_parser.cpp src/json_parser.cpp src/json_tokenizer.cpp)
target_link_libraries(test_json_parser PRIVATE Catch2::Catch2WithMain)

# JSON Parser test
add_executable(test_pretty_printer tests/test_pretty_printer.cpp src/json_parser.cpp src/json_tokenizer.cpp src/pretty_printer.cpp)
target_link_libraries(test_pretty_printer PRIVATE Catch2::Catch2WithMain)


#  The app
add_executable(jqcpp src/json_parser.cpp src/json_tokenizer.cpp src/pretty_printer.cpp app/main.cpp)

# Enable testing
enable_testing()
add_test(NAME json_tokenizer_test COMMAND test_json_tokenizer)
add_test(NAME json_parser_test COMMAND test_json_parser)
add_test(NAME json_pretty_printer COMMAND test_pretty_printer)

# Add a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_json_tokenizer 
            test_json_parser 
            test_pretty_printer
)

# Installation rules
# install(TARGETS json_parser
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
# )
# install(DIRECTORY include/jqcpp DESTINATION include)
#

