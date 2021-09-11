noinst_PROGRAMS = testSecDifTime

MCXXFLAGS = \
	-std=c++17 \
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

testSecDifTime_SOURCES   = sec_dif_time.cpp
testSecDifTime_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ## -L$(top_srcdir)/src
testSecDifTime_LDADD     = $(top_srcdir)/src/libggdatetime.la
