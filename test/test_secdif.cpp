#include <iostream>
#include <chrono>
#include <cassert>
#include <limits>

#include "dtfund.hpp"
#include "dtcalendar.hpp"
#include "datetime_read.hpp"

using namespace ngpt;

int main()
{
  std::cout<<"Testing datetime subtract/difference functions\n";
  std::cout<<"-------------------------------------------------------------\n";
  const char* date1_str = "2015-12-30 12:09:30";
  const char* date2_str = "2015/12/30 12:09:30.001";
  const char* date3_str = "2015-12-30 12:09:30.000011";
  const char* date4_str = "2015-12-30 12:09:30.000012";
  auto d1 = ngpt::strptime_ymd_hms<seconds>(date1_str);
  auto d2 = ngpt::strptime_ymd_hms<milliseconds>(date2_str);
  auto d3 = ngpt::strptime_ymd_hms<microseconds>(date3_str);
  auto d4 = ngpt::strptime_ymd_hms<microseconds>(date4_str);

  auto df12 = delta_sec(d1, d2); // 0 - 1 milliseconds
  std::cout<<"\nDifference: "<<date1_str<<" - "<<date2_str<<" = "<<df12.as_underlying_type();
  assert( df12 == milliseconds{-1} );
  assert( df12.as_underlying_type() == -delta_sec(d2, d1).as_underlying_type() );
  
  auto df13 = delta_sec(d1, d3); // 0 - 11 microseconds
  std::cout<<"\nDifference: "<<date1_str<<" - "<<date3_str<<" = "<<df13.as_underlying_type();
  assert( df13 == microseconds{-11} );
  assert( df13.as_underlying_type() == -delta_sec(d3, d1).as_underlying_type() );
  
  auto df14 = delta_sec(d1, d4); // 0 - 12 microseconds
  std::cout<<"\nDifference: "<<date1_str<<" - "<<date4_str<<" = "<<df14.as_underlying_type();
  assert( df14 == microseconds{-12} );
  assert( df14.as_underlying_type() == -delta_sec(d4, d1).as_underlying_type() );
  
  auto df23 = delta_sec(d2, d3); // 1000 - 11 microseconds
  std::cout<<"\nDifference: "<<date2_str<<" - "<<date3_str<<" = "<<df23.as_underlying_type();
  assert( df23 == microseconds{1000-11} );
  assert( df23.as_underlying_type() == -delta_sec(d3, d2).as_underlying_type() );
  
  auto df24 = delta_sec(d2, d4); // 1000 - 12 microseconds
  std::cout<<"\nDifference: "<<date2_str<<" - "<<date4_str<<" = "<<df24.as_underlying_type();
  assert( df24 == microseconds{1000-12} );
  assert( df24.as_underlying_type() == -delta_sec(d4, d2).as_underlying_type() );
  
  auto df34 = delta_sec(d3, d4); // 11 - 12 microseconds
  std::cout<<"\nDifference: "<<date3_str<<" - "<<date4_str<<" = "<<df34.as_underlying_type();
  assert( df34 == microseconds{11-12} );
  assert( df34.as_underlying_type() == -delta_sec(d4, d3).as_underlying_type() );

  d1.add(modified_julian_day{1}, seconds{123});
  df12 = delta_sec(d1, d2); // 1 day + (123000 - 1) milliseconds
  // assert( df12 == milliseconds{123000 - 1 + milliseconds::max_in_day} );
  std::cout<<"\ndiff = "<<df12.as_underlying_type();
  df13 = delta_sec(d1, d3); // 1day + (123000000 - 11) microseconds
  std::cout<<"\ndiff = "<<df13.as_underlying_type();
  // assert( df13 == microseconds{123000000 - 11 + microseconds::max_in_day} );
  df14 = delta_sec(d1, d4); // 1day + (123000000 - 12) microseconds
  std::cout<<"\ndiff = "<<df14.as_underlying_type();
  // assert( df14 == microseconds{123000000-12 + microseconds::max_in_day} );

  std::cout<<"\n";
}
