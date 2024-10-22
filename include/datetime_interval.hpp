/** @file
 *
 * Define a datetime_interval type. This corresponds, physically, to a time
 * duration, measured in a given precision (e.g. seconds, milliseconds, etc).
 * This class will assist the datetime class; for example, the difference of
 * two datetime's will be a datetime_interval.
 */

#ifndef __DSO_DATETIME_INTEGRAL_INTERVAL__HPP__
#define __DSO_DATETIME_INTEGRAL_INTERVAL__HPP__

#include "date_integral_types.hpp"
#include "hms_time.hpp"
#ifdef DEBUG
#include <cassert>
#endif

namespace dso {

namespace core {
/** @brief Return +1 or -1 depending on the sign of the input (arithmetic)
 * parameter.
 *
 * @param[in] value Any arithmetic value
 * @return +1 if the passed in value is >=0; -1 otherwise
 */
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
constexpr int sgn(T val) noexcept {
  return (T(0) <= val) - (val < T(0));
}

/** @brief Specialization of core::sgn for fundamental datetime types. */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr int sgn(DType val) noexcept {
  return (DType(0) <= val) - (val < DType(0));
}

/** @brief A copysign implementation for *seconds.
 *
 * Returns the magnitude of
 * */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType, typename I>
  requires integral<I>
#else
template <typename DType, typename I,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>,
          typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
constexpr DType copysign(DType val, I isgn) noexcept {
  return DType{((val >= DType(0)) ? (val.as_underlying_type())
                                  : (val.as_underlying_type() * -1)) *
               sgn(isgn)};
}

/** @brief A copysign implementation for integral types. */
#if __cplusplus >= 202002L
template <typename Iv, Is>
  requires integral<Iv> && integral<Is>
#else
template <typename Iv, typename Is,
          typename = std::enable_if_t<std::is_integral_v<Iv>>,
          typename = std::enable_if_t<std::is_integral_v<Is>>>
#endif
constexpr Iv copysign(Iv val, Is isgn) noexcept {
  return ((val >= 0) ? (val) : (val * -1)) * sgn(isgn);
}

} /* namespace core*/

/** @brief A generic, templatized class to hold a datetime period/interval.
 *
 * A datetime_interval represents a time (datetime) interval or period, i.e.
 * 5 days, 12 hours and 49 seconds. We assume a continuous time scale, no leap
 * seconds are taken into consideration --this is only an interval not an
 * actual datetime instance--.
 *
 * A datetime_interval instance can only have positive (or zero) values (for
 * both of its members). However, seperate field is stored (i.e. \p m_sign) to
 * hold 'sign' information, so that a datetime_interval instance can be easily
 * used with a datetime instance. That means that e.g. 'adding' a negative
 * interval, will extend the datetime in the past.
 *
 * A datetime_interval instance has two fundamental parts (members):
 * - a day part (i.e. holding the integral days), and
 * - a time part (i.e. holding any type S  of *second type)
 * - a sign (i.e. an integer, we only consider its sign here, not the value)
 *
 * The purpose of this class is to work together with the datetime and
 * datetimeUTC classes.
 *
 * @tparam S Any class of 'second type', i.e. any class S that has a (static)
 *           member variable S::is_of_sec_type set to true. This can be
 *           dso::seconds, dso::milliseconds, dso::microseconds.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class datetime_interval {
  /* the integral type of (whole) days */
  typedef typename S::underlying_type SecIntType;
  /* the integral type of S (*seconds) */
  typedef modified_julian_day::underlying_type DaysIntType;

private:
  /** number of whole days in interval */
  DaysIntType m_days;
  /** number of *sec in interval */
  S m_secs;
  /** sign of interval (only care for the sign of m_sign) */
  int m_sign;

public:
  /** @brief Default constructor (everything set to 0). */
  explicit constexpr datetime_interval() noexcept : m_days(0), m_secs(0) {};

  /** @brief Constructor from number of days and number of *seconds.
   *
   * The sign of the interval is extracted from \p days. The number of days
   * and number of *seconds of the interval, are considered positive (i.e.
   * absolute values of input parameters).
   * For example to construct an interval of 2days + 2sec, use:
   * datetime_interval<sec> interval(2, seconds(2));
   * If you want to mark this interval as 'negative' (normally for algebraic
   * operations with a datetime isntance), use:
   * datetime_interval<sec> interval(-2, seconds(2));
   * If you want to mark a 'negative' interval but the number of days is zero,
   * then you should use:
   * datetime_interval<sec> interval(0, seconds(-2));
   * or
   * datetime_interval<sec> interval(seconds(-2));
   * and **NOT**
   * datetime_interval<sec> interval(-0, seconds(2));
   * since there is no 'signed 0' value.
   *
   * @warning It is not possible to differentiate between -0 and +0. Hence,
   *          to construct an interval of 0 days and 1 secs (S), use:
   *          datetime_interval(S(-1)) and **NOT**
   *          datetime_interval(-0, S(1))
   */
  constexpr datetime_interval(DaysIntType days, S secs) noexcept
      : m_days(days), m_secs(secs), m_sign(core::sgn(days)) {
    normalize();
  };

