cmake_minimum_required(VERSION 4.2.3)
Project("main.elf")

set(SERVER_EXECUTABLE "server.elf")
include_directories(headers)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -std=c99 -g)
set(SOURCES source/main.c)

add_executable(${SERVER_EXECUTABLE} ${SOURCES})
