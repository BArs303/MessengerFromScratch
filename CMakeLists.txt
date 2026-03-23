cmake_minimum_required(VERSION 4.2.3)
Project("main.elf")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SERVER_EXECUTABLE "server.elf")
include_directories(headers)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -std=c99 -g)
set(SOURCES 
	source/server.c
	source/dynamic_array.c
)

add_executable(${SERVER_EXECUTABLE} ${SOURCES})
