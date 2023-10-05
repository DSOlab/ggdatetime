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
struct hms_time {
  /** int type of seconds */
  using SecIntType = typename S::underlying_type;
  /** hours */
  hours _hours;
  /** minutes */
  minutes _minutes;
  /** seconds */
  S _sec;

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
    SecIntType remaining = isecs - _hours * secInHour;
#ifdef DEBUG
    assert(remaining < secInHour);
#endif
    /* seconds in minute */
    const SecIntType secInMin = 60L * S::template sec_factor<SecIntType>();
    /* compute/remove minutes */
    const SecIntType mn = remaining / secInMin;
    _minutes = mn;
    remaining = remaining - _minutes * secInMin;
#ifdef DEBUG
    assert(remaining < secInMin);
#endif
    /* remaining S seconds */
    _sec = remaining;
#ifdef DEBUG
    assert(_sec.as_underlying_type() + secInMin * minutes +
               secInHour * _hours ==
           seconds.as_underlying_type());
#endif
  }
}; /* hms_time */

} /* namespace dso */

#endif
