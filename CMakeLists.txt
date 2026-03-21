cmake_minimum_required(VERSION 3.22)

project(derapkbuilder)

set(CMAKE_C_STANDARD 90)


set(SOURCES
  src/main.c
  src/config.c)

add_executable(derapkbuilder ${SOURCES})

target_compile_options(derapkbuilder
  PRIVATE
  -pedantic
  -pedantic-errors
  -Wall
  -Wextra
  -Werror)

target_link_libraries(derapkbuilder
  PRIVATE
  yaml)

target_include_directories(derapkbuilder
  PRIVATE
  include/)
