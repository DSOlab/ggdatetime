#ifndef __DSO_UTC_DATETIME_HPP__
#define __DSO_UTC_DATETIME_HPP__

#include "dtcalendar.hpp"
#include "tpdate.hpp"
#include <cassert>

namespace dso {

double dtf2d(dso::year yr, dso::month mt, dso::day_of_month dm, dso::hours hr,
             dso::minutes mn, dso::nanoseconds _sec,
             dso::modified_julian_day &mjd, dso::nanoseconds &leap) noexcept;
TwoPartDate dtf2d(dso::year yr, dso::month mt, dso::day_of_month dm,
                  dso::hours hr, dso::minutes mn, dso::nanoseconds _sec,
                  dso::nanoseconds &leap) noexcept;

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
  // Cast S-type to nanoseconds
  dso::nanoseconds n_sec = cast_to<S, dso::nanoseconds>(_sec);
  dso::nanoseconds nleap;
  // call the function (with S=nanoseconds)
  const double fday = dtf2d(yr, mt, dm, hr, mn, n_sec, mjd, nleap);
  // Cast leap seconds from nanoseconds to type S
  leap = cast_to<dso::nanoseconds, S>(nleap);
  // return fractional days
  return fday;
}

double tai2utc(const dso::datetime<dso::nanoseconds> &tai,
               dso::modified_julian_day &mjd) noexcept;
// TwoPartDate tai2utc(const TwoPartDate &tai) noexcept;

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
  const dso::datetime<dso::nanoseconds> ntai =
      tai.template cast_to<dso::nanoseconds>();
  return tai2utc(ntai, mjd);
}

double utc2tai(const dso::modified_julian_day utc_mjd, double utc_fday,
               dso::modified_julian_day &tai_mjd) noexcept;

double utc_strptime_ymd_hms(const char *str, dso::modified_julian_day &utc_mjd,
                            const char **stop = nullptr);
TwoPartDate utc_strptime_ymd_hms(const char *str, const char **stop = nullptr);

} // namespace dso
#endif
