#include "datetime_write.hpp"
#include <cassert>
#include <cstring>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;
constexpr const long SEC = microseconds::sec_factor<long>();

int main() {

  constexpr hms_time my_time(hours(1), minutes(30), microseconds(45));
  static_assert(my_time.mn().as_underlying_type() == 30,
                "Not evaluated as constexpr");

  char buffer[64];
  int sz;

  {
    /* check time */
    hms_time<microseconds> t1(microseconds(1));
    to_char<HMSFormat::ISECDAY>(t1, buffer);
    sz = dso::SpitTime<microseconds, HMSFormat::ISECDAY>::numChars;
    assert(!std::strncmp(buffer, "    0", sz));
  }

  {
    /* check time */
    hms_time<microseconds> t1(microseconds(1));
    to_char<HMSFormat::FSECDAY>(t1, buffer);
    sz = dso::SpitTime<microseconds, HMSFormat::FSECDAY>::numChars;
    assert(!std::strncmp(buffer, "    0.000001000", sz));
  }

  return 0;
}
