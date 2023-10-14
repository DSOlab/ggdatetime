#include "dtfund.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::hours;

int main() {
  hours h1(2023);
  assert(h1.as_underlying_type() == 2023);

  h1 = 2024;
  assert(h1.as_underlying_type() == 2024);

  /* not allowed */
  // h1 == 2024;
  /* not allowed */
  // h1 + 2024;

  assert(h1 >= hours(2024));
  assert(h1 >= hours(2023));
  assert(h1 <= hours(2024));
  assert(h1 <= hours(2025));
  assert(h1 + hours(1) == hours(2025));
  assert(h1 - hours(1) == hours(2023));
  assert((h1 += hours(1)) == hours(2025));
  assert((h1 -= hours(1)) == hours(2024));
  assert((h1 += 1) == hours(2025));
  assert((h1 -= 1) == hours(2024));
  assert((h1++) == hours(2024));
  assert((++h1) == hours(2026));
  assert((h1--) == hours(2026));
  assert((--h1) == hours(2024));

  return 0;
}
