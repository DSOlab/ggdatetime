add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

# Define a macro to add the private include directory for all targets
macro(add_internal_includes target_name)
  target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
endmacro()

##
## fractional_seconds_op_equal.cpp
##
add_executable(fail-build-fractional-seconds-op-equal fractional_seconds_op_equal.cpp)
add_internal_includes(fail-build-fractional-seconds-op-equal)
target_link_libraries(fail-build-fractional-seconds-op-equal datetime)
set_target_properties(fail-build-fractional-seconds-op-equal PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)
add_test(
    NAME fail-build-fractional-seconds-op-equal
    COMMAND ${CMAKE_COMMAND} --build . --target fail-build-fractional-seconds-op-equal --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} # the place where we would run ctest
)
##
## Note that the error message is compiler-specific ...
##
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set_tests_properties(fail-build-fractional-seconds-op-equal PROPERTIES
    PASS_REGULAR_EXPRESSION "'FractionalSeconds' and 'double'"
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_tests_properties(fail-build-fractional-seconds-op-equal PROPERTIES
    PASS_REGULAR_EXPRESSION "no match for ‘operator==’"
  )
endif()

##
## fractional_seconds_op_greter.cpp
##
add_executable(fail-build-fractional-seconds-op-greater fractional_seconds_op_greater.cpp)
add_internal_includes(fail-build-fractional-seconds-op-greater)
target_link_libraries(fail-build-fractional-seconds-op-greater datetime)
set_target_properties(fail-build-fractional-seconds-op-greater PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)
add_test(
    NAME fail-build-fractional-seconds-op-greater
    COMMAND ${CMAKE_COMMAND} --build . --target fail-build-fractional-seconds-op-greater --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} # the place where we would run ctest
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set_tests_properties(fail-build-fractional-seconds-op-greater PROPERTIES
    PASS_REGULAR_EXPRESSION "'FractionalSeconds' and 'double'"
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_tests_properties(fail-build-fractional-seconds-op-greater PROPERTIES
    PASS_REGULAR_EXPRESSION "no match for ‘operator>’"
  )
endif()

##
## datetime_diff_type.cpp
##
add_executable(fail-build-datetime-diff-type datetime_diff_type.cpp)
add_internal_includes(fail-build-datetime-diff-type)
target_link_libraries(fail-build-datetime-diff-type datetime)
set_target_properties(fail-build-datetime-diff-type PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)
add_test(
    NAME fail-build-datetime-diff-type
    COMMAND ${CMAKE_COMMAND} --build . --target fail-build-datetime-diff-type --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} # the place where we would run ctest
)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set_tests_properties(fail-build-datetime-diff-type PROPERTIES
    PASS_REGULAR_EXPRESSION "deduced conflicting types for parameter"
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_tests_properties(fail-build-datetime-diff-type PROPERTIES
    PASS_REGULAR_EXPRESSION "no match for ‘operator>’"
  )
endif()

##
## fund_type_op_plusequal.cpp
##
if(NOT ALLOW_DT_INTEGRAL_MATH)
  add_executable(fail-build-fund-type-op-plusequal fund_type_op_plusequal.cpp)
  add_internal_includes(fail-build-fund-type-op-plusequal)
  target_link_libraries(fail-build-fund-type-op-plusequal datetime)
  set_target_properties(fail-build-fund-type-op-plusequal PROPERTIES
      EXCLUDE_FROM_ALL TRUE
      EXCLUDE_FROM_DEFAULT_BUILD TRUE
  )
  add_test(
      NAME fail-build-fund-type-op-plusequal
      COMMAND ${CMAKE_COMMAND} --build . --target fail-build-fund-type-op-plusequal --config $<CONFIG>
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_tests_properties(fail-build-fund-type-op-plusequal PROPERTIES
      PASS_REGULAR_EXPRESSION "deduced conflicting types for parameter"
    )
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_tests_properties(fail-build-fund-type-op-plusequal PROPERTIES
      PASS_REGULAR_EXPRESSION "deduced conflicting types for parameter ‘DType’"
    )
  endif()
endif()
