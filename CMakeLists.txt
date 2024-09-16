cmake_minimum_required(VERSION 3.1)

project(
  datetime
  VERSION 1.1.0
  DESCRIPTION "DateTime Library for Satellite Geoddesy"
  LANGUAGES CXX
)

# The library
add_subdirectory(src)

# The tests
include(CTest)
add_subdirectory(test//unit_tests)
find_library(sofa sofa_c)
if (sofa)
  add_subdirectory(test//sofa)
endif()
enable_testing()

add_compile_options(
  -Wall -Wextra -Werror -pedantic -W -Wshadow -march=native
  $<$<CONFIG:RELEASE>:-O2 -march=native>
  $<$<CONFIG:DEBUG>:-g -pg -Wdisabled-optimization -DDEBUG>
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
