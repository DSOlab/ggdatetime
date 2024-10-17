add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

# Define a macro to add the private include directory for all targets
macro(add_internal_includes target_name)
  target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
endmacro()

add_executable(cal2jd cal2jd.cpp)
add_internal_includes(cal2jd)
add_executable(jd2cal jd2cal.cpp)
add_internal_includes(jd2cal)
add_executable(dat dat.cpp)
add_internal_includes(dat)
add_executable(epj epj_date.cpp)
add_internal_includes(epj)

target_link_libraries(cal2jd PRIVATE datetime sofa_c)
target_link_libraries(jd2cal PRIVATE datetime sofa_c)
target_link_libraries(dat PRIVATE datetime sofa_c)
target_link_libraries(epj PRIVATE datetime sofa_c)

add_test(NAME cal2jd COMMAND cal2jd)
add_test(NAME jd2cal COMMAND jd2cal)
add_test(NAME dat COMMAND dat)
add_test(NAME epj COMMAND epj)
