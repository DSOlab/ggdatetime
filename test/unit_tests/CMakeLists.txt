# add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-unused-variable)

# Define a macro to add the private include directory for all targets
macro(add_internal_includes target_name)
  target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
endmacro()

add_executable(test_datetime datetime.cpp)
add_internal_includes(test_datetime)
target_link_libraries(test_datetime PRIVATE datetime)
add_test(NAME test_datetime COMMAND test_datetime)

add_executable(datetime_addition datetime_addition.cpp)
add_internal_includes(datetime_addition)
target_link_libraries(datetime_addition PRIVATE datetime)
add_test(NAME datetime_addition COMMAND datetime_addition)

add_executable(datetime_addition_sec datetime_addition_sec.cpp)
add_internal_includes(datetime_addition_sec)
target_link_libraries(datetime_addition_sec PRIVATE datetime)
add_test(NAME datetime_addition_sec COMMAND datetime_addition_sec)

add_executable(datetime_addition_sec2 datetime_addition_sec2.cpp)
add_internal_includes(datetime_addition_sec2)
target_link_libraries(datetime_addition_sec2 PRIVATE datetime)
add_test(NAME datetime_addition_sec2 COMMAND datetime_addition_sec2)

add_executable(datetime_diff datetime_diff.cpp)
add_internal_includes(datetime_diff)
target_link_libraries(datetime_diff PRIVATE datetime)
add_test(NAME datetime_diff COMMAND datetime_diff)

add_executable(dom dom.cpp)
add_internal_includes(dom)
target_link_libraries(dom PRIVATE datetime)
add_test(NAME dom COMMAND dom)

add_executable(doy doy.cpp)
add_internal_includes(doy)
target_link_libraries(doy PRIVATE datetime)
add_test(NAME doy COMMAND doy)

add_executable(dwrite dwrite.cpp)
add_internal_includes(dwrite)
target_link_libraries(dwrite PRIVATE datetime)
add_test(NAME dwrite COMMAND dwrite)

add_executable(dwrite2 dwrite2.cpp)
add_internal_includes(dwrite2)
target_link_libraries(dwrite2 PRIVATE datetime)
add_test(NAME dwrite2 COMMAND dwrite2)

add_executable(dwrite3 dwrite3.cpp)
add_internal_includes(dwrite3)
target_link_libraries(dwrite3 PRIVATE datetime)
add_test(NAME dwrite3 COMMAND dwrite3)

add_executable(dwrite4 dwrite4.cpp)
add_internal_includes(dwrite4)
target_link_libraries(dwrite4 PRIVATE datetime)
add_test(NAME dwrite4 COMMAND dwrite4)

add_executable(dwrite5 dwrite5.cpp)
add_internal_includes(dwrite5)
target_link_libraries(dwrite5 PRIVATE datetime)
add_test(NAME dwrite5 COMMAND dwrite5)

add_executable(dwrite6 dwrite6.cpp)
add_internal_includes(dwrite6)
target_link_libraries(dwrite6 PRIVATE datetime)
add_test(NAME dwrite6 COMMAND dwrite6)

add_executable(dwrite7 dwrite7.cpp)
add_internal_includes(dwrite7)
target_link_libraries(dwrite7 PRIVATE datetime)
add_test(NAME dwrite7 COMMAND dwrite7)

add_executable(dwrite8 dwrite8.cpp)
add_internal_includes(dwrite8)
target_link_libraries(dwrite8 PRIVATE datetime)
add_test(NAME dwrite8 COMMAND dwrite8)

add_executable(dwrite9 dwrite9.cpp)
add_internal_includes(dwrite9)
target_link_libraries(dwrite9 PRIVATE datetime)
add_test(NAME dwrite9 COMMAND dwrite9)

add_executable(hours hours.cpp)
add_internal_includes(hours)
target_link_libraries(hours PRIVATE datetime)
add_test(NAME hours COMMAND hours)

add_executable(interval interval.cpp)
add_internal_includes(interval)
target_link_libraries(interval PRIVATE datetime)
add_test(NAME interval COMMAND interval)

add_executable(mjd mjd.cpp)
add_internal_includes(mjd)
target_link_libraries(mjd PRIVATE datetime)
add_test(NAME mjd COMMAND mjd)

add_executable(month month.cpp)
add_internal_includes(month)
target_link_libraries(month PRIVATE datetime)
add_test(NAME month COMMAND month)

