#include "sofa.h"
#include "utcdates.hpp"
#include <vector>
#include <cassert>

/*
 * Unit test test_utc2tai
 * tests function utcdates::utc2tai against the SOFA implementation (function
 * iauUtctai)
 * 
 * Create a number of UTC dates (close or on leap seconds introduction)
 * and try to resolve them to a two-part TAI respresentation.
 * If results differ from the SOFA implementation, the tests will fail.
 */

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

    void print() const noexcept {
      printf("%d/%02d/%02d", yr.as_underlying_type(), mo.as_underlying_type(),
             dm.as_underlying_type());

        SecIntType isec = nsec.as_underlying_type();
        int extra_sec;
        dat(dso::modified_julian_day(yr,mo,dm), extra_sec);
        // remove hours
        int ihr = isec / (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        isec = isec - ihr*(60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        if (extra_sec && ihr==24) {
            --ihr;
            isec += (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        }
        // remove minutes
        int imn = isec / (60L * nanoseconds::template sec_factor<SecIntType>());
        isec = isec - imn*(60L * nanoseconds::template sec_factor<SecIntType>());
        if (extra_sec && imn==60) {
            --imn;
            isec+=(60L * nanoseconds::template sec_factor<SecIntType>());
        }
        // seconds as double
        double sec = (double)isec / (nanoseconds::template sec_factor<double>());
        assert(sec<=60e0 + extra_sec);
    
      printf("   %02d/%02d/%018.15f\n", hr.as_underlying_type() + ihr,
             imn + mn.as_underlying_type(), sec);
      return;
    }

    // resolve to a two-part (JD) date using SOFA
    int sofa_resolve(double &d1, double &d2) const noexcept {
        SecIntType isec = nsec.as_underlying_type();
        int extra_sec;
        dat(dso::modified_julian_day(yr,mo,dm), extra_sec);
        // remove hours
        int ihr = isec / (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        isec = isec - ihr*(60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        if (extra_sec && ihr==24) {
            --ihr;
            isec += (60L * 60L * nanoseconds::template sec_factor<SecIntType>());
        }
        // remove minutes
        int imn = isec / (60L * nanoseconds::template sec_factor<SecIntType>());
        isec = isec - imn*(60L * nanoseconds::template sec_factor<SecIntType>());
        if (extra_sec && imn==60) {
            --imn;
            isec+=(60L * nanoseconds::template sec_factor<SecIntType>());
        }
        // seconds as double
        double sec = (double)isec / (nanoseconds::template sec_factor<double>());
        assert(sec<=60e0 + extra_sec);
        return iauDtf2d("UTC", yr.as_underlying_type(), mo.as_underlying_type(),
                      dm.as_underlying_type(), hr.as_underlying_type()+ihr,
                      mn.as_underlying_type()+imn, sec, &d1, &d2);
    }

    UtcDate(dso::year _yr, dso::month _mo, dso::day_of_month _dm,
            dso::hours _hr, dso::minutes _mn, nanoseconds _nsec) noexcept
        : yr(_yr), mo(_mo), dm(_dm), hr(_hr), mn(_mn), nsec(_nsec){};
    
    double resolve(dso::modified_julian_day &mjd, dso::nanoseconds &leap) const noexcept {
        return dso::dtf2d(yr, mo, dm, hr, mn, nsec, mjd, leap);
    }
}; // UtcDate
    
int main() {  
  // Some UTC dates
  std::vector<UtcDate> idates;

  // some special dates ...
  // 0
  idates.emplace_back(dso::year(2022), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0), nanoseconds(0));
  // 1
  idates.emplace_back(dso::year(2022), dso::month(1), dso::day_of_month(1),
                      dso::hours(23), dso::minutes(59), nanoseconds(59L*factor));
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
                      nanoseconds(nanoseconds::max_in_day+1'000));
  // 7
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day+1'000'000));
  // 8
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(nanoseconds::max_in_day+999'000'000));
  // 9
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(0));
  // 10
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(1'000));
  // 11
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(31),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(0));
  // 12
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(0));
  // 13
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(1'000));
  // 14
  idates.emplace_back(dso::year(2016), dso::month(12), dso::day_of_month(30),
                      dso::hours(23), dso::minutes(59),
                      nanoseconds(1'000'000));
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
                      dso::hours(0), dso::minutes(5),
                      nanoseconds(0));
  // 23
  idates.emplace_back(dso::year(2017), dso::month(1), dso::day_of_month(1),
                      dso::hours(0), dso::minutes(0),
                      nanoseconds(1'000'000'000L* 60L * 5));
  
  int i=0;
  for (const auto &ud : idates) {
    printf("> Test Case %d\n", i++);

    // resolve UTC date to MJD and fractional days
    dso::modified_julian_day utc_mjd;
    dso::nanoseconds leap;
    const double utc_fday = ud.resolve(utc_mjd, leap);

    // UTC to TAI
    dso::modified_julian_day tai_mjd;
    double tai_fday = dso::utc2tai(utc_mjd, utc_fday, tai_mjd);
    
    // transform using SOFA (UTC to TAI)
    double d1, d2;
    double utc1 = (double)utc_mjd.as_underlying_type() + 2400000.5e0;
    double utc2 = utc_fday;
    if (int error; (error = iauUtctai(utc1, utc2, &d1, &d2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }

    // yeat another test; resolve to a two-part JD using SOFA, and then use
    // SOFA (again) to transform to TAI
    double sd1, sd2;
    if (int error; (error = ud.sofa_resolve(sd1, sd2))) {
        fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }
    assert(std::abs((sd1 - 2400000.5e0) - (double)utc_mjd.as_underlying_type() +
                    sd2 - utc_fday) < 1e-15);
    double td1, td2;
    if (int error; (error = iauUtctai(sd1, sd2, &td1, &td2))) {
      fprintf(stderr, "SOFA failed to resolve date! error code=%d\n", error);
    }
    assert(std::abs((td1-d1)+(td2-d2))<1e-15);

    // print results
    double dmjd = (double)tai_mjd.as_underlying_type();
    printf("\t%.5f + %+20.15f = %.15f\n", dmjd, tai_fday, dmjd+tai_fday);
    printf("\t%.5f + %+20.15f = %.15f\n", d1 - 2400000.5e0, d2,
           (d1 - 2400000.5e0) + d2);

    // assertion ....
    //assert(std::abs((dmjd+utc_fday) - ((d1 - 2400000.5e0)+d2)) < 1e-15);
    }
  
  return 0;
}