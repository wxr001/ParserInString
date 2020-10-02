cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ParserInString)

add_executable(test test/test.cpp test/test_lexer.hpp)
target_include_directories(test PRIVATE include)
target_include_directories(test PRIVATE lib/TAVL)
target_include_directories(test PRIVATE lib/static_print)
target_include_directories(test PRIVATE lib/doctest)
