#include "sofa.h"
#include "utcdates.hpp"
#include <cassert>
#include <vector>

/*
 * Unit test test_utc2tai
 * tests function utcdates::utc2tai against the SOFA implementation (function
 * iauUtctai)
 *
 * Create a number of UTC dates (close or on leap seconds introduction)
 * and try to resolve them to a two-part TAI respresentation.
 * If results differ from the SOFA implementation, the tests will fail.
 *
 * WARNING depends on SOFA
 */

#define PRECISION 1e-15

using dso::datetime;
using dso::nanoseconds;

typedef nanoseconds::underlying_type SecIntType;
constexpr const SecIntType factor = nanoseconds::sec_factor<SecIntType>();

struct UtcDate {
  dso::year yr;
  dso::month mo;
  dso::day_of_month dm;
  dso::hours hr;
  dso::minutes mn;
  nanoseconds nsec;
  
  // resolve to a two-part (JD) date using SOFA
  int sofa_resolve(double &d1, double &d2) const noexcept {
    SecIntType isec = nsec.as_underlying_type();
    int extra_sec;
    dat(dso::modified_julian_day(yr, mo, dm), extra_sec);
    // remove hours
    int ihr =
        isec / (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
    isec = isec -
           ihr * (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
    if (extra_sec && ihr == 24) {
      --ihr;
      isec += (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
    }
    // remove minutes
    int imn = isec / (60L * nanoseconds::template sec_factor<SecIntType>());
    isec = isec - imn * (60L * nanoseconds::template sec_factor<SecIntType>());
    if (extra_sec && imn == 60) {
      --imn;
      isec += (60L * nanoseconds::template sec_factor<SecIntType>());
    }
    // seconds as double
    double sec = (double)isec / (nanoseconds::template sec_factor<double>());
    assert(sec <= 60e0 + extra_sec);
    return iauDtf2d("UTC", yr.as_underlying_type(), mo.as_underlying_type(),
                    dm.as_underlying_type(), hr.as_underlying_type() + ihr,
                    mn.as_underlying_type() + imn, sec, &d1, &d2);
  }
  
  dso::TwoPartDate resolve(dso::nanoseconds &leap) const noexcept {
    return dso::dtf2d(yr, mo, dm, hr, mn, nsec, leap);
  }

  UtcDate(dso::year _yr, dso::month _mo, dso::day_of_month _dm, dso::hours _hr,
          dso::minutes _mn, nanoseconds _nsec) noexcept
      : yr(_yr), mo(_mo), dm(_dm), hr(_hr), mn(_mn), nsec(_nsec){};

}; // UtcDate

int main([[maybe_unused]]int argc, char *argv[]) {
  // Some UTC dates
  std::vector<UtcDate> idates;

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

  [[maybe_unused]] int i = 0;
  for (const auto &ud : idates) {
#ifdef VERBOSE_TESTS
    printf("> Test Case %d\n", i++);
#endif

    // Resolve datetime instance to a TwoPartDate instance
    [[maybe_unused]] dso::nanoseconds leap;
    dso::TwoPartDate utc(ud.resolve(leap));

    // get the TAI date
    dso::TwoPartDate tai = dso::utc2tai(utc);

    // test 1: resolve YYMMDD,HH:MM.SS to a two-part JD using SOFA, and then
    // use SOFA (again) to transform to TAI
    double sd1, sd2;
    if (int error; (error = ud.sofa_resolve(sd1, sd2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }
    // now sd1 + sd2 (JD) should be the same as utc instance
    assert(std::abs((sd1 - 2400000.5e0) - utc._big +
                    sd2 - utc._small) < PRECISION);
    // SOFA resolved UTC (aka sd1+sd2) to TAI
    double td1, td2;
    if (int error; (error = iauUtctai(sd1, sd2, &td1, &td2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }

    // note that SOFA produces a JD (not MJD)
    assert(std::abs((td1 - 2400000.5e0 - tai._big) + (td2 - tai._small)) <
           PRECISION);
  }
  
  printf("All tests passed in %s\n", argv[0]);
  return 0;
}
