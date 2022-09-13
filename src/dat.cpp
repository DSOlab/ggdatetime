/// @file  dat.cpp
/// @brief Implementation file for computation of UTC-TAI, i.e. leap seconds.
/// This file contains the implementation of dat function(s), to compute the
/// number of leap seconds in a UTC date/time.
/// @see SOFA Software Collection (iauDat), http://www.iausofa.org/

#include "dtfund.hpp"

/// If the specified date is for a day which ends with a leap second,
/// the UTC-TAI value returned is for the period leading up to the
/// leap second.  If the date is for a day which begins as a leap
/// second ends, the UTC-TAI returned is for the period following the
/// leap second.
/// This version works for post 01.01.1972 dates. The ipnut date will only be
/// checked if USE_DATETIME_CHECKS flag is enabled during compilation.
///
/// The day of month is actually not needed, since all leap second insertions
/// happen at the begining, i.e. the first day of a month.
int dso::dat(dso::year iy, dso::month im) noexcept {
#ifdef USE_DATETIME_CHECKS
  assert(iy >= dso::year(1972));
#endif

  // Dates and Delta(AT)s
  constexpr struct {
    int iyear, month, delat;
  } changes[] = {{1972, 1, 10}, {1972, 7, 11}, {1973, 1, 12}, {1974, 1, 13},
                 {1975, 1, 14}, {1976, 1, 15}, {1977, 1, 16}, {1978, 1, 17},
                 {1979, 1, 18}, {1980, 1, 19}, {1981, 7, 20}, {1982, 7, 21},
                 {1983, 7, 22}, {1985, 7, 23}, {1988, 1, 24}, {1990, 1, 25},
                 {1991, 1, 26}, {1992, 7, 27}, {1993, 7, 28}, {1994, 7, 29},
                 {1996, 1, 30}, {1997, 7, 31}, {1999, 1, 32}, {2006, 1, 33},
                 {2009, 1, 34}, {2012, 7, 35}, {2015, 7, 36}, {2017, 1, 37}};

  // Number of Delta(AT) changes
  constexpr int NDAT{(int)(sizeof changes / sizeof changes[0])};

  // Combine year and month to form a date-ordered integer...
  int m = 12 * iy.as_underlying_type() + im.as_underlying_type();
  // ...and use it to find the preceding table entry.
  int idx = NDAT - 1;
  for (; idx >= 0; idx--) {
    if (m >= (12 * changes[idx].iyear + changes[idx].month))
      break;
  }

  // Get the Delta(AT).
  return changes[idx].delat;
}

///
/// If the specified date is for a day which ends with a leap second,
/// the UTC-TAI value returned is for the period leading up to the
/// leap second.  If the date is for a day which begins as a leap
/// second ends, the UTC-TAI returned is for the period following the
/// leap second.
///
/// The day of month is actually not needed, since all leap second insertions
/// happen at the begining, i.e. the first day of a month.
///
int dso::dat(dso::modified_julian_day mjd) noexcept {

  // Dates and Delta(AT)s
  constexpr struct {
    dso::modified_julian_day::underlying_type mjday;
    int delat;
  } changes[] = {{41317L, 10}, {41499L, 11}, {41683L, 12}, {42048L, 13},
                 {42413L, 14}, {42778L, 15}, {43144L, 16}, {43509L, 17},
                 {43874L, 18}, {44239L, 19}, {44786L, 20}, {45151L, 21},
                 {45516L, 22}, {46247L, 23}, {47161L, 24}, {47892L, 25},
                 {48257L, 26}, {48804L, 27}, {49169L, 28}, {49534L, 29},
                 {50083L, 30}, {50630L, 31}, {51179L, 32}, {53736L, 33},
                 {54832L, 34}, {56109L, 35}, {57204L, 36}, {57754L, 37}};

  // Number of Delta(AT) changes
  constexpr int NDAT{(int)(sizeof changes / sizeof changes[0])};

  // find the preceding table entry.
  const dso::modified_julian_day::underlying_type today{
      mjd.as_underlying_type()};
  int idx = NDAT - 1;
  for (; idx >= 0; idx--) {
    if (today >= changes[idx].mjday)
      break;
  }

  // Get the Delta(AT).
  return changes[idx].delat;
}

int dso::dat(dso::modified_julian_day mjd, int &extra_sec_in_day) noexcept {
  extra_sec_in_day = 0;

  // Dates and Delta(AT)s
  constexpr struct {
    dso::modified_julian_day::underlying_type mjday;
    int delat;
  } changes[] = {{41317L, 10}, {41499L, 11}, {41683L, 12}, {42048L, 13},
                 {42413L, 14}, {42778L, 15}, {43144L, 16}, {43509L, 17},
                 {43874L, 18}, {44239L, 19}, {44786L, 20}, {45151L, 21},
                 {45516L, 22}, {46247L, 23}, {47161L, 24}, {47892L, 25},
                 {48257L, 26}, {48804L, 27}, {49169L, 28}, {49534L, 29},
                 {50083L, 30}, {50630L, 31}, {51179L, 32}, {53736L, 33},
                 {54832L, 34}, {56109L, 35}, {57204L, 36}, {57754L, 37}};

  // Number of Delta(AT) changes
  constexpr int NDAT{(int)(sizeof changes / sizeof changes[0])};

  // find the preceding table entry.
  const dso::modified_julian_day::underlying_type today{
      mjd.as_underlying_type()};
  int idx = NDAT - 1;
  int next_leap = changes[idx].delat;
  for (; idx >= 0; idx--) {
    if (today >= changes[idx].mjday) {
      if (idx < NDAT - 1) {
        next_leap = changes[idx + 1].delat;
        extra_sec_in_day = (today == changes[idx + 1].mjday - 1)
                               ? (next_leap - changes[idx].delat)
                               : 0;
      }
      break;
    }
  }

  // Get the Delta(AT).
  return changes[idx].delat;
}