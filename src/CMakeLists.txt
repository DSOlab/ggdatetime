add_library(datetime)

target_sources(datetime
  PRIVATE
    dat.cpp
    datetime_io_core.cpp
    modified_julian_day.cpp
    month.cpp
    strmonth.cpp
    tpdateutc.cpp
    twopartdates.cpp
    utc2tai.cpp
    ydoy_date.cpp
    ymd_date.cpp
)

target_include_directories(datetime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Install headers at: $PREFIX/datetime/...
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
	DESTINATION include/datetime
)

# install library
install(TARGETS datetime
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin
)
