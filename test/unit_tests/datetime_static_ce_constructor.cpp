#include "calendar.hpp"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

int main() {

  constexpr const auto j2000 = dso::datetime<dso::seconds>::j2000_mjd();
  static_assert(j2000.imjd() == dso::modified_julian_day(51544));
  static_assert(j2000.sec() == dso::seconds(86400 / 2));
  return 0;
}
