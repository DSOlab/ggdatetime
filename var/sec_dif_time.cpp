#include <iostream>
#include <chrono>
#include <cassert>
#include <limits>

#include "dtfund.hpp"
#include "dtcalendar.hpp"
#include "datetime_read.hpp"
#include "datetime_write.hpp"

using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::literals::chrono_literals;

double
delta_sec_1(const dso::datetime<dso::seconds>& ref, 
            const dso::datetime<dso::milliseconds>& t)
noexcept
{
  auto dt = dso::delta_sec(ref, t);
  return dt.to_fractional_seconds();
}

double
delta_sec_2(dso::datetime<dso::seconds> ref,
            dso::datetime<dso::milliseconds> t)
noexcept
{
  auto dt = dso::delta_sec(ref, t);
  return dt.to_fractional_seconds();
}

int main()
{
  const char* ref_date_str = "2015/12/30 12:09:30";
  auto ref_date = dso::strptime_ymd_hms<dso::seconds>(ref_date_str);
  
  const char* date_str = "2015/12/30 12:09:30.000";
  auto date = dso::strptime_ymd_hms<dso::milliseconds>(date_str);

  dso::milliseconds MAX {dso::milliseconds::max_in_day};
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);

  time_point<Clock> start1_1 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_1(ref_date, date);
  }
  time_point<Clock> end1_1 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);
  
  time_point<Clock> start2_1 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_2(ref_date, date);
  }
  time_point<Clock> end2_1 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);
  
  time_point<Clock> start1_2 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_1(ref_date, date);
  }
  time_point<Clock> end1_2 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);

  time_point<Clock> start2_2 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_2(ref_date, date);
  }
  time_point<Clock> end2_2 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);
  
  time_point<Clock> start1_3 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_1(ref_date, date);
  }
  time_point<Clock> end1_3 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);

  time_point<Clock> start2_3 = Clock::now();
  for (dso::milliseconds s{1L}; s<MAX; s+=10) {
    date.add_seconds(s);
    delta_sec_2(ref_date, date);
  }
  time_point<Clock> end2_3 = Clock::now();
  std::cout<<"\nDate is now: "<<dso::strftime_ymd_hmfs(date);

  milliseconds diff1_1 = duration_cast<milliseconds>(end1_1 - start1_1);
  milliseconds diff1_2 = duration_cast<milliseconds>(end1_2 - start1_2);
  milliseconds diff1_3 = duration_cast<milliseconds>(end1_3 - start1_3);
  std::cout<<"\nResults using references -> A:"<<diff1_1.count()<<" B: "<<diff1_2.count()<<" C: "<<diff1_3.count()<<" ms ";
  milliseconds diff2_1 = duration_cast<milliseconds>(end2_1 - start2_1);
  milliseconds diff2_2 = duration_cast<milliseconds>(end2_2 - start2_2);
  milliseconds diff2_3 = duration_cast<milliseconds>(end2_3 - start2_3);
  std::cout<<"\nResults using instances  -> A:"<<diff2_1.count()<<" B: "<<diff2_2.count()<<" C: "<<diff2_3.count()<<" ms ";

  std::cout<<"\n";
  return 0;
}
