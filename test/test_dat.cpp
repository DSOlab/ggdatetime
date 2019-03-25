#include <iostream>
#include <cassert>
#include "dcalendar.hpp"

using namespace ngpt;
using ngpt::vector

int main()
{
  std::cout<<"\nTesting function dat, aka TAI-UTC difference (sec)";
  std::cout<<"\n-------------------------------------------------------";
  std::vector<datetime<seconds>> dates;
  dates.push_back(strptime_ymd_hms<seconds>"1972-06-30 00:00:00");
  dates.push_back(strptime_ymd_hms<seconds>"1972-06-30 23:59:30");
  dates.push_back(strptime_ymd_hms<seconds>"1972-07-01 00:00:00");
  dates.push_back(strptime_ymd_hms<seconds>"1979-12-31 00:00:00");
  dates.push_back(strptime_ymd_hms<seconds>"1979-12-31 23:59:59");
  dates.push_back(strptime_ymd_hms<seconds>"1980-01-01 00:00:00");
  dates.push_back(strptime_ymd_hms<seconds>"2016-12-31 00:00:00");
  dates.push_back(strptime_ymd_hms<seconds>"2016-12-31 23-59-59");
  dates.push_back(strptime_ymd_hms<seconds>"2017-01-01 00-00-00");
  
  for (const auto& d : dates) {
    std::cout<<"\n"<<d.stringify() << " leap secs: "<< leap;
  }

  std::cout<<"\n";
  return 0;
}
