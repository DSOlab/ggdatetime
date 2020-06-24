#include <iostream>
#include <chrono>
#include <cassert>
#include <limits>
#include "dtfund.hpp"
#include "dtcalendar.hpp"
#include "datetime_read.hpp"

using namespace ngpt;
/*
constexpr long MilliSec = 1000L;
constexpr long MicroSec = 1000000L;
constexpr long NanoSec  = 1000000000L;
constexpr long maxL     = std::numeric_limits<long int>::max();
*/

int main()
{
  std::cout<<"\nThis is a test program to check the conversion between"
           <<"\ncalendar dates and GPS Week / Seconds of Week format."
           <<"\nChecks use the calendar date 2019-MAR-18 13 25 24.123,"
           <<"\ncorresponds to gps week: 2045 and sow: 134724"
           <<"\nIf the program fails, an assertion will be thrown.";
  std::cout<<"\n-------------------------------------------------------";
  
  const char* date1_str = "2019 Mar 18 13 25 24.0";
  const char* date2_str = "2019/MAR/18 13 25 24.1230";
  const char* date3_str = "2019-MAR-18 13 25 24.1231230";
  
  datetime<seconds> d1      = ngpt::strptime_yod_hms<seconds>(date1_str);
  datetime<milliseconds> d2 = ngpt::strptime_yod_hms<milliseconds>(date2_str);
  datetime<microseconds> d3 = ngpt::strptime_yod_hms<microseconds>(date3_str);
  std::cout << "\nd1 = " << d1.stringify() << " (" << d1.sec_as_i() << ")";
  std::cout << "\nd2 = " << d2.stringify() << " (" << d2.sec_as_i() << ")";
  std::cout << "\nd3 = " << d3.stringify() << " (" << d3.sec_as_i() << ")";

  gps_week w1, w2, w3;
  long sow1, sow2, sow3;

  w1 = d1.as_gps_wsow(sow1);
  std::cout<<"\nDate is GPS Week: "<<w1.as_underlying_type()<<", SOW:"<<sow1;
  assert(w1 == gps_week(2045) && sow1==134724);
  w2 = d2.as_gps_wsow(sow2);
  std::cout<<"\nDate is GPS Week: "<<w2.as_underlying_type()<<", SOW:"<<sow2;
  w3 = d3.as_gps_wsow(sow3);
  std::cout<<"\nDate is GPS Week: "<<w3.as_underlying_type()<<", SOW:"<<sow3;

  assert(w1 == w2 && w2 == w3);
  assert(sow1 == sow2 && sow2 == sow3);
  std::cout<<"\n>Date to GPS Week & SecOfWeek OK!";

  ngpt::seconds sow;
  ngpt::milliseconds mlsow;
  ngpt::microseconds mcsow;
  w1 = d1.as_gps_wsow(sow);
  std::cout<<"\nDate is GPS Week: "<<w1.as_underlying_type()<<", Seconds OW     :"<<sow.as_underlying_type();
  w2 = d2.as_gps_wsow(mlsow);
  std::cout<<"\nDate is GPS Week: "<<w2.as_underlying_type()<<", MilliSeconds OW:"<<mlsow.as_underlying_type();
  w3 = d3.as_gps_wsow(mcsow);
  std::cout<<"\nDate is GPS Week: "<<w3.as_underlying_type()<<", MicroSeconds OW:"<<mcsow.as_underlying_type();

  int dow1, dow2, dow3;
  dow1 = day_of_week(sow);
  dow2 = day_of_week(mlsow);
  dow3 = day_of_week(mcsow);
  std::cout<<"\nDay of week = "<<dow1;
  assert(dow1 == dow2 && dow2 == dow3);
    
  std::cout<<"\n-------------------------------------------------------"
           <<"\nEnd of Test\n";
  return 0;
}