  /** @brief Constructor from number of *seconds.
   *
   * The sign of the interval is taken from \p secs.
   */
  constexpr datetime_interval(S secs) noexcept
      : m_days(0), m_secs(secs), m_sign(1) {
    normalize();
  };

  /** @brief Normalize, i.e. split to integral days and *seconds of day. */
  constexpr void normalize() noexcept {
    /* number of whole days in seconds */
    const DaysIntType more =
        core::copysign(m_secs.as_underlying_type(), 1) / S::max_in_day;
    /* leftover seconds (positive) */
    const SecIntType s =
        core::copysign(m_secs.as_underlying_type(), 1) - more * S::max_in_day;
    /* add to current days, with the right sign */
    const DaysIntType days = core::copysign(m_days, m_sign) +
                             core::copysign(more, m_secs.as_underlying_type());
    /* member vars */
    m_sign =
        (days > 0) * 1 + (days < 0) * (-1) + (days == 0) * core::sgn(m_secs);
    m_secs = S(s);
    m_days = core::copysign(days, 1);
#ifdef DEBUG
    assert(m_days >= 0 && (m_secs >= 0 && m_secs < S::max_in_day));
#endif
  }

  /** @brief Return number of days in interval, always positive. */
  constexpr DaysIntType days() const noexcept { return m_days; }

  /** @brief Return number of *secs (of day) in interval, always positive. */
  constexpr S sec() const noexcept { return m_secs; }

  /** @brief Return number of *secs (of day) in interval, signed. */
  constexpr S signed_sec() const noexcept {
    return core::copysign(m_secs, m_sign);
  }

  /** @brief Return the sign of the interval. */
  constexpr int sign() const noexcept { return m_sign; }

  /** @brief Return the interval as days + *seconds in seconds type S,
   * ignoring sign (i.e. always positive).
   */
  constexpr S unsigned_total_sec() const noexcept {
    return m_secs + S(S::max_in_day * m_days);
  }

  /** @brief Return the interval as days+ *seconds in seconds type S,
   * using a negative value if the instance is marked as 'negative'.
   */
  constexpr S signed_total_sec() const noexcept {
    return core::copysign(unsigned_total_sec(), m_sign);
  }

  /** @brief Cast the interval to a signed floating point representation, i.e.
   * FractionalSeconds, FractionalDays or FractionalYears.
   */
  template <DateTimeDifferenceType DT>
  typename DateTimeDifferenceTypeTraits<DT>::dif_type
  to_fraction() const noexcept {
    /* the return type */
    using RT = typename DateTimeDifferenceTypeTraits<DT>::dif_type;

    if constexpr (DT == DateTimeDifferenceType::FractionalSeconds) {
      /* difference in fractional seconds */
      const auto big = static_cast<double>(seconds::max_in_day * m_days);
      return RT(m_sign * (big + to_fractional_seconds(m_secs).seconds()));
    } else if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
      /* difference in fractional days */
      const auto big = static_cast<double>(m_days);
      return RT(m_sign * (big + to_fractional_days(m_secs).days()));
    } else {
      /* difference in fractional years */
      const double big = static_cast<double>(m_days);
      return RT(m_sign * (big + to_fractional_days(m_secs)) /
                DAYS_IN_JULIAN_YEAR);
    }
  }

  /** @brief Overload equality operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator==(const datetime_interval &d) const noexcept {
    return m_days == d.m_days && m_secs == d.m_secs;
  }

  /** @brief Overload in-equality operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator!=(const datetime_interval &d) const noexcept {
    return !(this->operator==(d));
  }

  /** @brief Overload ">" operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator>(const datetime_interval &d) const noexcept {
    return m_days > d.m_days || (m_days == d.m_days && m_secs > d.m_secs);
  }

  /** @brief Overload ">=" operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator>=(const datetime_interval &d) const noexcept {
    return m_days > d.m_days || (m_days == d.m_days && m_secs >= d.m_secs);
  }

  /** @brief Overload "<" operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator<(const datetime_interval &d) const noexcept {
    return m_days < d.m_days || (m_days == d.m_days && m_secs < d.m_secs);
  }

  /** @brief Overload "<=" operator.
   * @warning Comparing only the 'absolute value' of the interval, the sign
   * is not considered.
   */
  constexpr bool operator<=(const datetime_interval &d) const noexcept {
    return m_days < d.m_days || (m_days == d.m_days && m_secs <= d.m_secs);
  }
}; /* class datetime_interval */

} /* namespace dso */

#endif
