#include "utcdates.hpp"
#include <cmath>

typedef dso::nanoseconds::underlying_type SecIntType;
constexpr const SecIntType MAXSDAY = dso::nanoseconds::max_in_day;
constexpr const SecIntType FACTOR =
    dso::nanoseconds::template sec_factor<SecIntType>();

double dso::utc2tai(const dso::modified_julian_day utc_mjd, double utc_fday,
                    dso::modified_julian_day &tai_mjd) noexcept {
  int itai_mjd = utc_mjd.as_underlying_type();

  // make sure the utc_fday is just a fractional part
  double ipart;
  double tai_fday = std::modf(utc_fday, &ipart);
  itai_mjd += ipart;

  // get DAT for this mjd and extra seconds in day
  int eid_m0;
  const int leap_sec_m0 = dso::dat(dso::modified_julian_day(itai_mjd), eid_m0);
  const SecIntType leap_m0 = leap_sec_m0 * FACTOR;
  const SecIntType more_m0 = eid_m0 * FACTOR;

  // initial guess, TAI and UTC are in the same day
  double fday_more =
      static_cast<double>(leap_m0) / static_cast<double>(MAXSDAY + more_m0);

  if (tai_fday + fday_more > 1e0) {
    // well, TAI could be ahead by one day ...
    // get DAT for mjd+1 and extra seconds in day
    ++itai_mjd;
    int eid_p1;
    const int leap_sec_p1 =
        dso::dat(dso::modified_julian_day(itai_mjd), eid_p1);
    const SecIntType leap_p1 = leap_sec_p1 * FACTOR;
    const SecIntType more_p1 = eid_p1 * FACTOR;

    double fday_left = 1e0 - (tai_fday + fday_more);
    fday_more = fday_left + static_cast<double>(leap_p1 - leap_m0) /
                                static_cast<double>(MAXSDAY + more_p1);
    tai_fday = 0e0;
  }

  tai_mjd = modified_julian_day(itai_mjd);
  return tai_fday + fday_more;
}