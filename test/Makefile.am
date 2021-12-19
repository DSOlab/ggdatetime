noinst_PROGRAMS = testDatetime \
		  testRead \
		  testGPSt \
		  testLeap \
		  testOps \
		  testSecDif

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

testDatetime_SOURCES   = example.cpp
testDatetime_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testDatetime_LDADD     = $(top_srcdir)/src/libdatetime.la

testRead_SOURCES   = test_dt_read.cpp
testRead_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testRead_LDADD     = $(top_srcdir)/src/libdatetime.la

testGPSt_SOURCES   = test_gpst.cpp
testGPSt_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testGPSt_LDADD     = $(top_srcdir)/src/libdatetime.la

testLeap_SOURCES   = test_dat.cpp
testLeap_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testLeap_LDADD     = $(top_srcdir)/src/libdatetime.la

testOps_SOURCES   = test_operators.cpp
testOps_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testOps_LDADD     = $(top_srcdir)/src/libdatetime.la

testSecDif_SOURCES   = test_secdif.cpp
testSecDif_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src ##-L$(top_srcdir)/src
testSecDif_LDADD     = $(top_srcdir)/src/libdatetime.la
