cmake_minimum_required(VERSION 3.22)

project(derapkbuilder)

set(CMAKE_C_STANDARD 90)


set(SOURCES
  src/main.c
  src/project.c)

add_executable(derapkbuilder ${SOURCES})

target_compile_options(derapkbuilder
  PRIVATE
  -pedantic
  -pedantic-errors
  -Wall
  -Wextra
  -Werror)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_options(derapkbuilder
    PRIVATE
    -fsanitize=address)
  target_link_options(derapkbuilder
    PRIVATE
    -fsanitize=address)
endif()

target_link_libraries(derapkbuilder
  PRIVATE
  yaml)

target_include_directories(derapkbuilder
  PRIVATE
  include/)
