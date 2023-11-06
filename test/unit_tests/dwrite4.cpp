#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;
constexpr const long SEC = milliseconds::sec_factor<long>();

int main() {

  char buffer[64];
  int sz;

  /* check time */
  hms_time<milliseconds> t1(milliseconds(1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(59));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(59 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:03", sz));

  t1 = hms_time<milliseconds>(milliseconds(60 * 60));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:03", sz));

  t1 = hms_time<milliseconds>(milliseconds(SEC - 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<milliseconds>(milliseconds(SEC + 10));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<milliseconds>(milliseconds(SEC + SEC - 1));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:01", sz));

  t1 = hms_time<milliseconds>(milliseconds(59 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:00:59", sz));

  t1 = hms_time<milliseconds>(milliseconds(60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:01:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(59 * 60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "00:59:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(60 * 60 * SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "01:00:00", sz));

  t1 = hms_time<milliseconds>(milliseconds(86400L * SEC - SEC));
  to_char<HMSFormat::HHMMSS>(t1, buffer);
  sz = dso::SpitTime<milliseconds, HMSFormat::HHMMSS>::numChars;
  assert(!std::strncmp(buffer, "23:59:59", sz));

  return 0;
}
