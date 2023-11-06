#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;
constexpr const long SEC = microseconds::sec_factor<long>();

int main() {

  char buffer[64];
  int sz;

  /* check time */
  hms_time<microseconds> t1(microseconds(1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(59));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(59 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(60 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(SEC - 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<microseconds>(microseconds(SEC + 10));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<microseconds>(microseconds(SEC + SEC - 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<microseconds>(microseconds(59 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:59", sz));

  t1 = hms_time<microseconds>(microseconds(60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:01:00", sz));

  t1 = hms_time<microseconds>(microseconds(59 * 60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:59:00", sz));

  t1 = hms_time<microseconds>(microseconds(60 * 60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "01:00:00", sz));

  t1 = hms_time<microseconds>(microseconds(86400L * SEC - SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<microseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "23:59:59", sz));

  return 0;
}
