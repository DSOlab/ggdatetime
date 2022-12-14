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

#define PRECISION 1e-15

using dso::datetime;
using dso::nanoseconds;

typedef nanoseconds::underlying_type SecIntType;
constexpr const SecIntType factor = nanoseconds::sec_factor<SecIntType>();

int main([[maybe_unused]]int argc, char *argv[]) {
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
    dso::TwoPartDate tai(ud);
    dso::TwoPartDate utc = dso::tai2utc(tai);

    // resolve using SOFA
    double d1, d2;
    double tai1 = (double)ud.mjd().as_underlying_type() + 2400000.5e0;
    double tai2 = ud.sec().fractional_days();
    if (int error; (error = iauTaiutc(tai1, tai2, &d1, &d2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }
    // so, SOFA's UTC date is d1 + d2 (in JD)

    // assertion ....
    assert(std::abs((utc._big + utc._small) - ((d1 - 2400000.5e0) + d2)) < PRECISION);
  }

  // by the way, results for 22 and 23 should be the same
  dso::TwoPartDate utc22 = dso::tai2utc(dso::TwoPartDate(idates[22]));
  dso::TwoPartDate utc23 = dso::tai2utc(dso::TwoPartDate(idates[23]));
  assert(utc22._big == utc23._big);
  assert(utc22._small == utc23._small);

  printf("All tests passed in %s\n", argv[0]);
  return 0;
}
