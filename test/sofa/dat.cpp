#include "dtfund.hpp"
#include "sofa.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <random>

using dso::day_of_month;
using dso::month;
using dso::year;
using dso::ymd_date;

const std::array<ymd_date, 27> leap_insertion_dates = {
    ymd_date{year(1972), month(6), day_of_month(30)},
    ymd_date{year(1972), month(12), day_of_month(31)},
    ymd_date{year(1973), month(12), day_of_month(31)},
    ymd_date{year(1974), month(12), day_of_month(31)},
    ymd_date{year(1975), month(12), day_of_month(31)},
    ymd_date{year(1976), month(12), day_of_month(31)},
    ymd_date{year(1977), month(12), day_of_month(31)},
    ymd_date{year(1978), month(12), day_of_month(31)},
    ymd_date{year(1979), month(12), day_of_month(31)},
    ymd_date{year(1981), month(6), day_of_month(30)},
    ymd_date{year(1982), month(6), day_of_month(30)},
    ymd_date{year(1983), month(6), day_of_month(30)},
    ymd_date{year(1985), month(6), day_of_month(30)},
    ymd_date{year(1987), month(12), day_of_month(31)},
    ymd_date{year(1989), month(12), day_of_month(31)},
    ymd_date{year(1990), month(12), day_of_month(31)},
    ymd_date{year(1992), month(6), day_of_month(30)},
    ymd_date{year(1993), month(6), day_of_month(30)},
    ymd_date{year(1994), month(6), day_of_month(30)},
    ymd_date{year(1995), month(12), day_of_month(31)},
    ymd_date{year(1997), month(6), day_of_month(30)},
    ymd_date{year(1998), month(12), day_of_month(31)},
    ymd_date{year(2005), month(12), day_of_month(31)},
    ymd_date{year(2008), month(12), day_of_month(31)},
    ymd_date{year(2012), month(6), day_of_month(30)},
    ymd_date{year(2015), month(6), day_of_month(30)},
    ymd_date{year(2016), month(12), day_of_month(31)},
};

bool is_leap_insertion_day(const ymd_date &ymd) noexcept {
  auto it =
      std::find_if(leap_insertion_dates.begin(), leap_insertion_dates.end(),
                   [&](const ymd_date &d) { return d == ymd; });
  return it != leap_insertion_dates.end();
}

