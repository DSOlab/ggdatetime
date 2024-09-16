/** @file
 * Define/declare generic template functions to assist handling fundamental
 * date/time classes (normally defined in dtfund.hpp) and upgrade their user
 * interafaces.
 * This file uses C++ concepts if compilation is performed vs the C++20
 * standard.
 */

#ifndef __DSO_DATETIME_GENERIC_TEMPLATE_OPERATIONS_HPP__
#define __DSO_DATETIME_GENERIC_TEMPLATE_OPERATIONS_HPP__

#include "dtconcepts.hpp"
#include <limits>
#include <type_traits>
#if __cplusplus >= 202002L
#include <concepts>
#endif

namespace dso {

/** Overload bool operator '==' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 * @note that this function only allows comparisson when both right/left sides
 *       of the expression satisfly (1) and (2)
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator==(DType a, DType b) noexcept {
  return a.__member_const_ref__() == b.__member_const_ref__();
}

/** Overload bool operator '!=' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 * 3. the operator '==' is defined
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator!=(DType a, DType b) noexcept {
  return !(a == b);
}

/** Overload bool operator '>' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator>(DType a, DType b) noexcept {
  return a.__member_const_ref__() > b.__member_const_ref__();
}

/** Overload bool operator '>=' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator>=(DType a, DType b) noexcept {
  return a.__member_const_ref__() >= b.__member_const_ref__();
}

/** Overload bool operator '<' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator<(DType a, DType b) noexcept {
  return a.__member_const_ref__() < b.__member_const_ref__();
}

/** Overload bool operator '<=' for datetime fundamental types.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr bool operator<=(DType a, DType b) noexcept {
  return a.__member_const_ref__() <= b.__member_const_ref__();
}

/** Overload bool operator '+' for datetime fundamental types
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 * 3. right operand is any Integral type
 * This function will allow e.g.
 * modified_julian_day mjd_sum =
 *  modified_julian_day(123) + modified_julian_day(1);
 * Now sum's MJD will be 124
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr DType operator+(DType _a, DType _b) noexcept {
  return DType(_a.__member_const_ref__() + _b.__member_const_ref__());
}

/** Overload bool operator '+=' for datetime fundamental types when the right
 * operand is any integral type.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * 3. right operand is any Integral type
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * mjd += 1;
 * Now mjd's internal member, will have a value of 124.
 */
template <typename DType, typename I,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>,
          typename = std::enable_if_t<std::is_integral_v<I>>>
constexpr DType &operator+=(DType &_a, I _intv) noexcept {
  _a.__member_ref__() += _intv;
  return _a;
}

/** Overload bool operator '+=' for datetime fundamental types when the right
 * operand is the same type as the calling instance.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * 3. right and left operands are of the same type
 * This function will allow e.g.
 * modified_julian_day mjd1 (123), mjd2 (132);
 * mjd1 += mjd2;
 * Now mjd's internal member, will have a value of 255.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType &operator+=(DType &_a, DType _b) noexcept {
  _a.__member_ref__() += _b.__member_const_ref__();
  return _a;
}

/** Overload bool operator '-=' for datetime fundamental types when the right
 * operand is any integral type.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * 3. right operand is any Integral type
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * mjd -= 1;
 * Now mjd's internal member, will have a value of 122.
 */
template <typename DType, typename I,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>,
          typename = std::enable_if_t<std::is_integral_v<I>>>
constexpr DType &operator-=(DType &_a, I _intv) noexcept {
  _a.__member_ref__() -= _intv;
  return _a;
}

/** Overload bool operator '-=' for datetime fundamental types when the right
 * operand is of the same type as the calling instance.
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * 3. right and left hand sides are of the same type
 * This function will allow e.g.
 * modified_julian_day mjd1 (123), mjd2 (100);
 * mjd1 -= mjd2;
 * Now mjd's internal member, will have a value of 23.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType &operator-=(DType &_a, DType _b) noexcept {
  _a.__member_ref__() -= _b.__member_const_ref__();
  return _a;
}

/** Overload prefix increment operator '++' for datetime fundamental types
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * ++mjd;
 * Now mjd's internal member, will have a value of 124.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType &operator++(DType &_a) noexcept {
  ++(_a.__member_ref__());
  return _a;
}

/** Overload postfix increment operator '++' for datetime fundamental types
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * mjd++;
 * Now mjd's internal member, will have a value of 124.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType operator++(DType &_a, int) noexcept {
  auto tmp{_a};
  ++_a;
  return tmp;
}

/** Overload prefix decrement operator '--' for datetime fundamental types
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * --mjd;
 * Now mjd's internal member, will have a value of 122.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType &operator--(DType &_a) noexcept {
  --(_a.__member_ref__());
  return _a;
}

/** Overload postfix decrement operator '--' for datetime fundamental types
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_ref__()
 * This function will allow e.g.
 * modified_julian_day mjd (123);
 * mjd--;
 * Now mjd's internal member, will have a value of 122.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>>
#endif
constexpr DType operator--(DType &_a, int) noexcept {
  auto tmp{_a};
  --_a;
  return tmp;
}

#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType>
#else
template <typename DType,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
constexpr DType operator-(DType _a, DType _b) noexcept {
  return DType(_a.__member_const_ref__() - _b.__member_const_ref__());
}

/** @brief Number of expressible days for any second type.
 *
 * This (template) function will return the number of whole days that can be
 * expressed using any instance of a second type (i.e. dso::seconds,
 * dso::milliseconds, etc). For any of these types, trying to hold more days
 * than this limit may result in overflow.
 *
 * @tparam S Any class of second type, i.e. any class S that has a (static)
 *         member variable S::is_of_sec_type set to true.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
constexpr typename S::underlying_type max_days_allowed() {
  return std::numeric_limits<typename S::underlying_type>::max() /
         S::max_in_day;
}

/** Transform any second type S to fractional days
 *
 * @warning This function assumes that a day is made up of exactly 86400 sec
 * and is thus not able to represent a fractional day when the day at hand is
 * on a leap second insertion.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S, std::floating_point T = double>
#else
template <typename S, typename T = double,
          typename = std::enable_if_t<S::is_of_sec_type>,
          typename = std::enable_if_t<std::is_floating_point<T>::value>>
#endif
T to_fractional_days(S nsec) noexcept {
  const T sec = static_cast<T>(nsec.__member_ref__());
  return sec / S::max_in_day;
}

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S, std::floating_point T = double>
#else
template <typename S, typename T = double,
          typename = std::enable_if_t<S::is_of_sec_type>,
          typename = std::enable_if_t<std::is_floating_point<T>::value>>
#endif
T to_fractional_seconds(S nsec) noexcept {
  return nsec.S::template cast_to<T>() * S::sec_inv_factor();
}

/** Explicit cast of any second type to another second type.
 *
 * Cast an instance of any second type (aka any instance for which
 * is_of_sec_type is defined and is true) to any other second type. E.g.,
 * cast seconds to milliseconds or cast microseconds to seconds. Be warned,
 * that casting to less precission (e.g. microseconds to seconds) will cause
 * loss of precission (1 microsecond is not 1e-6 seconds, it is just 0
 * seconds, remember?).
 *
 * @tparam Ssrc Any class of second type, i.e. any class S that has a (static)
 *         member variable S::is_of_sec_type set to true.
 * @tparam Strg Any class of second type, i.e. any class S that has a (static)
 *         member variable S::is_of_sec_type set to true.
 * @param[in] s An instance of type Ssrc to be cast to an instance of Strg
 * @return      The input s instance, as an instance of type Strg
 *
 * @warning this may cause loss of precission when e.g. casting milliseconds to
 *     seconds. E.g.
 *     cast_to<seconds, milliseconds>(seconds {1}) // result is 1000
 *     cast_to<milliseconds, seconds>(milliseconds {1}) // result is 0
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt Ssrc, gconcepts::is_sec_dt Strg>
#else
template <typename Ssrc, typename Strg,
          typename = std::enable_if_t<Ssrc::is_of_sec_type>,
          typename = std::enable_if_t<Strg::is_of_sec_type>>
#endif
constexpr Strg cast_to(Ssrc s) noexcept {
  /* underlying int type of the S with the highest resolution */
  using SecIntType = std::conditional_t<
      (Ssrc::template sec_factor<long>() > Strg::template sec_factor<long>()),
      typename Ssrc::underlying_type, typename Strg::underlying_type>;

  if constexpr (Strg::template sec_factor<long>() >=
                Ssrc::template sec_factor<long>()) {
    constexpr const SecIntType factor =
        Strg::template sec_factor<SecIntType>() /
        Ssrc::template sec_factor<SecIntType>();
    return Strg(s.__member_ref__() * factor);
  } else {
    // this is tricky! We must first compute the numerator and then the
    // fraction. why? check this out seconds _s1 = cast_to<milliseconds,
    // seconds>(milliseconds{2000L}); this is: (1/1000)*2000 which is 0 because
    // 1/1000 is 0, but (2000*1)/1000 = 2 which is correct
    const auto numerator =
        s.__member_ref__() * Strg::template sec_factor<long>();
    return Strg(numerator / Ssrc::template sec_factor<long>());
  }
}

} /* namespace dso */

#endif
