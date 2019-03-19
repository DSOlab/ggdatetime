#include <iostream>
#include <chrono>
#include <cassert>
#include <limits>

#include "dtfund.hpp"
#include "dtcalendar.hpp"
#include "datetime_read.hpp"

using namespace ngpt;

constexpr long MilliSec = 1000L;
constexpr long MicroSec = 1000000L;
constexpr long NanoSec  = 1000000000L;
constexpr long maxL     = std::numeric_limits<long int>::max();

int main()
{
  const char* date1_str = "2019 Mar 18 13 25 24.0";
  const char* date2_str = "2019/MAR/18 13 25 24123.0";
  const char* date3_str = "2019-MAR-18 13 25 24123123.0";
  
  datetime<seconds> d1      = ngpt::strptime_yod_hms<seconds>(date1_str);
  datetime<milliseconds> d2 = ngpt::strptime_yod_hms<milliseconds>(date2_str);
  datetime<microseconds> d3 = ngpt::strptime_yod_hms<microseconds>(date3_str);

  gps_week w;
  long sow;

  w = d1.as_gps_wsow(sow);
  std::cout<<"\nDateq is GPS Week: "<<w.as_underlying_type()<<", SOW:"<<sow;
  w = d2.as_gps_wsow(sow);
  std::cout<<"\nDateq is GPS Week: "<<w.as_underlying_type()<<", SOW:"<<sow;
  w = d3.as_gps_wsow(sow);
  std::cout<<"\nDateq is GPS Week: "<<w.as_underlying_type()<<", SOW:"<<sow;

  return 0;
}
