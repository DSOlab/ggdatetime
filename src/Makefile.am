##
##  The library will be called: (lib)"ggdatetime".
## ----------------------------------------------
##
lib_LTLIBRARIES = libggdatetime.la

##
##  Only use the following flags for debuging purposes.
## ----------------------------------------------------
##
libggdatetime_la_CXXFLAGS = \
	-std=c++14 \
	-g \
	-pg \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Winline \
	-Wdisabled-optimization \
	-march=native \
	-DDEBUG

##
##  Header files (distributed).
## ----------------------------
##
dist_include_HEADERS = \
	dtfund.hpp \
	dtcalendar.hpp \
	datetime_read.hpp

##
##  Source files (distributed).
## ----------------------------
##
dist_libggdatetime_la_SOURCES = \
	dtfund.cpp
