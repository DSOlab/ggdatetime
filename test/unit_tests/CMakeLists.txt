# add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

add_executable(test_datetime datetime.cpp)
target_link_libraries(test_datetime PRIVATE datetime)
add_test(NAME test_datetime COMMAND test_datetime)

add_executable(datetime_addition datetime_addition.cpp)
target_link_libraries(datetime_addition PRIVATE datetime)
add_test(NAME datetime_addition COMMAND datetime_addition)

add_executable(datetime_addition_sec datetime_addition_sec.cpp)
target_link_libraries(datetime_addition_sec PRIVATE datetime)
add_test(NAME datetime_addition_sec COMMAND datetime_addition_sec)

add_executable(datetime_addition_sec2 datetime_addition_sec2.cpp)
target_link_libraries(datetime_addition_sec2 PRIVATE datetime)
add_test(NAME datetime_addition_sec2 COMMAND datetime_addition_sec2)

add_executable(datetime_diff datetime_diff.cpp)
target_link_libraries(datetime_diff PRIVATE datetime)
add_test(NAME datetime_diff COMMAND datetime_diff)

add_executable(dom dom.cpp)
target_link_libraries(dom PRIVATE datetime)
add_test(NAME dom COMMAND dom)

add_executable(doy doy.cpp)
target_link_libraries(doy PRIVATE datetime)
add_test(NAME doy COMMAND doy)

add_executable(dwrite dwrite.cpp)
target_link_libraries(dwrite PRIVATE datetime)
add_test(NAME dwrite COMMAND dwrite)

add_executable(dwrite2 dwrite2.cpp)
target_link_libraries(dwrite2 PRIVATE datetime)
add_test(NAME dwrite2 COMMAND dwrite2)

add_executable(dwrite3 dwrite3.cpp)
target_link_libraries(dwrite3 PRIVATE datetime)
add_test(NAME dwrite3 COMMAND dwrite3)

add_executable(dwrite4 dwrite4.cpp)
target_link_libraries(dwrite4 PRIVATE datetime)
add_test(NAME dwrite4 COMMAND dwrite4)

add_executable(dwrite5 dwrite5.cpp)
target_link_libraries(dwrite5 PRIVATE datetime)
add_test(NAME dwrite5 COMMAND dwrite5)

add_executable(dwrite6 dwrite6.cpp)
target_link_libraries(dwrite6 PRIVATE datetime)
add_test(NAME dwrite6 COMMAND dwrite6)

add_executable(dwrite7 dwrite7.cpp)
target_link_libraries(dwrite7 PRIVATE datetime)
add_test(NAME dwrite7 COMMAND dwrite7)

add_executable(dwrite8 dwrite8.cpp)
target_link_libraries(dwrite8 PRIVATE datetime)
add_test(NAME dwrite8 COMMAND dwrite8)

add_executable(dwrite9 dwrite9.cpp)
target_link_libraries(dwrite9 PRIVATE datetime)
add_test(NAME dwrite9 COMMAND dwrite9)

#add_executable(hour_mock_1 hour_mock_1.cpp)
#target_link_libraries(hour_mock_1 PRIVATE datetime)
#add_test(NAME hour_mock_1 COMMAND hour_mock_1)
#
#add_executable(hour_mock_2 hour_mock_2.cpp)
#target_link_libraries(hour_mock_2 PRIVATE datetime)
#add_test(NAME hour_mock_2 COMMAND hour_mock_2)
#
#add_executable(hour_mock_3 hour_mock_3.cpp)
#target_link_libraries(hour_mock_3 PRIVATE datetime)
#add_test(NAME hour_mock_3 COMMAND hour_mock_3)

add_executable(hours hours.cpp)
target_link_libraries(hours PRIVATE datetime)
add_test(NAME hours COMMAND hours)

add_executable(interval interval.cpp)
target_link_libraries(interval PRIVATE datetime)
add_test(NAME interval COMMAND interval)

add_executable(mjd mjd.cpp)
target_link_libraries(mjd PRIVATE datetime)
add_test(NAME mjd COMMAND mjd)

add_executable(month month.cpp)
target_link_libraries(month PRIVATE datetime)
add_test(NAME month COMMAND month)

add_executable(tpdates1 tpdates1.cpp)
target_link_libraries(tpdates1 PRIVATE datetime)
add_test(NAME tpdates1 COMMAND tpdates1)

add_executable(tpdates2 tpdates2.cpp)
target_link_libraries(tpdates2 PRIVATE datetime)
add_test(NAME tpdates2 COMMAND tpdates2)

add_executable(tpdates3 tpdates3.cpp)
target_link_libraries(tpdates3 PRIVATE datetime)
add_test(NAME tpdates3 COMMAND tpdates3)

add_executable(tpdates4 tpdates4.cpp)
target_link_libraries(tpdates4 PRIVATE datetime)
add_test(NAME tpdates4 COMMAND tpdates4)

add_executable(ydoy_date ydoy_date.cpp)
target_link_libraries(ydoy_date PRIVATE datetime)
add_test(NAME ydoy_date COMMAND ydoy_date)

add_executable(year year.cpp)
target_link_libraries(year PRIVATE datetime)
add_test(NAME year COMMAND year)

add_executable(ymd_date ymd_date.cpp)
target_link_libraries(ymd_date PRIVATE datetime)
add_test(NAME ymd_date COMMAND ymd_date)

add_executable(dread21 dread21.cpp)
target_link_libraries(dread21 PRIVATE datetime)
add_test(NAME dread21 COMMAND dread21)

add_executable(dread2 dread2.cpp)
target_link_libraries(dread2 PRIVATE datetime)
add_test(NAME dread2 COMMAND dread2)

add_executable(dread3 dread3.cpp)
target_link_libraries(dread3 PRIVATE datetime)
add_test(NAME dread3 COMMAND dread3)

add_executable(leapday leapday.cpp)
target_link_libraries(leapday PRIVATE datetime)
add_test(NAME leapday COMMAND leapday)

add_executable(tpdate_add tpdate_add.cpp)
target_link_libraries(tpdate_add PRIVATE datetime)
add_test(NAME tpdate_add COMMAND tpdate_add)

#add_executable(year_mock_1 year_mock_1.cpp)
#set_target_properties(year_mock_1 PROPERTIES
#                      EXCLUDE_FROM_ALL TRUE
#                      EXCLUDE_FROM_DEFAULT_BUILD TRUE)
#target_compile_definitions(year_mock_1 PRIVATE SHOULD_FAIL_TEST_001)
#target_link_libraries(year_mock_1 PRIVATE datetime)
#add_test(NAME year_mock_1
#         COMMAND ${CMAKE_COMMAND} --build . --target year_mock_1 --config $<CONFIGURATION>
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# Expect these tests to fail (i.e. cmake --build should return
# a non-zero value)
#set_tests_properties(year_mock_1 PROPERTIES WILL_FAIL TRUE)
#
#add_executable(year_mock_2 year_mock_2.cpp)
#target_link_libraries(year_mock_2 PRIVATE datetime)
#add_test(NAME year_mock_2 COMMAND year_mock_2)
#
#add_executable(year_mock_3 year_mock_3.cpp)
#target_link_libraries(year_mock_3 PRIVATE datetime)
#add_test(NAME year_mock_3 COMMAND year_mock_3)

