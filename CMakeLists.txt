cmake_minimum_required(VERSION 3.8)

project(
  datetime
  VERSION 1.1.0
  DESCRIPTION "DateTime Library for Satellite Geoddesy"
  LANGUAGES CXX
)

# The tests
include(CTest)
add_subdirectory(test//unit_tests)
find_library(sofa sofa_c)
if (sofa)
  add_subdirectory(test//sofa)
endif()
enable_testing()

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

add_library(datetime)
target_include_directories(datetime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/datetime>
)

add_subdirectory(src)

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
      DESTINATION lib/cmake/datetime
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "datetimeConfigVersion.cmake"
  VERSION ${datetime_version}
  COMPATIBILITY AnyNewerVersion
  )
install(FILES "datetimeConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/datetimeConfigVersion.cmake"
  DESTINATION lib/cmake/dso
  )
