add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

##
## fractional_seconds_op_equal.cpp
##
add_executable(fail-build-fractional-seconds-op-equal fractional_seconds_op_equal.cpp)
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
set_tests_properties(fail-build-fractional-seconds-op-equal PROPERTIES
  PASS_REGULAR_EXPRESSION "no match for ‘operator==’"
)

##
## fractional_seconds_op_greter.cpp
##
add_executable(fail-build-fractional-seconds-op-greater fractional_seconds_op_greater.cpp)
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
set_tests_properties(fail-build-fractional-seconds-op-greater PROPERTIES
  PASS_REGULAR_EXPRESSION "no match for ‘operator>’"
)

##
## datetime_diff_type.cpp
##
add_executable(fail-build-datetime-diff-type datetime_diff_type.cpp)
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
set_tests_properties(fail-build-datetime-diff-type PROPERTIES
  PASS_REGULAR_EXPRESSION "no match for ‘operator>’"
)
