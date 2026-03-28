cmake_minimum_required(VERSION 3.28.0)
Project("main.elf")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SERVER_EXECUTABLE "server.elf")
set(CLIENT_EXECUTABLE "client.elf")

include_directories(headers)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -std=c99 -g)

set(SERVER_SOURCES 
  source/server.c
  source/dynamic_array.c
  source/message.c
)

set(CLIENT_SOURCES 
  source/client.c
  source/parser.c
)

add_executable(${SERVER_EXECUTABLE} ${SERVER_SOURCES})
add_executable(${CLIENT_EXECUTABLE} ${CLIENT_SOURCES})
