#include "calendar.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::day_of_month;

int main() {
  day_of_month y1(2023);
  assert(y1.as_underlying_type() == 2023);

  /* not allowed */
  // y1 = 2024;
  /* not allowed */
  // y1 == 2024;
  /* not allowed */
  // y1 + 2024;

  assert(y1 >= day_of_month(2024));
  assert(y1 >= day_of_month(2023));
  assert(y1 <= day_of_month(2024));
  assert(y1 <= day_of_month(2025));
  assert(y1 + day_of_month(1) == day_of_month(2025));
  assert(y1 - day_of_month(1) == day_of_month(2023));
  assert((y1 += day_of_month(1)) == day_of_month(2025));
  assert((y1 -= day_of_month(1)) == day_of_month(2024));
  // only allowed with ALLOW_DT_INTEGRAL_MATH
  // assert((y1 += 1) == day_of_month(2025));
  // assert((y1 -= 1) == day_of_month(2024));
  assert((y1++) == day_of_month(2024));
  assert((++y1) == day_of_month(2026));
  assert((y1--) == day_of_month(2026));
  assert((--y1) == day_of_month(2024));

  for (int y = 1804; y <= 2400; y++) {
    if (dso::year(y).is_leap()) {
      /* leap year, should have a valid date for YYYY/02/29 */
      assert(day_of_month(29).is_valid(dso::year(y), dso::month(2)));
    } else {
      /* non-leap year, should not have a valid date for YYYY/02/29 */
      assert(!(day_of_month(29).is_valid(dso::year(y), dso::month(2))));
      /* ...but should have a valid date for YYYY/02/28 */
      assert(day_of_month(28).is_valid(dso::year(y), dso::month(2)));
    }
  }

  return 0;
}
