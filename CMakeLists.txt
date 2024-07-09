cmake_minimum_required(VERSION 3.14.0)
project(jqcpp VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER "clang++")

include(Sanitizers)
find_package(Catch2  REQUIRED)

add_library(json_parser STATIC
    src/json_tokenizer.cpp
    src/json_parser.cpp
)
target_include_directories(json_parser PUBLIC include)

# JSON Parser test
add_executable(test_json_parser tests/test_json_parser.cpp)
target_link_libraries(test_json_parser PRIVATE json_parser Catch2::Catch2)

# Enable testing
enable_testing()
add_test(NAME json_parser_test COMMAND test_json_parser)

# Add a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_json_parser
)

# Installation rules
install(TARGETS json_parser
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/jqcpp DESTINATION include)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
