cmake_minimum_required(VERSION 4.2.3)
Project("main.elf")

add_compile_options(-Wall -std=c99)
set(SOURCES source/main.c)
add_executable(${PROJECT_NAME} ${SOURCES})
