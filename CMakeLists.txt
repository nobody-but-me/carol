
cmake_minimum_required(VERSION 3.21)
project(carol C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(carol)

target_sources(carol PRIVATE ${CMAKE_SOURCE_DIR}/main.c)

# target_compile_options(carol PRIVATE -Wall -Werror -pedantic -Wno-unused-function)
target_compile_options(carol PRIVATE -Wall -Werror -pedantic -Wno-unused-function -Wno-format)
