#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;

int main() {

  char buffer[64];
  int sz;

  /* check dates */
  datetime<seconds> d1(year(2023), month(10), day_of_month(24), seconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));
  /* check that the same is true for a TwoPartDate */
  TwoPartDate td1(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:00", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), seconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:01", sz));

  d1 = datetime(year(2023), month(10), day_of_month(24), seconds(59));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:00:59", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), seconds(60));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), hours(0),
                         minutes(1), seconds(0));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:00", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), hours(0),
                         minutes(1), seconds(1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:01", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:01", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), seconds(62));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:01:02", sz));

  d1 =
      datetime<seconds>(year(2023), month(10), day_of_month(24), seconds(3600));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 01:00:00", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24),
                         seconds(3600 - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 00:59:59", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), hours(12),
                         minutes(1), seconds(61));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 12:02:01", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24), hours(12),
                         minutes(121), seconds(61));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 14:02:01", sz));

  d1 = datetime<seconds>(year(2023), month(10), day_of_month(24),
                         seconds(86400 - 1));
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars +
       dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars + 1;
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59", sz));
  td1=TwoPartDate(d1);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(td1, buffer);
  assert(!std::strncmp(buffer, "2023/10/24 23:59:59", sz));

  return 0;
}
