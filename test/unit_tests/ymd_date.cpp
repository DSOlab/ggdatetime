#include "dtfund.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::day_of_month;
using dso::month;
using dso::year;
using dso::ymd_date;

int main() {

  for (int y = 1804; y <= 2400; y++) {
    if (dso::year(y).is_leap()) {
      /* leap year, should have a valid date for YYYY/02/29 */
      ymd_date ymd(year(y), month(2), day_of_month(29));
      assert(ymd.is_valid());
    } else {
      /* non-leap year, should not have a valid date for YYYY/02/29 */
      ymd_date iymd(year(y), month(2), day_of_month(29));
      assert(!(iymd.is_valid()));
      /* ...but should have a valid date for YYYY/02/28 */
      ymd_date vymd(year(y), month(2), day_of_month(28));
      assert(vymd.is_valid());
    }
  }

  return 0;
}
