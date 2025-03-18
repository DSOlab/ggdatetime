#include "dtdatetime.hpp"
#include <cassert>
#include <random>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

/* test adding and subtracting seconds of different type */

using namespace dso;

constexpr const long num_tests = 5'000;
using msec = dso::milliseconds;
typedef msec::underlying_type SecIntType;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, msec::max_in_day); /* range for day of month */

  int testnr = 0, ok;
  datetime<msec> d1, d2, d3;
  while (testnr < num_tests) {
    try {
      /* random date d1 */
      d1 = datetime<msec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), msec(nsdstr(gen))};
      /* one day next */
      d2 = d1 + datetime_interval<msec>(1, msec(0));
      /* one and a half day next */
      d3 = d1 + datetime_interval<msec>(
                    1, msec((86400L / 2) * msec::sec_factor<long>()));
      ok = 1;
    } catch (std::exception &) {
      /* failed to create some datetime; no worries, retry */
      ok = 0;
    }

    if (ok) {

      /* add nanoseconds to milliseconds */
      auto dt = d1;
      for (int i = 0; i < 86400; i++) {
        /* 1'000'123'456 is 1'000 milliseconds */
        dt.add_seconds(nanoseconds(1'000'123'456));
      }
      assert(dt == d2);

      dt = d2;
      for (int i = 0; i < 86400; i++) {
        dt.add_seconds(nanoseconds(-1'000'123'456));
      }
      assert(dt == d1);

      /* add seconds to nanoseconds */
      dt = d1;
      for (int i = 0; i < 86400 + 86400 / 2; i++)
        dt.add_seconds(nanoseconds(1'000'789'456));
      assert(dt == d3);

      for (int i = 0; i < 86400 + 86400 / 2; i++)
        dt.add_seconds(nanoseconds(-1'000'123'456));
      assert(dt == d1);

      ++testnr;
    }
  }

  return 0;
}
