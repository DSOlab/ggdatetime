#include "calendar.hpp"
#include <algorithm>
#include <cassert>
#include <vector>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using dso::day_of_month;
using dso::day_of_year;
using dso::month;
using dso::ydoy_date;
using dso::year;
using dso::ymd_date;

int main() {

  /* note that we can construct an invali date */
  ymd_date d1(year(2023), month(13), day_of_month(56));
  /* however, we can always validate a ymd_date instance */
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
      /* extra checks */
      ymd = ymd_date(year(y), month(1), day_of_month(1));
      assert(ydoy_date(ymd).dy() == day_of_year(1));
      ymd = ymd_date(year(y), month(12), day_of_month(31));
      assert(ymd.is_valid());
      assert(ydoy_date(ymd).dy() == day_of_year(366));
    } else {
      /* non-leap year, should not have a valid date for YYYY/02/29 */
      ymd_date iymd(year(y), month(2), day_of_month(29));
      assert(!(iymd.is_valid()));
      /* transform to Year - Day of Year; since this is an invalid date, the
       * construction should throw!
       */
      try {
        const ydoy_date iydoy(iymd);
        /* shit,it didn't throw*/
        assert(1 == 2);
      } catch (std::exception &) {
        ; /* good, it throwed */
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
