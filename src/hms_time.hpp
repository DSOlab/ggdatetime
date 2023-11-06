/** @file
 * A class to represent hours/minutes/seconds, i.e. a time instance.
 */
#ifndef __DSO_DATETIME_HMSTIME_HPP__
#define __DSO_DATETIME_HMSTIME_HPP__

#include "dtfund.hpp"

namespace dso {
/** A struct to hold time as hours (of day), minutes (of hour) and *seconds of
 * minute.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class hms_time {
  /** int type of seconds */
  using SecIntType = typename S::underlying_type;
  /** hours */
  hours _hours;
  /** minutes */
  minutes _minutes;
  /** seconds */
  S _sec;

public:
  hours hr() const noexcept { return _hours; }
  minutes mn() const noexcept { return _minutes; }
  S nsec() const noexcept { return _sec; }

  hms_time(hours hr, minutes mn, S sec) : _hours(hr), _minutes(mn), _sec(sec) {}

  /** @brief Check if instance is 'normally' split between hours, minutes and
   *         nsec of day.
   *
   *  Note that this version does not account for leap seconds, i.e. the 
   *  HHMMSS time: 23:59:60 is not valid!
   *  In this context, the instance is valid if:
   *  1. hours are in range [0, 24), and
   *  2. minutes are in range [0,60), and
   *  3. nsec are in range [0,max_nsec_in_one_minute)
   */
  bool is_valid() const noexcept {
    return (hr() >= hours(0) && hr() < hours(24)) &&
           (mn() >= minutes(0) && mn() < minutes(60)) &&
           (nsec() >= S(0) &&
            nsec() < S(60 * S::template sec_factor<SecIntType>()));
  }

  /** @brief Check if instance is 'normally' split between hours, minutes and
   *         nsec of day, fit for UTC dates
   *
   *  Note that this version does account for leap seconds, i.e. the HHMMSS 
   *  time: 23:59:60 can be valid, if the parameter \p is_leap_insertion_day 
   *  is set to true.
   *  In this context, the instance is valid if:
   *  1. hours are in range [0, 24), and
   *  2. minutes are in range [0,60), and
   *  3. nsec are in range [0,max_nsec_in_one_minute),
   *  or:
   *  4. the members of the instance are exactly: 23:59:60 and the parameter
   *     is_leap_insertion_day is set to true
   */
  bool is_valid(bool is_leap_insertion_day) const noexcept {
    if (!is_leap_insertion_day)
      return is_valid();
    else
      return (is_valid() ||
              ((hr() == hours(23)) && (mn() == minutes(59)) &&
               (nsec() == S(60 * S::template sec_factor<SecIntType>))));
  }

  template <typename Sto, typename = std::enable_if_t<Sto::is_of_sec_type>>
  double fractional_seconds() const noexcept {
    const double scale =
        S::template sec_factor<double>() / Sto::template sec_factor<double>();
    const SecIntType b = mn() * 60L + hr() * 60L * 60L;
    return nsec() * scale + b * scale;
  }

  /** Constructor from any second type */
  explicit hms_time(S seconds) noexcept {
    /* given integral seconds */
    SecIntType isecs = seconds.as_underlying_type();
    /* number of seconds S in hour */
    constexpr const SecIntType secInHour =
        3600L * S::template sec_factor<SecIntType>();
    /* compute/remove hours */
    const SecIntType hr = isecs / secInHour;
    _hours = hr;
    SecIntType remaining = isecs - _hours.as_underlying_type() * secInHour;
#ifdef DEBUG
    assert(remaining < secInHour);
#endif
    /* seconds in minute */
    constexpr const SecIntType secInMin =
        60L * S::template sec_factor<SecIntType>();
    /* compute/remove minutes */
    const SecIntType mn = remaining / secInMin;
    _minutes = mn;
    remaining = remaining - _minutes.as_underlying_type() * secInMin;
#ifdef DEBUG
    assert(remaining < secInMin);
#endif
    /* remaining S seconds */
    _sec = remaining;
#ifdef DEBUG
    assert(_sec.as_underlying_type() +
               secInMin * _minutes.as_underlying_type() +
               secInHour * _hours.as_underlying_type() ==
           seconds.as_underlying_type());
#endif
  }
}; /* hms_time */

} /* namespace dso */

#endif
