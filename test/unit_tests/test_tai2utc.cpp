#include "sofa.h"
#include "utcdates.hpp"
#include <cassert>
#include <vector>

/*
 * Unit test test_tai2utc
 * tests function utcdates::tai2utc against the SOFA implementation (function
 * iauTaiutc)
 *
 * Create a number of TAI dates (close or on leap seconds introduction)
 * and try to resolve them to a two-part UTC respresentation.
 *
 * If results differ from the SOFA implementation, the tests will fail.
 *
 * WARNING Dependency on SOFA
 */

// in fractional days ...
#define PRECISION 1e-15

using dso::datetime;
using dso::nanoseconds;

typedef nanoseconds::underlying_type SecIntType;
constexpr const SecIntType factor = nanoseconds::sec_factor<SecIntType>();

int main() {
  // Some TAI dates
  std::vector<datetime<nanoseconds>> idates;

  // some special dates ...
  // 0
  idates.emplace_back(dso::year(2022), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0), nanoseconds(0));
  // 1
  idates.emplace_back(dso::year(2022), dso::month(1), dso::day_of_month(1),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(59L * factor));
  // 2
  idates.emplace_back(dso::year(2022), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day) - nanoseconds(1));
  // leap second introduction at 31/12/2016, summing up to 37
  // 3
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(1'000'000'000));
  // 4
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day) - nanoseconds(1));
  // 5
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day));
  // 6
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day + 1'000));
  // 7
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day + 1'000'000));
  // 8
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day + 999'000'000));
  // 9
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0), nanoseconds(0));
  // 10
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0), nanoseconds(1'000));
  // 11
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0), nanoseconds(0));
  // 12
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59), nanoseconds(0));
  // 13
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59), nanoseconds(1'000));
  // 14
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59), nanoseconds(1'000'000));
  // 15
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(1'000'000'000));
  // 16
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(36'000'000'000));
  // 17
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(37'000'000'000));
  // 18
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(38'000'000'000));
  // 19
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(36'000'100'000));
  // 20
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(37'000'100'000));
  // 21
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(38'000'100'000));
  // 22
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(5), nanoseconds(0));
  // 23
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(1'000'000'000L * 60L * 5));

  [[maybe_unused]]int i = 0;
  for (const auto &ud : idates) {
#ifdef VERBOSE_TESTS
    printf("> Test Case %d\n", i++);
#endif

    // TAI to UTC
    dso::modified_julian_day utc_mjd;
    double utc_fday = dso::tai2utc<nanoseconds>(ud, utc_mjd);

    // so, our UTC date is utc_mjd (the MJD) + utc_fday (the fractional day
    // part (in MJD)

    // resolve using SOFA
    double d1, d2;
    double tai1 = (double)ud.mjd().as_underlying_type() + 2400000.5e0;
    double tai2 = ud.sec().fractional_days();
    if (int error; (error = iauTaiutc(tai1, tai2, &d1, &d2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }

    // so, SOFA's UTC date is d1 + d2 (in JD)

    // print results
    double dmjd = (double)utc_mjd.as_underlying_type();
#ifdef VERBOSE_TESTS
    printf("\t%.5f + %+20.15f = %.15f\n", dmjd, utc_fday, dmjd + utc_fday);
    printf("\t%.5f + %+20.15f = %.15f\n", d1 - 2400000.5e0, d2,
           (d1 - 2400000.5e0) + d2);
#endif

    // assertion ....
    assert(std::abs((dmjd + utc_fday) - ((d1 - 2400000.5e0) + d2)) < PRECISION);
  }

  // by the way, results for 22 and 23 should be the same
  dso::modified_julian_day utc_mjd_22, utc_mjd_23;
  double utc_fday_22 = dso::tai2utc<nanoseconds>(idates[22], utc_mjd_22);
  double utc_fday_23 = dso::tai2utc<nanoseconds>(idates[23], utc_mjd_23);
  assert(utc_mjd_22 == utc_mjd_23);
  assert(utc_fday_22 == utc_fday_23);

  return 0;
}
