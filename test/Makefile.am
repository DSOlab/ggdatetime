noinst_PROGRAMS = testDatetime \
		  testRead \
		  testGPSt

MCXXFLAGS = \
	-std=c++14 \
	-Wno-unused-variable \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-O2

testDatetime_SOURCES   = example.cpp
testDatetime_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testDatetime_LDADD     = $(top_srcdir)/src/libggdatetime.la

testRead_SOURCES   = test_dt_read.cpp
testRead_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testRead_LDADD     = $(top_srcdir)/src/libggdatetime.la

testGPSt_SOURCES   = test_gpst.cpp
testGPSt_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testGPSt_LDADD     = $(top_srcdir)/src/libggdatetime.la
