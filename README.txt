## Introduction
jqcpp is a standalone JSON processor software similar to jq. It can process JSON data from standard input and support jq-like filtering and manipulation.

## Build and run
Let $TOP_DIR denote the directory containing this README file.  
Let $INSTALL_DIR denote the directory into which this software is to be installed.
To build and install the software, use the commands:
    cd $TOP_DIR
    cmake -H. -Btmp_cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build tmp_cmake --clean-first --target install

To run a demonstration, use the commands:
    $INSTALL_DIR/bin/demo

## Features
- JSON parsing and validation
- jq-like expression evaluation
- Pretty printing of JSON output

Supported operations: 
• Indexing: .field, .[index] 
• Array Slicing: .[start:end] 
• Arithmetic: +, - 
• Functions: length, keys


## Command line Interface

Basic Usage:
jqcpp [options] <expression> [input-file]

Example Commands:
echo '{"name": "John", "age": 30}' | jqcpp '.name'
echo '{"users": [{"name": "Alice", "age": 25}, {"name": "Bob", "age": 30}]}' | jqcpp '.users[0]'

Supported Options:
-h, --help: Display help information
-v, --version: Show version information

Input Methods:
Piping JSON data: echo '{"key": "value"}' | jqcpp 'keys'
Reading from file: jqcpp 'keys'  input.json
Interactive mode: jqcpp 'keys' (then type JSON and press Ctrl+D)

Expression Syntax:
Expressions in jqcpp allow you to filter and transform JSON data. Here are some common expression patterns:

Identity: . 
  Returns the input unchanged
  Example: echo '{"name": "John"}' | jqcpp '.'

Object Identifier: .foo
  Retrieve the value associated with a given field name.
  Example: echo '{"name": "John", "age": 30}' | jqcpp '.name'

Array index: .[<number>] 
  Retrieves an element from an array by its index.
  Example: echo '[10, 20, 30]' | jqcpp '.[1]'

Array Slicing: .[start:end]
  Retrieves a subset of an array.
  Example: echo '[10, 20, 30, 40]' | jqcpp '.[1:3]' 

Arithmetic Operations: + -
  Perform arithmetic on numeric values
  Example: echo '{"x": 10, "y": 5}' | jqcpp '.x + .y'

Built-in Functions: 
  length: Returns the length of a string, array, or object.
  keys: Returns an array of an object's keys.
  Example: echo '{"a": 1, "b": 2}' | jqcpp 'keys' 
