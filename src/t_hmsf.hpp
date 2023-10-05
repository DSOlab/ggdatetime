#ifndef __DSO_DATETIME_THMSFFRACTION_HPP__
#define __DSO_DATETIME_THMSFFRACTION_HPP__

#include "dtfund.hpp"

namespace dso {

/** A simple wrapper class to hold time as hours, minutes, seconds
 *  and fractional seconds.
 *  
 *  The actual seconds (omitting whole minutes and hours) is: 
 *  (double)_seconds + _fraction
 */
struct t_hmsf {
  hours _hours;
  minutes _minutes;
  seconds _seconds;
  double _fraction;

/** Constructor from any type that has S::is_of_sec_type member, aka
 *  any sec-type
 */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt S>
#else
  template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
  constexpr t_hmsf(S sec) noexcept {
    const long fac = S::template sec_factor<long>();
    const typename S::underlying_type fac_36L = fac * 3600L;
    const typename S::underlying_type _sec = sec.as_underlying_type();

    const long hr = _sec / fac_36L;               // hours
    const long mn = (_sec % fac_36L) / 60L / fac; // minutes
    const typename S::underlying_type sc =
        (_sec - (hr * 3600L + mn * 60L) * fac) / fac;
    const typename S::underlying_type fs =
        _sec - ((hr * 60L + mn) * 60L + sc) * fac; // remaining S's

    _hours = hours(hr);
    _minutes = minutes(mn);
    _seconds = seconds(sc);
    _fraction = static_cast<double>(fs) / S::template sec_factor<double>();
  }

  /** Overload equality operator */
  constexpr bool operator==(const t_hmsf &other) const noexcept {
    return (_hours == other._hours && _minutes == other._minutes &&
            _seconds == other._seconds && _fraction == other._fraction);
  }

  /** Overload in-equality operator */
  constexpr bool operator!=(const t_hmsf &other) const noexcept {
    return !(*this == other);
  }

  /** return as fractional seconds */
  constexpr double fractional_seconds() const noexcept {
    return _fraction + static_cast<double>(_seconds.as_underlying_type());
  }
}; /* t_hmsf */

}/* dso */

#endif
