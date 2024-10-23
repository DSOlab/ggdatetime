cmake_minimum_required(VERSION 3.8)

project(
  datetime
  VERSION 1.1.0
  DESCRIPTION "DateTime Library for Satellite Geodesy"
  LANGUAGES CXX
)

# Enable clang-tidy option
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

# Define an option for building tests (defaults to ON)
option(BUILD_TESTING "Enable building of tests" ON)

# compiler flags
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

# the library and includes
add_library(datetime)
target_include_directories(datetime 
PUBLIC
  # Public headers during build time
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  # Public headers during install time
  $<INSTALL_INTERFACE:include/datetime>
PRIVATE
  # Private/internal headers only during build time (src/core)
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  # Internal/private headers during install time (from include/datetime/core)
  $<INSTALL_INTERFACE:include/datetime/core>
)

# library source code
add_subdirectory(src/lib)

# add executables
add_executable(ymd2mjd src/bin/ymd2mjd.cpp)
target_link_libraries(ymd2mjd PRIVATE datetime)
target_include_directories(ymd2mjd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(mjd2ymd src/bin/mjd2ymd.cpp)
target_link_libraries(mjd2ymd PRIVATE datetime)
target_include_directories(mjd2ymd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(mjd2ydoy src/bin/mjd2ydoy.cpp)
target_link_libraries(mjd2ydoy PRIVATE datetime)
target_include_directories(mjd2ydoy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(ydoy2mjd src/bin/ydoy2mjd.cpp)
target_link_libraries(ydoy2mjd PRIVATE datetime)
target_include_directories(ydoy2mjd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(integral_seconds_limits src/bin/integral_datetime_limits.cpp)
target_link_libraries(integral_seconds_limits PRIVATE datetime)
target_include_directories(integral_seconds_limits PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# disable clang-tidy (targets that follow will not be checked)
set(CMAKE_CXX_CLANG_TIDY "")

# The tests
if(BUILD_TESTING)
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
endif()

# Install the executables
install(TARGETS ymd2mjd
        RUNTIME DESTINATION bin)
install(TARGETS mjd2ymd
        RUNTIME DESTINATION bin)
install(TARGETS mjd2ydoy
        RUNTIME DESTINATION bin)
install(TARGETS ydoy2mjd
        RUNTIME DESTINATION bin)

# install library
install(TARGETS datetime
        EXPORT datetimeTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

# Install public headers at: $PREFIX/datetime/...
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
	DESTINATION include/datetime
)

# Install private headers to a separate directory $PREFIX/datetime/core
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/core/ 
        DESTINATION include/datetime/core
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