/* number of tests to perform (pre template parameter) */
const long num_tests = 5'000'000;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */

  double jd0=0, jd1=0;
  for (long i = 0; i < num_tests; i++) {
    const int iy = ydstr(gen);
    const int im = mdstr(gen);
    const int id = ddstr(gen);
    double deltat;
    ymd_date ymd{dso::year(iy), dso::month(im), dso::day_of_month(id)};
    if (!ymd.is_valid()) {
      /* erronuous date, SOFA should verify that! */
      assert(iauCal2jd(iy, im, id, &jd0, &jd1) < 0);
    } else {
      /* get DAT from SOFA for 0hours in day */
      assert(iauDat(iy, im, id, 0e0, &deltat) >= 0);
      /* get DAT from libgdatetime */
      int dat = dso::dat(ymd);
      assert(deltat == (double)dat);
    }
  }

  int extra;
  int leap_seconds_found = 0;

  /* leap second years in range [1972 to now] */
  const std::vector<int> leap = {2016, 2015, 2012, 2008, 2005, 1998, 1997,
                                 1995, 1994, 1993, 1992, 1990, 1989, 1987,
                                 1985, 1983, 1982, 1981, 1979, 1978, 1977,
                                 1976, 1975, 1974, 1973, 1972};
  for (const auto y : leap) {
    double deltat;
    /* 29/06/YYYY - no insertion of leap second */
    ymd_date ymd{dso::year(y), dso::month(6), dso::day_of_month(29)};
    assert(iauDat(ymd.yr().as_underlying_type(), ymd.mn().as_underlying_type(),
                  ymd.dm().as_underlying_type(), 0e0, &deltat) >= 0);
    assert(deltat == (double)dso::dat(ymd));
    /* transform to MJD */
    assert(iauCal2jd(ymd.yr().as_underlying_type(),
                     ymd.mn().as_underlying_type(),
                     ymd.dm().as_underlying_type(), &jd0, &jd1) == 0);
    assert(jd0 == dso::MJD0_JD);
    /* compute DAT using the MJD */
    auto mjd = dso::modified_julian_day((long)jd1);
    assert(deltat == (double)dso::dat(mjd));
    dso::dat(mjd, extra);
    if (extra) {
      ++leap_seconds_found;
      assert(is_leap_insertion_day(ymd));
    }

    /* 30/06/YYYY - insertion of leap second could happen ! */
    ymd = ymd_date(dso::year(y), dso::month(6), dso::day_of_month(30));
    assert(iauDat(ymd.yr().as_underlying_type(), ymd.mn().as_underlying_type(),
                  ymd.dm().as_underlying_type(), 0e0, &deltat) >= 0);
    assert(deltat == (double)dso::dat(ymd));
    assert(iauCal2jd(ymd.yr().as_underlying_type(),
                     ymd.mn().as_underlying_type(),
                     ymd.dm().as_underlying_type(), &jd0, &jd1) == 0);
    assert(jd0 == dso::MJD0_JD);
    mjd = dso::modified_julian_day((long)jd1);
    assert(deltat == (double)dso::dat(mjd));
    dso::dat(mjd, extra);
    if (extra) {
      ++leap_seconds_found;
      assert(is_leap_insertion_day(ymd));
    }

    /* 01/07/YYYY - no insertion of leap second */
    ymd = ymd_date(dso::year(y), dso::month(7), dso::day_of_month(1));
    assert(iauDat(ymd.yr().as_underlying_type(), ymd.mn().as_underlying_type(),
                  ymd.dm().as_underlying_type(), 0e0, &deltat) >= 0);
    assert(deltat == (double)dso::dat(ymd));
    assert(iauCal2jd(ymd.yr().as_underlying_type(),
                     ymd.mn().as_underlying_type(),
                     ymd.dm().as_underlying_type(), &jd0, &jd1) == 0);
    assert(jd0 == dso::MJD0_JD);
    mjd = dso::modified_julian_day((long)jd1);
    assert(deltat == (double)dso::dat(mjd));
    dso::dat(mjd, extra);
    if (extra) {
      ++leap_seconds_found;
      assert(is_leap_insertion_day(ymd));
    }

    /* 01/01/YYYY - no insertion of leap second */
    ymd = ymd_date(dso::year(y), dso::month(1), dso::day_of_month(1));
    assert(iauDat(ymd.yr().as_underlying_type(), ymd.mn().as_underlying_type(),
                  ymd.dm().as_underlying_type(), 0e0, &deltat) >= 0);
    assert(deltat == (double)dso::dat(ymd));
    assert(iauCal2jd(ymd.yr().as_underlying_type(),
                     ymd.mn().as_underlying_type(),
                     ymd.dm().as_underlying_type(), &jd0, &jd1) == 0);
    assert(jd0 == dso::MJD0_JD);
    mjd = dso::modified_julian_day((long)jd1);
    assert(deltat == (double)dso::dat(mjd));
    dso::dat(mjd, extra);
    if (extra) {
      ++leap_seconds_found;
      assert(is_leap_insertion_day(ymd));
    }

    /* 31/01/YYYY - insertion of leap second could happen ! */
    ymd = ymd_date(dso::year(y), dso::month(12), dso::day_of_month(31));
    assert(iauDat(ymd.yr().as_underlying_type(), ymd.mn().as_underlying_type(),
                  ymd.dm().as_underlying_type(), 0e0, &deltat) >= 0);
    assert(deltat == (double)dso::dat(ymd));
    assert(iauCal2jd(ymd.yr().as_underlying_type(),
                     ymd.mn().as_underlying_type(),
                     ymd.dm().as_underlying_type(), &jd0, &jd1) == 0);
    assert(jd0 == dso::MJD0_JD);
    mjd = dso::modified_julian_day((long)jd1);
    assert(deltat == (double)dso::dat(mjd));
    dso::dat(mjd, extra);
    if (extra) {
      ++leap_seconds_found;
      assert(is_leap_insertion_day(ymd));
    }
  }

  /* all leap seconds found */
  assert(leap_seconds_found == 27);

  return 0;
}
