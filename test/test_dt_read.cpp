#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>

#include "datetime_read.hpp"
#include "dtcalendar.hpp"
#include "dtfund.hpp"

using namespace ngpt;
/*
constexpr long MilliSec = 1000L;
constexpr long MicroSec = 1000000L;
constexpr long NanoSec  = 1000000000L;
constexpr long maxL     = std::numeric_limits<long int>::max();
*/

int main() {
  //
  // Let's try reading dates
  // -----------------------------------------------------------------------
  //
  std::cout << "Testing datetime reading functions\n";
  std::cout
      << "-------------------------------------------------------------\n";

  // Test datetime strings
  /*char* __end = nullptr;*/
  const char *date1_str = "2015-12-30 12:09:30";
  const char *date2_str = "2015/12/30 12:09:30";
  const char *date3_str = "2015-12-30 12:09:30.000011";
  const char *date4_str = "2015 12 30 12 9 30";

  // Resolve the test strings
  datetime<seconds> dfs1 =
      ngpt::strptime_ymd_hms<seconds>(date1_str /*, &__end*/);
  datetime<seconds> dfs2 =
      ngpt::strptime_ymd_hms<seconds>(date2_str /*, &__end*/);
  datetime<seconds> dfs3 =
      ngpt::strptime_ymd_hms<seconds>(date3_str /*, &__end*/);
  datetime<seconds> dfs4 =
      ngpt::strptime_ymd_hms<seconds>(date4_str /*, &__end*/);

  // make sure we resolved the datetime strings correctly
  datetime<seconds> dfs1_{year(2015), month(12),  day_of_month(30),
                          hours(12),  minutes(9), seconds(30)};
  assert(dfs1 == dfs1_ && dfs1 == dfs2 && dfs2 == dfs3 && dfs3 == dfs4);

  // make sure we can resolve the fractional seconds correctly
  auto dfs5 = ngpt::strptime_ymd_hms<microseconds>(date3_str);
  datetime<microseconds> dfs5_{year(2015), month(12),  day_of_month(30),
                               hours(12),  minutes(9), microseconds(30000011)};
  assert(dfs5 == dfs5_);

  // make sure if only the date is passed, the hms part is set to 0
  const char *date6_str = "2015-12-30";
  datetime<seconds> dfs6 = ngpt::strptime_ymd<seconds>(date6_str);
  const char *date7_str = "2015-12-30 0 0 0";
  datetime<seconds> dfs7 = ngpt::strptime_ymd<seconds>(date7_str);
  datetime<seconds> dfs7_{year(2015), month(12),  day_of_month(30),
                          hours(0),   minutes(0), seconds(0)};
  assert(dfs6 == dfs7 && dfs7 == dfs7_);

  // test if we can resolve months as strings
  const char *date8_str = "2015 Dec 30 12 9 30";
  const char *date9_str = "2015/DEC/30 12 9 30";
  const char *date10_str = "2015-DEC-30 12 9 30";
  datetime<seconds> dfs8 = ngpt::strptime_yod_hms<seconds>(date8_str);
  datetime<seconds> dfs9 = ngpt::strptime_yod_hms<seconds>(date9_str);
  datetime<seconds> dfs10 = ngpt::strptime_yod_hms<seconds>(date10_str);
  assert(dfs8 == dfs9 && dfs9 == dfs10);
  assert(dfs1 == dfs1_ && dfs1 == dfs8 && dfs8 == dfs9 && dfs9 == dfs10);

  std::cout << "Part C -- OK\n\n";

  return 0;
}
