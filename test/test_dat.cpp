#include <iostream>
#include <cassert>
#include <vector>
#include "datetime_read.hpp"

using namespace ngpt;

int main()
{
  std::cout<<"\nTesting function dat, aka TAI-UTC difference (sec)";
  std::cout<<"\nThis program will test if the functions:";
  std::cout<<"\n\tngpt::dat(year, day_of_year)";
  std::cout<<"\n\tngpt::dat(datetime<T>)";
  std::cout<<"\n\tngpt::dat(modified_julian_day)";
  std::cout<<"\nall produce identical results; if not, an assertion error will";
  std::cout<<"\nbe thrown. Let's see ...";
  std::cout<<"\n-------------------------------------------------------";
  std::vector<datetime<seconds>> dates;
  dates.push_back(strptime_ymd_hms<seconds>("1972-06-30 00:00:00"));
  dates.push_back(strptime_ymd_hms<seconds>("1972-06-30 23:59:30"));
  dates.push_back(strptime_ymd_hms<seconds>("1972-07-01 00:00:00"));
  dates.push_back(strptime_ymd_hms<seconds>("1979-12-31 00:00:00"));
  dates.push_back(strptime_ymd_hms<seconds>("1979-12-31 23:59:59"));
  dates.push_back(strptime_ymd_hms<seconds>("1980-01-01 00:00:00"));
  dates.push_back(strptime_ymd_hms<seconds>("2016-12-31 00:00:00"));
  dates.push_back(strptime_ymd_hms<seconds>("2016-12-31 23-59-59"));
  dates.push_back(strptime_ymd_hms<seconds>("2017-01-01 00-00-00"));
  
  int leap=0,
      leap_1=0,
      leap_2=0;
  for (const auto& d : dates) {
    leap = dat(d);
    auto ymd = d.as_ymd();
    leap_1 = dat(ymd.__year, ymd.__month);
    leap_2 = dat(d.mjd());
    assert(leap == leap_1 && leap_1 == leap_2);
    std::cout<<"\n"<<d.stringify() << " leap secs: "<< leap;
  }

  std::cout<<"\n-------------------------------------------------------";
  std::cout<<"\nFunctions do produce identical results; for the actual"
           <<"\nleap second values, you'll have to check for yourself!"
           <<"\nEnd of Test\n";
  return 0;
}
