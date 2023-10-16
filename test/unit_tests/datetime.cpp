#include "dtcalendar.hpp"
#include <cassert>
#include <random>

using namespace dso;

constexpr const long num_tests = 1'000;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;
constexpr const SecIntType TOSEC = nsec::sec_factor<SecIntType>();

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */

  int testnr = 0;
  while (testnr < num_tests) {
    const int iy = ydstr(gen);
    const int im = mdstr(gen);
    const int id = ddstr(gen);
    /* do we have a valid date ? */
    ymd_date ymd{year(iy), month(im), day_of_month(id)};
    if (ymd.is_valid()) {
      /* check constructors YMD vs YDOY */
      const datetime<nsec> d{year(iy), month(im), day_of_month(id)};
      const ydoy_date ydoy(ymd);
      datetime<nsec> d2(year(ydoy.yr()), day_of_year(ydoy.dy()));
      assert(d == d2);
      /* copy date to play with */
      auto dt = d;
      /* check addition */
      dt += datetime_interval<nsec>(0,nsec(1));
      assert(dt > d);
      dt += datetime_interval<nsec>(nsec(-1));
      assert(dt == d);
      dt += datetime_interval<nsec>(nsec(-1));
      assert(dt.imjd() == d.imjd()-modified_julian_day(1));
      for (int j=0; j<86400; j++) {
        dt += datetime_interval<nsec>(0,nsec(1*TOSEC));
        /* reached one nsec before end of day */
      }
      assert(dt.imjd() == d.imjd());
      /* add onw more nsec; we are now on next day */
      dt += datetime_interval<nsec>(0,nsec(1));
      assert(dt.imjd() == d.imjd()+modified_julian_day(1));
      /* and nsec of day should be 0 */
      assert(dt.sec() == nsec(0));

      /* add two whole days and 11 nsec */
      dt = d;
      dt += datetime_interval<nsec>(0,nsec(11 + 2*86400*TOSEC));
      assert(dt.imjd() == d.imjd()+modified_julian_day(2));
      assert(dt.sec() == nsec(11));
      /* or do it this way ... */
      auto dt2 = d;
      dt2 += datetime_interval<nsec>(2,nsec(11));
      assert( dt == dt2 );

      /* remove two whole days and 11 nsec */
      dt = d;
      dt += datetime_interval<nsec>(0,nsec(-(11 + 2*86400*TOSEC)));
      assert(dt.imjd() == d.imjd()-modified_julian_day(3));
      assert(dt.sec() == nsec(nsec::max_in_day - 11));
      /* or do it this way ... */
      dt2 = d;
      dt2 += datetime_interval<nsec>(-2,nsec(11));
      assert( dt == dt2 );

      ++testnr;
    }
  }

  return 0;
}
