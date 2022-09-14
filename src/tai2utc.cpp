#include "utcdates.hpp"

typedef dso::nanoseconds::underlying_type SecIntType;
constexpr const SecIntType MAXSDAY = dso::nanoseconds::max_in_day;
constexpr const SecIntType FACTOR =
    dso::nanoseconds::template sec_factor<SecIntType>();

double dso::tai2utc(const dso::datetime<dso::nanoseconds> &tai,
                    dso::modified_julian_day &mjd) noexcept {
  // calendar date to MJD
  mjd = tai.mjd();

  // get DAT for this mjd and extra seconds in day
  int eid_m0;
  const int leap_sec_m0 = dso::dat(mjd, eid_m0);

  // leap as S
  const SecIntType leap_m0 = leap_sec_m0 * FACTOR;
  const SecIntType more_m0 = eid_m0 * FACTOR;

  SecIntType max_sec_in_day = MAXSDAY + more_m0;
  SecIntType sec_in_day = tai.sec().as_underlying_type();

  // initial guess ....
  sec_in_day -= leap_m0;

  // this is actually the previous day in UTC ...
  if (sec_in_day < leap_m0) {
    // get DAT for previous mjd
    const int leap_sec_m1 = dso::dat(mjd - modified_julian_day(1));
    const SecIntType leap_m1 = leap_sec_m1 * FACTOR;

    --mjd;
    sec_in_day = tai.sec().as_underlying_type() - leap_m0 + MAXSDAY +
                 (leap_m0 - leap_m1);
    max_sec_in_day = MAXSDAY + (leap_m0 - leap_m1);

    if (sec_in_day > max_sec_in_day) {
      // not yesterday, we are in today after all ...
      ++mjd;
      sec_in_day -= MAXSDAY + leap_m0 - leap_m1;
      max_sec_in_day = MAXSDAY + more_m0;
    }
  }

#ifdef DEBUG
  assert(sec_in_day <= max_sec_in_day);
  assert(sec_in_day >= 0);
#endif
  return static_cast<double>(sec_in_day) / static_cast<double>(max_sec_in_day);
}