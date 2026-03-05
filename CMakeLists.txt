cmake_minimum_required(VERSION 4.2.3)
Project("main.elf")

include_directories(headers)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -std=c99)
set(SOURCES source/main.c)

add_executable(${PROJECT_NAME} ${SOURCES})
