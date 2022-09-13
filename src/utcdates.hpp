#ifndef __DSO_UTC_DATETIME_HPP__
#define __DSO_UTC_DATETIME_HPP__

#include "dtcalendar.hpp"
#include <cassert>

namespace dso {

/// Resolve a UTC date to an MJD and a `fractional day` part, so that the
/// date is mjd + returned value
/// @param[in] yr The input year (UTC)
/// @param[in] mt The input month (UTC)
/// @param[in] dm The input day of month (UTC)
/// @param[in] hr The input hour of day (UTC)
/// @param[in] mn The input minute of hour (UTC)
/// @param[in] _sec Seconds in day, added to hr and mn (UTC); that is, time
///            can be represented by giving hours, minutes of hour and seconds
///            of minute, or just seconds of day (with hr = mn = 0) or any 
///            combination of the two.
/// @param[out] mjd  The MJD corresponding to the input UTC (calendar) date
/// @param[out] leap The current (aka at input date) leap seconds
/// @return fractional part of the UTC date, to go with the output mjd
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
double dtf2d(dso::year yr, dso::month mt, dso::day_of_month dm, dso::hours hr,
             dso::minutes mn, S _sec, dso::modified_julian_day &mjd,
             S &leap) noexcept {

  typedef dso::nanoseconds::underlying_type SecIntType;

  // calendar date to MJD
  mjd = dso::modified_julian_day(yr, mt, dm);
  
  // the time in seconds of day
  S sinday(hr, mn, _sec);
  
  // max *seconds in day, in S resolution
  S maxsday (S::max_in_day); 

  // deal with the UTC leap second
  int extra_seconds_in_day;
  [[maybe_unused]]int leap_sec = dso::dat(mjd, extra_seconds_in_day);

  // leap seconds to S (for user)
  leap = S(leap_sec * S::template sec_factor<SecIntType>());

  // do we have extra seconds ?
  if (extra_seconds_in_day) {
    maxsday += S(extra_seconds_in_day * S::template sec_factor<SecIntType>());
  }

  // validate input time
  assert(hr.as_underlying_type() < 24 && mn.as_underlying_type() < 60 &&
         _sec < maxsday);

  // do we have an extra day (in S) ? what id we do ?
  #ifdef DEBUG
  if (sinday > maxsday) {
    fprintf(stderr, "Computed overflowing seconds in day! -> %ld\n",
            sinday.as_underlying_type());
  }
  assert(sinday <= maxsday);
  #endif

  const double dsec = static_cast<double>(sinday.as_underlying_type());
  const double msec = static_cast<double>(maxsday.as_underlying_type());
  return dsec / msec;
}

/*
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
double utc2tai(dso::modified_julian_day utc_mjd, double utc_fday, dso::modified_julian_day &tai_mjd) noexcept {
assert(utc_fday < 1e0);
  typedef dso::nanoseconds::underlying_type SecIntType;
  tai_mjd = utc_mjd;
  // get DAT for this mjd and extra seconds in day
  int eid_m0;
  const int leap_sec_m0 = dso::dat(utc_mjd, eid_m0);
  // leap as S
  const SecIntType leap_m0 = leap_sec_m0 * S::template sec_factor<SecIntType>();
  double tai_fday =
      utc_fday +
      static_cast<double>(leap_m0) /
          static_cast<double>(S::max_in_day.as_underlying_type() +
                              eid_m0 * S::template sec_factor<SecIntType>());
}
*/

/*
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
double tai2utc2(const dso::datetime<S> &tai,
               dso::modified_julian_day &mjd) noexcept {
  typedef dso::nanoseconds::underlying_type SecIntType;

  // calendar date to MJD
  mjd = tai.mjd();

  // get DAT for this mjd and extra seconds in day
  int eid_m0;
  const int leap_sec_m0 = dso::dat(mjd, eid_m0);

  // leap as S
  const SecIntType leap_m0 = leap_sec_m0 * S::template sec_factor<SecIntType>();
  const SecIntType more_m0 = eid_m0 * S::template sec_factor<SecIntType>();

  SecIntType max_sec_in_day = S::max_in_day + more_m0;
  SecIntType sec_in_day = tai.sec().as_underlying_type();

  // this is actually the previous day in UTC ...
  if (sec_in_day < leap_m0) {
    // get DAT for previous mjd
    int eid_m1;
    const int leap_sec_m1 = dso::dat(mjd - modified_julian_day(1), eid_m1);
    const SecIntType leap_m1 =
        leap_sec_m1 * S::template sec_factor<SecIntType>();
    const SecIntType more_m1 = eid_m1 * S::template sec_factor<SecIntType>();

    if (leap_m1 == leap_m0) {
      // same leap seconds for this and yesterday, indeed we are in the
      // previous day
      --mjd;
      sec_in_day = (tai.sec().as_underlying_type() - leap_m0) + S::max_in_day +
                   (leap_m0 - leap_m1);
#ifdef DEBUG
      assert(eid_m1 == eid_m0 == 0);
#endif
      // max seconds in day for yesterday
      max_sec_in_day = S::max_in_day + (leap_m0 - leap_m1);
    } else {
      // leap seconds are different for today and yesterday!! count seconds
      // from start of yesterday ...
      --mjd;
      sec_in_day = tai.sec().as_underlying_type() - leap_m0 + S::max_in_day +
                   (leap_m0 - leap_m1);
      max_sec_in_day = S::max_in_day + (leap_m0 - leap_m1);
      if (sec_in_day > max_sec_in_day) {
        // not yesterday, we are in today after all ...
        ++mjd;
        sec_in_day -= (S::max_in_day + more_m1) + (leap_m0 - leap_m1);
        max_sec_in_day = S::max_in_day + more_m0;
      }
    }
  } else {
    sec_in_day -= leap_m0;
  }

  return static_cast<double>(sec_in_day) / static_cast<double>(max_sec_in_day);
}
*/

/// Transform a datetime given in TAI time-scale to a corresponding UTC date.
/// UTC date is returned in two parts: mjd + returned_double
/// @param[in]  tai A datetime instance in TAI scale
/// @param[out] mjd The MJD corresponding to the input date transformed to UTC
/// @return     Fractional part of the transformed-to UTC date; always in
///             range [0, 1]
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
double tai2utc(const dso::datetime<S> &tai,
               dso::modified_julian_day &mjd) noexcept {
  typedef dso::nanoseconds::underlying_type SecIntType;

  // calendar date to MJD
  mjd = tai.mjd();

  // get DAT for this mjd and extra seconds in day
  int eid_m0;
  const int leap_sec_m0 = dso::dat(mjd, eid_m0);

  // leap as S
  const SecIntType leap_m0 = leap_sec_m0 * S::template sec_factor<SecIntType>();
  const SecIntType more_m0 = eid_m0 * S::template sec_factor<SecIntType>();

  SecIntType max_sec_in_day = S::max_in_day + more_m0;
  SecIntType sec_in_day = tai.sec().as_underlying_type();

  // initial guess ....
  sec_in_day -= leap_m0;

  // this is actually the previous day in UTC ...
  if (sec_in_day < leap_m0) {
    // get DAT for previous mjd
    const int leap_sec_m1 = dso::dat(mjd - modified_julian_day(1));
    const SecIntType leap_m1 =
        leap_sec_m1 * S::template sec_factor<SecIntType>();

    --mjd;
    sec_in_day = tai.sec().as_underlying_type() - leap_m0 + S::max_in_day +
                 (leap_m0 - leap_m1);
    max_sec_in_day = S::max_in_day + (leap_m0 - leap_m1);

    if (sec_in_day > max_sec_in_day) {
      // not yesterday, we are in today after all ...
      ++mjd;
      sec_in_day -= S::max_in_day + leap_m0 - leap_m1;
      max_sec_in_day = S::max_in_day + more_m0;
    }
  }

#ifdef DEBUG
assert(sec_in_day <= max_sec_in_day);
assert(sec_in_day >= 0);
#endif
  return static_cast<double>(sec_in_day) / static_cast<double>(max_sec_in_day);
}

} // namespace dso
#endif
