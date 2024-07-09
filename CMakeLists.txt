cmake_minimum_required(VERSION 3.14.0)
project(jqcpp VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE true)

set(CMAKE_CXX_COMPILER "clang++")

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
# add_executable(test_json_parser tests/test_json_parser.cpp)
# target_link_libraries(test_json_parser PRIVATE json_parser Catch2::Catch2WithMain)

# Enable testing
enable_testing()
add_test(NAME json_tokenizer_test COMMAND test_json_tokenizer)
# add_test(NAME json_parser_test COMMAND test_json_parser)

# Add a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_json_tokenizer 
)

# Installation rules
# install(TARGETS json_parser
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
# )
# install(DIRECTORY include/jqcpp DESTINATION include)
#

