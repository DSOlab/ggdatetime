#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;
constexpr const long SEC = nanoseconds::sec_factor<long>();

int main() {

  char buffer[64];
  int sz;

  /* check dates */
  datetime<nanoseconds> d1(year(2023), month(10), day_of_month(24),
                            nanoseconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000000", sz));
  /* check that the same is true for a TwoPartDate */
  TwoPartDate td1(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000000", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), nanoseconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000001", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000001", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), nanoseconds(59));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000059", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000059", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(60));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000060", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.000000060", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00.999999999", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(SEC + 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.000000001", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.000000001", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(2*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01.999999999", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(5*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:04.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:04.999999999", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(59*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:58.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:58.999999999", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(60*SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59.999999999", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(60*SEC + 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000001", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000001", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), hours(0),
                              minutes(1), nanoseconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), hours(0),
                              minutes(1), nanoseconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000001", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00.000000001", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(62 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(3600 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(SEC * 3600 - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59.999999999", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              hours(12), minutes(1), nanoseconds(61 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              hours(12), minutes(121), nanoseconds(61 * SEC));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01.000000000", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01.000000000", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                              nanoseconds(SEC * 86400 - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<nanoseconds, HMSFormat::HHMMSSF>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59.999999999", sz));
  td1 = TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59.999999999", sz));

  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                             nanoseconds(0));
  for (long i = 0; i < 86400L-1; i++)
    d1 += datetime_interval<nanoseconds>(nanoseconds(SEC));
  d1 += datetime_interval<nanoseconds>(nanoseconds(SEC - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59.999999999", sz));
  
  d1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24),
                             nanoseconds(0));
  td1 = TwoPartDate(d1);
  for (long i = 0; i < 86400L-1; i++)
    td1.add_seconds(1e0);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  printf("Expected %s\n", "2023/10/24 23:59:59.000000000");
  printf("Got (1)  %.29s\n", buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59.000000000", sz));
  td1.add_seconds(.999999999e0);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59.999999999", sz));

  return 0;
}
