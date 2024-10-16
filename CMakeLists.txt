cmake_minimum_required(VERSION 3.8)

project(
  datetime
  VERSION 1.1.0
  DESCRIPTION "DateTime Library for Satellite Geoddesy"
  LANGUAGES CXX
)

# Enable clang-tidy option
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
add_compile_options(-Wall 
  -Wextra 
  -Werror 
  -pedantic 
  -W 
  -Wshadow 
  $<$<CONFIG:Release>:-O2>
  $<$<CONFIG:Release>:-march=native>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-pg> 
  $<$<CONFIG:Debug>:-Wdisabled-optimization>
  )
add_compile_definitions(
  $<$<CONFIG:Debug>:DEBUG>
)

# clang-tidy (targets that follow will be checked)
if(ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=bugprone-*,\
  clang-analyzer-*,\
  cppcoreguidelines-*,\
  llvm-*,\
  modernize-*,\
  performance-*,\
  -modernize-use-trailing-return-type,\
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,\
  -cppcoreguidelines-pro-bounds-constant-array-index,\
  -cppcoreguidelines-pro-type-vararg")
  message(STATUS "clang-tidy is enabled.")
else()
  message(STATUS "clang-tidy is disabled. To enable it, use -DENABLE_CLANG_TIDY=ON.")
endif()

add_library(datetime)
target_include_directories(datetime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/datetime>
)

add_subdirectory(src/lib)

# add executables
add_executable(ymd2mjd src/bin/ymd2mjd.cpp)
target_link_libraries(ymd2mjd PRIVATE datetime)
add_executable(mjd2ymd src/bin/mjd2ymd.cpp)
target_link_libraries(mjd2ymd PRIVATE datetime)
add_executable(mjd2ydoy src/bin/mjd2ydoy.cpp)
target_link_libraries(mjd2ydoy PRIVATE datetime)

# disable clang-tidy (targets that follow will not be checked)
set(CMAKE_CXX_CLANG_TIDY "")

# The tests
include(CTest)
add_subdirectory(test/unit_tests)
add_subdirectory(test/should_not_compile)
find_library(sofa sofa_c)
if (sofa)
  add_subdirectory(test/sofa)
  message(STATUS "found sofa lib, will compile relevant tests")
else()
  message(STATUS "sofa lib not found; tests in test/sofa will not be compiled.")
endif()
enable_testing()

# Install headers at: $PREFIX/datetime/...
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
	DESTINATION include/datetime
)

# install library
install(TARGETS datetime
        EXPORT datetimeTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(EXPORT datetimeTargets
        FILE datetimeTargets.cmake
        NAMESPACE dso::
        DESTINATION lib/cmake
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "datetimeConfigVersion.cmake"
  VERSION ${datetime_version}
  COMPATIBILITY AnyNewerVersion
)

install(FILES "datetimeConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/datetimeConfigVersion.cmake"
  DESTINATION lib/cmake
)
