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
	-std=c++17 \
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
##  Header files (distributed) installed in /$(includedir)/$(package name).
## ------------------------------------------------------------------------
##
pkginclude_HEADERS = \
	dtfund.hpp \
	dtcalendar.hpp \
	datetime_read.hpp \
	datetime_write.hpp \
	gnsstm.hpp

##
##  Source files (distributed).
## ----------------------------
##
dist_libggdatetime_la_SOURCES = \
	dtfund.cpp \
	dat.cpp
