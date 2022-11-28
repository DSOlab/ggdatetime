#include "utcdates.hpp"
#include <cmath>

typedef dso::nanoseconds::underlying_type SecIntType;

///< Max nanoseconds in day
constexpr const SecIntType MAXSDAY = dso::nanoseconds::max_in_day;

///< nanoseconds to seconds (int type)
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

  // scale fday to real number of seconds in day
  tai_fday *=
      static_cast<double>(MAXSDAY + more_m0) / static_cast<double>(MAXSDAY);

  // add leap seconds
  double fday_more =
      static_cast<double>(leap_m0) / static_cast<double>(MAXSDAY);

  tai_fday = std::modf(tai_fday+fday_more, &ipart);  

  tai_mjd = modified_julian_day(itai_mjd+ipart);
  return tai_fday;
}
