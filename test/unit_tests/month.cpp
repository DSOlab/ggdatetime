#include "calendar.hpp"
#include <algorithm>
#include <cassert>
#include <vector>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using dso::month;

int main() {
  month m1(2023);
  assert(m1.as_underlying_type() == 2023);

  m1 = month(2024);
  assert(m1.as_underlying_type() == 2024);

  /* not allowed */
  // m1 == 2024;
  /* not allowed */
  // m1 + 2024;

  assert(m1 >= month(2024));
  assert(m1 >= month(2023));
  assert(m1 <= month(2024));
  assert(m1 <= month(2025));
  assert(m1 + month(1) == month(2025));
  assert(m1 - month(1) == month(2023));
  assert((m1 += month(1)) == month(2025));
  assert((m1 -= month(1)) == month(2024));
  // only allowed with ALLOW_DT_INTEGRAL_MATH
  // assert((m1 += 1) == month(2025));
  // assert((m1 -= 1) == month(2024));
  assert((m1++) == month(2024));
  assert((++m1) == month(2026));
  assert((m1--) == month(2026));
  assert((--m1) == month(2024));

  for (int i = -100; i < 100; i++)
    assert(month(i).is_valid() || (i < 1 || i > 12));

  assert(month("jan") == month(1));
  assert(month("feb") == month(2));
  assert(month("Mar") == month(3));
  assert(month("APR") == month(4));
  assert(month("mAy") == month(5));
  assert(month("juN") == month(6));
  assert(month("jul") == month(7));
  assert(month("AUG") == month(8));
  assert(month("sep") == month(9));
  assert(month("oct") == month(10));
  assert(month("nov") == month(11));
  assert(month("dec") == month(12));

  assert(month("january") == month(1));
  assert(month("februarY") == month(2));
  assert(month("March") == month(3));
  assert(month("APRil") == month(4));
  assert(month("mAy") == month(5));
  assert(month("juNe") == month(6));
  assert(month("july") == month(7));
  assert(month("AUGUST") == month(8));
  assert(month("septembEr") == month(9));
  try {
    /* spelled wrong! should throw! */
    assert(month("octocber") == month(10));
    assert(1 == 0);
  } catch (std::exception &) {
    ;
  }
  assert(month("october") == month(10));
  assert(month("november") == month(11));
  assert(month("decEMBER") == month(12));

  return 0;
}
