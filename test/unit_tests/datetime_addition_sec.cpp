#include "dtdatetime.hpp"
#include <cassert>
#include <random>

using namespace dso;

constexpr const long num_tests = 5'000;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, nsec::max_in_day); /* range for day of month */

  int testnr = 0, ok;
  datetime<nsec> d1, d2, d3;
  while (testnr < num_tests) {
    try {
      /* random date */
      d1 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      /* one day next */
      d2 = d1 + datetime_interval<nsec>(1, nsec(0));
      /* one and a half day next */
      d3 = d1 + datetime_interval<nsec>(
                    1, nsec((86400L / 2) * nsec::sec_factor<long>()));
      ok = 1;
    } catch (std::exception &) {
      /* no worries, will retry */
      ok = 0;
    }
    if (ok) {

      /* add seconds to nanoseconds */
      auto dt = d1;
      for (int i = 0; i < 86400; i++)
        dt.add_seconds(seconds(1));
      assert(dt == d2);

      for (int i = 0; i < 86400; i++)
        dt.add_seconds(seconds(-1));
      assert(dt == d1);

      /* add seconds to nanoseconds */
      dt = d1;
      for (int i = 0; i < 86400 + 86400 / 2; i++)
        dt.add_seconds(seconds(1));

      assert(dt == d3);
      for (int i = 0; i < 86400 + 86400 / 2; i++)
        dt.add_seconds(seconds(-1));
      assert(dt == d1);

      ++testnr;
    }
  }

  return 0;
}
