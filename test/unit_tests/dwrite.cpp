#include "datetime_write.hpp"
#include <cassert>
#include <cstring>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;

int main() {

  char buffer[64];
  int sz;

  /* check dates */
  ymd_date d1(year(2023), month(10), day_of_month(24));
  to_char<YMDFormat::YYYYMMDD>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars;
  assert(!std::strncmp(buffer, "2023/10/24", sz));

  d1 = ymd_date(year(2023), month(1), day_of_month(4));
  to_char<YMDFormat::YYYYMMDD>(d1, buffer);
  sz = dso::SpitDate<YMDFormat::YYYYMMDD>::numChars;
  assert(!std::strncmp(buffer, "2023/01/04", sz));

  /* check time */
  hms_time<seconds> t1(seconds(1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<seconds>(seconds(59));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:59", sz));

  t1 = hms_time<seconds>(seconds(60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:01:00", sz));

  t1 = hms_time<seconds>(seconds(59 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:59:00", sz));

  t1 = hms_time<seconds>(seconds(60 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "01:00:00", sz));

  t1 = hms_time<seconds>(seconds(60 * 60 + 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "01:00:01", sz));

  t1 = hms_time<seconds>(seconds(86400 - 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<seconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "23:59:59", sz));

  return 0;
}
