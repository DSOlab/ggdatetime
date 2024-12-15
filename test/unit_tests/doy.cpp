#include "calendar.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::day_of_year;

int main() {
  day_of_year y1(2024);
  assert(y1.as_underlying_type() == 2024);

  /* not allowed */
  // y1 = 2024;
  /* not allowed */
  // y1 == 2024;
  /* not allowed */
  // y1 + 2024;

  assert(y1 >= day_of_year(2024));
  assert(y1 >= day_of_year(2023));
  assert(y1 <= day_of_year(2024));
  assert(y1 <= day_of_year(2025));
  assert(y1 + day_of_year(1) == day_of_year(2025));
  assert(y1 - day_of_year(1) == day_of_year(2023));
  assert((y1 += day_of_year(1)) == day_of_year(2025));
  assert((y1 -= day_of_year(1)) == day_of_year(2024));
  // ALLOW_DT_INTEGRAL_MATH
  // assert((y1 += 1) == day_of_year(2025));
  // assert((y1 -= 1) == day_of_year(2024));
  assert((y1++) == day_of_year(2024));
  assert((++y1) == day_of_year(2026));
  assert((y1--) == day_of_year(2026));
  assert((--y1) == day_of_year(2024));

  for (int y = 1804; y <= 2400; y++) {
    if (dso::year(y).is_leap()) {
      /* leap year, should have a valid date for YYYY/366 */
      assert(day_of_year(366).is_valid(dso::year(y)));
    } else {
      /* non-leap year, should not have a valid date for YYYY/366 */
      assert(!(day_of_year(366).is_valid(dso::year(y))));
      /* ...but should have a valid date for YYYY/365 */
      assert(day_of_year(365).is_valid(dso::year(y)));
    }
  }

  return 0;
}
