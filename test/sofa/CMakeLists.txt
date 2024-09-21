add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

add_executable(cal2jd cal2jd.cpp)
add_executable(jd2cal jd2cal.cpp)
add_executable(dat dat.cpp)
add_executable(epj epj_date.cpp)

target_link_libraries(cal2jd PRIVATE datetime sofa_c)
target_link_libraries(jd2cal PRIVATE datetime sofa_c)
target_link_libraries(dat PRIVATE datetime sofa_c)
target_link_libraries(epj PRIVATE datetime sofa_c)

add_test(NAME cal2jd COMMAND cal2jd)
add_test(NAME jd2cal COMMAND jd2cal)
add_test(NAME dat COMMAND dat)
add_test(NAME epj COMMAND epj)
