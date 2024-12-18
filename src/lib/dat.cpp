#include "date_integral_types.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>

namespace {
/** just so we do not have magic numbers */
constexpr const int MONTHS_IN_YEAR = 12;
} /* unnamed namespace */

namespace calendar_dat {
/** Dates and Delta(AT)s */
struct change {
  int iyear, month, delat;
  /** Combine year and month to form a date-ordered integer */
  [[nodiscard]] int ordered_int() const noexcept {
    return MONTHS_IN_YEAR * iyear + month;
  }
};
constexpr const std::array<change, 28> changes = {
    {{1972, 1, 10}, {1972, 7, 11}, {1973, 1, 12}, {1974, 1, 13}, {1975, 1, 14},
     {1976, 1, 15}, {1977, 1, 16}, {1978, 1, 17}, {1979, 1, 18}, {1980, 1, 19},
     {1981, 7, 20}, {1982, 7, 21}, {1983, 7, 22}, {1985, 7, 23}, {1988, 1, 24},
     {1990, 1, 25}, {1991, 1, 26}, {1992, 7, 27}, {1993, 7, 28}, {1994, 7, 29},
     {1996, 1, 30}, {1997, 7, 31}, {1999, 1, 32}, {2006, 1, 33}, {2009, 1, 34},
     {2012, 7, 35}, {2015, 7, 36}, {2017, 1, 37}}};
static_assert(changes.size() == dso::TOTAL_LEAP_SEC_INSERTION_DATES,
              "Invalid number of leap second insertion days!");
} /* namespace calendar_dat */

namespace mjd_dat {
/** Dates and Delta(AT)s */
struct change {
  int mjd, delat;
};
/** @brief MJDs where the leap seconds have changed. */
constexpr const std::array<change, 28> changes = {
    {{41317L, 10}, {41499L, 11}, {41683L, 12}, {42048L, 13}, {42413L, 14},
     {42778L, 15}, {43144L, 16}, {43509L, 17}, {43874L, 18}, {44239L, 19},
     {44786L, 20}, {45151L, 21}, {45516L, 22}, {46247L, 23}, {47161L, 24},
     {47892L, 25}, {48257L, 26}, {48804L, 27}, {49169L, 28}, {49534L, 29},
     {50083L, 30}, {50630L, 31}, {51179L, 32}, {53736L, 33}, {54832L, 34},
     {56109L, 35}, {57204L, 36}, {57754L, 37}}};
static_assert(changes.size() == dso::TOTAL_LEAP_SEC_INSERTION_DATES,
              "Invalid number of leap second insertion days!");
} /* namespace mjd_dat */

int dso::dat(const dso::ymd_date &ymd) noexcept {
  return dso::dat(ymd.yr(), ymd.mn());
}

int dso::dat(dso::year iy, dso::month im) noexcept {
  assert(iy >= dso::year(1972));

  /* Combine year and month to form a date-ordered integer... */
  int m = MONTHS_IN_YEAR * iy.as_underlying_type() + im.as_underlying_type();

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
  assert(mjd >= modified_julian_day(41317));
  extra_sec_in_day = 0;

  /* find the preceding table entry. */
  auto it = std::find_if(mjd_dat::changes.rbegin(), mjd_dat::changes.rend(),
                         [&](const mjd_dat::change &c) {
                           return mjd >= dso::modified_julian_day(c.mjd);
                         });

  /* extra seconds in day */
  extra_sec_in_day = 0;

  /* check to see if given MJD is a leap-second day (i.e. has more seconds) */
  if (it != mjd_dat::changes.rend() && it != mjd_dat::changes.rbegin()) {
    /* given MJD is on a leap-insertion date (i.e. day prior to next leap) */
    if (mjd == dso::modified_julian_day((it - 1)->mjd - 1)) {
      /* DAT at next period */
      int next_leap = (it - 1)->delat;
      extra_sec_in_day = next_leap - it->delat;
    }
  }

  /* Get the Delta(AT). */
  return it->delat;
}