add_executable(tpdates1 tpdates1.cpp)
add_internal_includes(tpdates1)
target_link_libraries(tpdates1 PRIVATE datetime)
add_test(NAME tpdates1 COMMAND tpdates1)

add_executable(tpdates2 tpdates2.cpp)
add_internal_includes(tpdates2)
target_link_libraries(tpdates2 PRIVATE datetime)
add_test(NAME tpdates2 COMMAND tpdates2)

add_executable(tpdates3 tpdates3.cpp)
add_internal_includes(tpdates3)
target_link_libraries(tpdates3 PRIVATE datetime)
add_test(NAME tpdates3 COMMAND tpdates3)

add_executable(tpdates4 tpdates4.cpp)
add_internal_includes(tpdates4)
target_link_libraries(tpdates4 PRIVATE datetime)
add_test(NAME tpdates4 COMMAND tpdates4)

add_executable(ydoy_date ydoy_date.cpp)
add_internal_includes(ydoy_date)
target_link_libraries(ydoy_date PRIVATE datetime)
add_test(NAME ydoy_date COMMAND ydoy_date)

add_executable(year year.cpp)
add_internal_includes(year)
target_link_libraries(year PRIVATE datetime)
add_test(NAME year COMMAND year)

add_executable(ymd_date ymd_date.cpp)
add_internal_includes(ymd_date)
target_link_libraries(ymd_date PRIVATE datetime)
add_test(NAME ymd_date COMMAND ymd_date)

add_executable(dread21 dread21.cpp)
add_internal_includes(dread21)
target_link_libraries(dread21 PRIVATE datetime)
add_test(NAME dread21 COMMAND dread21)

add_executable(dread2 dread2.cpp)
add_internal_includes(dread2)
target_link_libraries(dread2 PRIVATE datetime)
add_test(NAME dread2 COMMAND dread2)

add_executable(dread3 dread3.cpp)
add_internal_includes(dread3)
target_link_libraries(dread3 PRIVATE datetime)
add_test(NAME dread3 COMMAND dread3)

add_executable(leapday leapday.cpp)
add_internal_includes(leapday)
target_link_libraries(leapday PRIVATE datetime)
add_test(NAME leapday COMMAND leapday)

add_executable(tpdate_add tpdate_add.cpp)
add_internal_includes(tpdate_add)
target_link_libraries(tpdate_add PRIVATE datetime)
add_test(NAME tpdate_add COMMAND tpdate_add)

add_executable(sectype_casts sectype_casts.cpp)
add_internal_includes(sectype_casts)
target_link_libraries(sectype_casts PRIVATE datetime)
add_test(NAME sectype_casts COMMAND sectype_casts)

add_executable(datetime_static_ce_constructor datetime_static_ce_constructor.cpp)
add_internal_includes(datetime_static_ce_constructor)
target_link_libraries(datetime_static_ce_constructor PRIVATE datetime)
add_test(NAME datetime_static_ce_constructor COMMAND datetime_static_ce_constructor)

add_executable(datetime_interval_constructor datetime_interval_constructor.cpp)
add_internal_includes(datetime_interval_constructor)
target_link_libraries(datetime_interval_constructor PRIVATE datetime)
add_test(NAME datetime_interval_constructor COMMAND datetime_interval_constructor)

add_executable(test_interval_overlap test_interval_overlap.cpp)
add_internal_includes(test_interval_overlap)
target_link_libraries(test_interval_overlap PRIVATE datetime)
add_test(NAME test_interval_overlap COMMAND test_interval_overlap)

add_executable(leap_insertion_dates_mjd leap_insertion_dates_mjd.cpp)
add_internal_includes(leap_insertion_dates_mjd)
target_link_libraries(leap_insertion_dates_mjd PRIVATE datetime)
add_test(NAME leap_insertion_dates_mjd COMMAND leap_insertion_dates_mjd)

add_executable(from_mjdepoch from_mjdepoch.cpp)
add_internal_includes(from_mjdepoch)
target_link_libraries(from_mjdepoch PRIVATE datetime)
add_test(NAME from_mjdepoch COMMAND from_mjdepoch)

add_executable(timescales time_scales.cpp)
add_internal_includes(timescales)
target_link_libraries(timescales PRIVATE datetime)
add_test(NAME timescales COMMAND timescales)
