#include "dtdatetime.hpp"
#include <cassert>
#include <random>

/* test datetime_interval
 * create random dates (e.g. d1 and d2) and check if their interval behaves
 * as expected (e.g. d=d2-d1; d1+d==d2)
 */

using namespace dso;

constexpr const long num_tests = 1'000'000;
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
    /* do we have a valid date ? */
    try {
      /* random date  d1 */
      d1 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      /* random date d2 */
      d2 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      /* d3 on same day as d2 */
      d3 = datetime<nsec>{d2.imjd(), nsec(nsdstr(gen))};
      ok = 1;
    } catch (std::exception &) {
      /* failed to create some date, no worries, retry */
      ok = 0;
    }

    /* dates successefully created; keep on ... */
    if (ok) {
      const auto interval = d2 - d1;
      auto d = d1 + interval;
      assert(d == d2);

      const auto d32 = d3 - d2;
      d = d2 + d32;
      assert(d == d3);

      ++testnr;
    }
  }

  return 0;
}
