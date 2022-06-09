#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso; 

int main() {
  char buf[64];

  datetime<seconds> d1(year(2021), month(1), day_of_month(1), hours(0),
                       minutes(0), seconds(1));
  strftime_ymd_hmfs(d1, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:01.000000000"));

  datetime<seconds> d2(year(2021), month(1), day_of_month(1), hours(0),
                       minutes(61), seconds(1));
  strftime_ymd_hmfs(d2, buf);
  assert(!std::strcmp(buf, "2021-01-01 01:01:01.000000000"));

  datetime<seconds> d3(year(2021), month(1), day_of_month(1), hours(0),
                       minutes(61), seconds(61));
  strftime_ymd_hmfs(d3, buf);
  assert(!std::strcmp(buf, "2021-01-01 01:02:01.000000000"));

  datetime<milliseconds> e1(year(2021), month(1), day_of_month(1), hours(0),
                            minutes(0), milliseconds(1));
  strftime_ymd_hmfs(e1, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:00.001000000"));
  
  datetime<milliseconds> e2(year(2021), month(1), day_of_month(1), hours(0),
                            minutes(0), milliseconds(1001));
  strftime_ymd_hmfs(e2, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:01.001000000"));

  datetime<microseconds> f1(year(2021), month(1), day_of_month(1), hours(0),
                            minutes(0), microseconds(1));
  strftime_ymd_hmfs(f1, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:00.000001000"));
  
  datetime<microseconds> f2(year(2021), month(1), day_of_month(1), hours(0),
                            minutes(0), microseconds(1001));
  strftime_ymd_hmfs(f2, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:00.001001000"));
  
  datetime<microseconds> f3(year(2021), month(1), day_of_month(1), hours(0),
                            minutes(0), microseconds(1'000'000 + 1));
  strftime_ymd_hmfs(f3, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:01.000001000"));

  datetime<nanoseconds> g1(year(2021), month(1), day_of_month(1), hours(0),
                           minutes(0), nanoseconds(1));
  strftime_ymd_hmfs(g1, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:00.000000001"));
  
  datetime<nanoseconds> g2(year(2021), month(1), day_of_month(1), hours(0),
                           minutes(0), nanoseconds(1001));
  strftime_ymd_hmfs(g2, buf);
  assert(!std::strcmp(buf, "2021-01-01 00:00:00.000001001"));

  return 0;
}
