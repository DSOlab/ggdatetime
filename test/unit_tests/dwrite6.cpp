#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;
constexpr const long SEC = milliseconds::sec_factor<long>();

int main() {

  char buffer[64];
  int sz;

  /* check dates */
  datetime<milliseconds> d1(year(2023), month(10), day_of_month(24),
                            milliseconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  /* check that the same is true for a TwoPartDate */
  TwoPartDate td1(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), milliseconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), milliseconds(59));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(60));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(SEC + 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  
  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(2*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  
  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(5*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:04", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:04", sz));
  
  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(59*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:58", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:58", sz));
  
  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(59*SEC + SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59", sz));
  
  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(59*SEC + SEC + 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24), hours(0),
                              minutes(1), milliseconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24), hours(0),
                              minutes(1), milliseconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(62 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(3600 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(SEC * (3600 - 1)));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              hours(12), minutes(1), milliseconds(61 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              hours(12), minutes(121), milliseconds(61 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01", sz));

  d1 = datetime<milliseconds>(year(2023), month(10), day_of_month(24),
                              milliseconds(SEC * (86400 - 1)));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59", sz));

  return 0;
}
