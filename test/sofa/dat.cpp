#include "dtfund.hpp"
#include "sofa.h"
#include <cassert>
#include <random>

using dso::ymd_date;

/* number of tests to perform (pre template parameter) */
long num_tests = 1'000'000;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */

  double jd0, jd1;
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
    if (i % 10)
      printf("%ld/%ld\r", i, num_tests);
  }

  int extra;
  /* leap second years in range [1972 to now] */
  const std::vector<int> leap = {2016, 2015, 2012, 2008, 2005, 1998, 1997,
                                 1995, 1994, 1993, 1992, 1990, 1989, 1987,
                                 1985, 1983, 1982, 1981, 1979, 1978, 1977,
                                 1976, 1975, 1974, 1973};
  for (const auto y : leap) {
    double deltat;
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
    if (extra)
      printf("->leap at %d/%02d/%02d\n", ymd.yr().as_underlying_type(),
             ymd.mn().as_underlying_type(), ymd.dm().as_underlying_type());

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
    if (extra)
      printf("->leap at %d/%02d/%02d\n", ymd.yr().as_underlying_type(),
             ymd.mn().as_underlying_type(), ymd.dm().as_underlying_type());
    
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
    if (extra)
      printf("->leap at %d/%02d/%02d\n", ymd.yr().as_underlying_type(),
             ymd.mn().as_underlying_type(), ymd.dm().as_underlying_type());

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
    if (extra)
      printf("->leap at %d/%02d/%02d\n", ymd.yr().as_underlying_type(),
             ymd.mn().as_underlying_type(), ymd.dm().as_underlying_type());
    
    ymd = ymd_date(dso::year(y - 1), dso::month(12), dso::day_of_month(31));
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
    if (extra)
      printf("->leap at %d/%02d/%02d\n", ymd.yr().as_underlying_type(),
             ymd.mn().as_underlying_type(), ymd.dm().as_underlying_type());
  }

  printf("All tests ok!\n");
  return 0;
}
