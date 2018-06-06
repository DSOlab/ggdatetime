noinst_PROGRAMS = testDatetime

MCXXFLAGS = \
	-std=c++14 \
	-g \
	-pg \
	-Wno-unused-variable \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Wdisabled-optimization \
	-DDEBUG

testDatetime_SOURCES   = example.cpp
testDatetime_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testDatetime_LDADD     = $(top_srcdir)/src/libggdatetime.la
