#include "dtfund.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::day_of_month;
using dso::day_of_year;
using dso::month;
using dso::ydoy_date;
using dso::year;
using dso::ymd_date;

int main() {

  /* note that we can construct an invali date */
  ydoy_date d1(year(2023), day_of_year(500));
  /* however, we can always validate a ydoy_date instance */
  assert(!(d1.is_valid()));

  /* note also that days of year start at 1 (not 0) */
  d1 = ydoy_date(year(2023), day_of_year(0));
  assert(!(d1.is_valid()));

  for (int y = 1804; y <= 2400; y++) {
    if (dso::year(y).is_leap()) {
      /* leap year, should have a valid date for YYYY/02/29 */
      ymd_date ymd(year(y), month(2), day_of_month(29));
      assert(ymd.is_valid());
      /* transform to Year - Day of Year */
      const ydoy_date ydoy(ymd);
      assert((ydoy.yr() == year(y)) && (ydoy.dy() == day_of_year(31 + 29)));
      assert(ydoy.is_valid());
      /* transform back to year/month/day of month */
      const ymd_date d2(ydoy);
      assert(d2 == ymd);
      /* extra checks */
      ymd = ymd_date(year(y), month(1), day_of_month(1));
      assert(ydoy_date(ymd).dy() == day_of_year(1));
      assert(ymd_date(ydoy_date(ymd)) == ymd);
      ymd = ymd_date(year(y), month(12), day_of_month(31));
      assert(ymd.is_valid());
      assert(ydoy_date(ymd).dy() == day_of_year(366));
      assert(ymd_date(ydoy_date(ymd)) == ymd);
    } else {
      /* non-leap year, should not have a valid date for YYYY/02/29 but we
       * should be able to create an invalid ydoy_date
       */
      ydoy_date iydoy(year(y), day_of_year(366));
      /* which should be invalid */
      assert(!(iydoy.is_valid()));
      /* and if we cast it to an ymd_date, it should throw */
      try {
        ymd_date d5(iydoy);
        assert(1 == 2);
      } catch (std::exception &) {
        ;
      }

      /* ...but should have a valid date for YYYY/02/28 */
      ymd_date vymd(year(y), month(2), day_of_month(28));
      assert(vymd.is_valid());
      /* transform to Year - Day of Year */
      const ydoy_date vydoy(vymd);
      assert((vydoy.yr() == year(y)) && (vydoy.dy() == day_of_year(31 + 28)));
      assert(vydoy.is_valid());
    }
  }

  return 0;
}
