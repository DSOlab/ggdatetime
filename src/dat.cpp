#include "dtfund.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>

namespace calendar_dat {
/** Dates and Delta(AT)s */
struct change {
  int iyear, month, delat;
  /** Combine year and month to form a date-ordered integer */
  int ordered_int() const noexcept { return 12 * iyear + month; }
};
constexpr const std::array<change, 28> changes = {
    {{1972, 1, 10}, {1972, 7, 11}, {1973, 1, 12}, {1974, 1, 13}, {1975, 1, 14},
     {1976, 1, 15}, {1977, 1, 16}, {1978, 1, 17}, {1979, 1, 18}, {1980, 1, 19},
     {1981, 7, 20}, {1982, 7, 21}, {1983, 7, 22}, {1985, 7, 23}, {1988, 1, 24},
     {1990, 1, 25}, {1991, 1, 26}, {1992, 7, 27}, {1993, 7, 28}, {1994, 7, 29},
     {1996, 1, 30}, {1997, 7, 31}, {1999, 1, 32}, {2006, 1, 33}, {2009, 1, 34},
     {2012, 7, 35}, {2015, 7, 36}, {2017, 1, 37}}};
} /* namespace calendar_dat */

namespace mjd_dat {
/** Dates and Delta(AT)s */
struct change {
  int mjd, delat;
};
constexpr const std::array<change, 28> changes = {
    {{41317L, 10}, {41499L, 11}, {41683L, 12}, {42048L, 13}, {42413L, 14},
     {42778L, 15}, {43144L, 16}, {43509L, 17}, {43874L, 18}, {44239L, 19},
     {44786L, 20}, {45151L, 21}, {45516L, 22}, {46247L, 23}, {47161L, 24},
     {47892L, 25}, {48257L, 26}, {48804L, 27}, {49169L, 28}, {49534L, 29},
     {50083L, 30}, {50630L, 31}, {51179L, 32}, {53736L, 33}, {54832L, 34},
     {56109L, 35}, {57204L, 36}, {57754L, 37}}};
} /* namespace mjd_dat */

int dso::dat(const dso::ymd_date &ymd) noexcept {
  return dso::dat(ymd.yr(), ymd.mn());
}

int dso::dat(dso::year iy, dso::month im) noexcept {
  assert(iy >= dso::year(1972));

  /* Combine year and month to form a date-ordered integer... */
  int m = 12 * iy.as_underlying_type() + im.as_underlying_type();

  /* ...and use it to find the preceding table entry. */
  auto it = std::find_if(
      calendar_dat::changes.rbegin(), calendar_dat::changes.rend(),
      [&](const calendar_dat::change &c) { return m >= c.ordered_int(); });

  /* Get the Delta(AT). */
  return it == (calendar_dat::changes.rend())
             ? (calendar_dat::changes.begin()->delat)
             : (it->delat);
}

int dso::dat(dso::modified_julian_day mjd) noexcept {

  /* find the preceding table entry. */
  auto it = std::find_if(mjd_dat::changes.rbegin(), mjd_dat::changes.rend(),
                         [&](const mjd_dat::change &c) {
                           return mjd >= dso::modified_julian_day(c.mjd);
                         });

  /* Get the Delta(AT). */
  return it == (mjd_dat::changes.rend()) ? (mjd_dat::changes.begin()->delat)
                                         : (it->delat);
}

int dso::dat(dso::modified_julian_day mjd, int &extra_sec_in_day) noexcept {
  extra_sec_in_day = 0;

  /* find the preceding table entry. */
  auto it = std::find_if(mjd_dat::changes.rbegin(), mjd_dat::changes.rend(),
                         [&](const mjd_dat::change &c) {
                           return mjd >= dso::modified_julian_day(c.mjd);
                         });

  /* check to see if given MJD is a leap-second day (i.e. has more seconds) */
  if (it != mjd_dat::changes.rend()) {
    /* given MJD is on a leap-insertion date */
    if (mjd == dso::modified_julian_day((it)->mjd-1)) {
      int current_leap = it->delat; printf("\tcurrent leaps: %d\n", current_leap);
      /* leap seconds one day before */
      int prev_leap =
          ((it + 1) == mjd_dat::changes.rend()) ? (0) : ((it + 1)->delat);
        printf("\tprev leaps: %d\n", prev_leap);
      extra_sec_in_day = current_leap - prev_leap;
    }
  }

  /* Get the Delta(AT). */
  return it == (mjd_dat::changes.rend()) ? (mjd_dat::changes.begin()->delat)
                                         : (it->delat);
}
