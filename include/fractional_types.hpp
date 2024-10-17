/** @file
 *
 * Fundamental (core) datetime types that are represented by floating point 
 * arithmetic.
 *
 * These are merely there to enforce type safety.
 */

#ifndef __DSO_DATETIME_FRACTIONAL_TYPES_HPP__
#define __DSO_DATETIME_FRACTIONAL_TYPES_HPP__

#include "core/fundamental_types_generic_utilities.hpp"

namespace dso {

/** A simple struct to signal fractional seconds; just to secure type safety */
class FractionalSeconds {
  double fsec;

public:
  typedef double underlying_type;
  static constexpr bool is_dt_fundamental_type{true};
  constexpr underlying_type __member_const_ref__() const noexcept {
    return fsec;
  }
  constexpr underlying_type &__member_ref__() noexcept { return fsec; }

  constexpr explicit FractionalSeconds(double _fsec = 0e0) noexcept : fsec(_fsec) {};
  constexpr double seconds() const noexcept { return fsec; }
  constexpr double &seconds() noexcept { return fsec; }
}; /* FractionalSeconds */

/** A simple struct to signal fractional days; just to secure type safety */
class FractionalDays {
  double fdays;

public:
  typedef double underlying_type;
  static constexpr bool is_dt_fundamental_type{true};
  constexpr underlying_type __member_const_ref__() const noexcept {
    return fdays;
  }
  constexpr underlying_type &__member_ref__() noexcept { return fdays; }

  constexpr explicit FractionalDays(double _fdays = 0e0) noexcept : fdays(_fdays) {};
  constexpr double days() const noexcept { return fdays; }
  constexpr double &days() noexcept { return fdays; }
}; /* FractionalDays */

/** A simple struct to signal fractional years; just to secure type safety */
class FractionalYears {
  double fyears;

public:
  typedef double underlying_type;
  static constexpr bool is_dt_fundamental_type{true};
  constexpr underlying_type __member_const_ref__() const noexcept {
    return fyears;
  }
  constexpr underlying_type &__member_ref__() noexcept { return fyears; }
  explicit FractionalYears(double _fyears = 0e0) noexcept : fyears(_fyears) {};
  constexpr double years() const noexcept { return fyears; }
  constexpr double &years() noexcept { return fyears; }
}; /* FractionalYears */

template <DateTimeDifferenceType DT> struct DateTimeDifferenceTypeTraits {};
template <>
struct DateTimeDifferenceTypeTraits<DateTimeDifferenceType::FractionalSeconds> {
  typedef typename dso::FractionalSeconds dif_type;
};
template <>
struct DateTimeDifferenceTypeTraits<DateTimeDifferenceType::FractionalDays> {
  typedef typename dso::FractionalDays dif_type;
};
template <>
struct DateTimeDifferenceTypeTraits<DateTimeDifferenceType::FractionalYears> {
  typedef typename dso::FractionalYears dif_type;
};

} /* namespace dso */

#endif
