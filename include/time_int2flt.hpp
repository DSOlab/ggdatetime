/** @file
 *
 */

#ifndef __DSO_TIME_INTEGRAL_TO_FLOAT_HPP__
#define __DSO_TIME_INTEGRAL_TO_FLOAT_HPP__

#include "fractional_types.hpp"
#include "time_integral_types.hpp"

namespace dso {

/** Transform any second type S to fractional days
 *
 * @warning This function assumes that a day is made up of exactly 86400 sec
 * and is thus not able to represent a fractional day when the day at hand is
 * on a leap second insertion.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
FractionalDays to_fractional_days(S nsec) noexcept {
  const double sec = static_cast<double>(nsec.__member_ref__());
  return FractionalDays(sec / S::max_in_day);
}

/** Transform any second type S to fractional seconds */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
FractionalSeconds to_fractional_seconds(S nsec) noexcept {
  const double sec = nsec.S::template cast_to<double>() * S::sec_inv_factor();
  return FractionalSeconds(sec);
}

} /* namespace dso */

#endif
