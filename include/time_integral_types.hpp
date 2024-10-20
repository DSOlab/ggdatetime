/** @file
 *
 * The following table lists the classes defined here, along with their main
 * (internal) member functions/vars used by generic template function to
 * define their behavior. See core/fundamental_types_generic_utilities.hpp
 * --------------------+----------------+-------------+--------------+
 * Class Name          |is_dt_\         |__member_\   |__member_\    |
 *                     |fundamental_type|const_ref__()|_ref__()      |
 * --------------------+----------------+-------------+--------------+
 * --------------------+----------------+-------------+--------------+
 */

#ifndef __DSO_TIME_INTEGRAL_TYPES_HPP__
#define __DSO_TIME_INTEGRAL_TYPES_HPP__

#include "core/fundamental_types_generic_utilities.hpp"
#include <cstdint>
#include <limits>

namespace dso {

/* Forward declerations */
class hours;
class minutes;
class seconds;
class milliseconds;
class microseconds;
class nanoseconds;
class picoseconds;

/** @brief A wrapper class for hours.
 *
 * An hour is just an integer number representing hours; there is no
 * 'valid range' for hours, meaning that any integer value will do!
 * No checks are provided for hours cause, well, i don't know what to check.
 * Note however, that hours are always integer; there is *no fractional part*.
 * hours are designed to be part of a (date)time system of type
 * hours/minutes/(milli|nano|...)seconds, so they only represent integeral
 * hours -- normally hour of day, although no such restriction exists --.
 * In case a subdivision is needed (e.g. minutes, seconds, etc), then use
 * the corresponsing classes (ndso::minutes, dso::seconds, etc...).
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 */
class hours {
public:
  /** Hours are represented by ints. */
  typedef int underlying_type;

  /** Is fundamental datetime typea */
  static constexpr bool is_dt_fundamental_type = true;

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_hours;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_hours; }

  /** Constructor; default hours is 0, but any hour will do */
  explicit constexpr hours(underlying_type i = 0) noexcept : m_hours(i) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr hours &operator=(Int i) noexcept {
    m_hours = i;
    return *this;
  }
#endif

  /** Get the hours as hours::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_hours;
  }

private:
  /** The hours as hours::underlying_type */
  underlying_type m_hours;
}; /* hours */

/** @brief A wrapper class for minutes.
 *
 * A minutes is just an integer number representing minutes; there is no
 * 'valid range', meaning that any integer value will do!
 * No checks are provided (for the minute range) cause, well, i don't know
 * what to check.
 * Note however, that minutes are always integer; there is *no fractional
 * part*. They are designed to be part of a (date)time system of type
 * hours/minutes/(milli|nano|...)seconds, so they only represent integeral
 * minutes -- normally min of hours, although no such restriction exists --.
 * In case a subdivision is needed (e.g. seconds, milliseconds etc), then use
 * the corresponsing classes (dso::seconds, dso::milliseconds, etc...).
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloading outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 */
class minutes {
public:
  /** Minutes are represented by ints */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type{true};

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_min;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_min; }

  /** Constructor; any integral number will do */
  explicit constexpr minutes(underlying_type i = 0) noexcept : m_min(i) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr minutes &operator=(Int i) noexcept {
    m_min = i;
    return *this;
  }
#endif

  /** Get the minutes as minutes::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_min;
  }

private:
  /** The minutes as underlying type. */
  underlying_type m_min;
}; /* minutes */

/** @brief A wrapper class for seconds.
 *
 * seconds is just a wrapper class around integer numbers, i.e. a second is
 * just a long int and can be either positive or negative. Users are however
 * restricted by integer overflow. The maximum number of days that can be
 * expressed in seconds without fear of overflow is given by the template
 * function dso::max_days_allowed.
 *
 * Negative seconds are allowed (so that a user can perform basic operations
 * like e.g. addition), but some functions expect only positive seconds
 * (seconds::remove_days, seconds::to_days).
 * seconds is a class which represents a second subdivision (as is
 * dso::milliseconds, dso::microseconds, etc); quite a few methods should be
 * common to all of these classes, all of which have a member variable
 * seconds::is_of_sec_type which is set to true.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 *
 * @warning The maximum number of days that can be expressed in seconds without
 *          fear of overflow is given by the template function
 *          dso::max_days_allowed
 *
 * @see dso::milliseconds
 * @see dso::microseconds
 */
class seconds {
public:
  /** Seconds are represented as long ints. */
  typedef int_fast64_t underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** literal (string) description */
  static const char *unit_literal() noexcept { return "sec"; }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /** Seconds is a subdivision of seconds. */
  static constexpr bool is_of_sec_type = true;

  /** Seconds in day. */
  static constexpr underlying_type max_in_day = 86400;

  /** The scale factor to transform from seconds to seconds. */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1e0);
  }

  /** The scale factor to transform from seconds to seconds. */
  static constexpr double sec_inv_factor() noexcept { return 1e0; }

  /** Constructor; default seconds is 0, but any integral will do */
  explicit constexpr seconds(underlying_type i = 0) noexcept : m_sec(i) {};

  /** Constructor from hours, minutes, seconds. */
  explicit constexpr seconds(hours h, minutes m, seconds c) noexcept
      : m_sec{static_cast<underlying_type>(c.as_underlying_type()) +
              static_cast<underlying_type>(m.as_underlying_type()) * 60L +
              static_cast<underlying_type>(h.as_underlying_type()) * 3600L} {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr seconds &operator=(Int i) noexcept {
    m_sec = i;
    return *this;
  }
#endif

  /** Get the seconds as seconds::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

/** Cast to any arithmetic type. */
#if __cplusplus >= 202002L
  template <typename T>
    requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept {
    return static_cast<T>(m_sec);
  }

private:
  /** The seconds as underlying type. */
  underlying_type m_sec;
}; /* seconds */

/** @brief A wrapper class for milliseconds (i.e. 10**-3 sec).
 *
 * milliseconds is just a wrapper class around long integer numbers, i.e. a
 * millisecond is just a long int and can be either positive or negative.
 * Users are however restricted by integer overflow. The maximum number of days
 * that can be expressed in milliseconds without fear of overflow is given by
 * the template function dso::max_days_allowed.
 * Negative milliseconds are allowed (so that a user can perform basic
 * operations like e.g. addition), but some functions expect only positive
 * milliseconds (milliseconds::remove_days, milliseconds::to_days).
 * milliseconds is a class which represents a second subdivision (as is
 * dso::seconds, dso::microseconds, etc); quite a few methods should be
 * common to all of these classes, all of which have a member variable
 * milliseconds::is_of_sec_type which is set to true.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 * @see dso::seconds
 * @see dso::microseconds
 */
class milliseconds {
public:
  /** MilliSeconds are represented as long ints. */
  typedef int_fast64_t underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** string literal */
  static const char *unit_literal() noexcept { return "millisec"; }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /** MilliSeconds are a subdivision of seconds. */
  static constexpr bool is_of_sec_type = true;

  /** The scale factor to transform from seconds to milliseconds. i.e.
   * milliseconds = seconds * sec_factor()
   */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1000);
  }

  /** The scale factor to transform from milliseconds to seconds, i.e.
   * seconds = milliseconds * sec_inv_factor()
   */
  static constexpr double sec_inv_factor() noexcept { return 1e-3; }

  /** Max milliseconds in one day. */
  static constexpr underlying_type max_in_day = 86400L * 1000L;
  static_assert(max_in_day < std::numeric_limits<underlying_type>::max());

  /** Constructor; default milliseconds is 0. */
  explicit constexpr milliseconds(underlying_type i = 0L) noexcept
      : m_sec(i) {};

  /** Constructor from hours, minutes, milliseconds. */
  explicit constexpr milliseconds(hours h, minutes m, milliseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr milliseconds &operator=(Int i) noexcept {
    m_sec = i;
    return *this;
  }
#endif

  /** Get the milliseconds as milliseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /** Cast to fractional hours */
  constexpr double to_fractional_hours() const noexcept {
    constexpr const underlying_type secinh =
        86400L * sec_factor<underlying_type>();
    return static_cast<double>(m_sec) / static_cast<double>(secinh);
  }

/** Cast to any arithmetic type. */
#if __cplusplus >= 202002L
  template <typename T>
    requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept {
    return static_cast<T>(m_sec);
  }

private:
  /** Milliseconds as underlying type. */
  underlying_type m_sec;
}; /* class milliseconds */

/** @brief A wrapper class for microseconds (i.e 10**-6 sec.).
 *
 * microseconds is just a wrapper class around long integer numbers, i.e. a
 * microsecond is just a long int and can be either positive or negative.
 * Users are however restricted by integer overflow. The maximum number of days
 * that can be expressed in microseconds without fear of overflow is given by
 * the template function dso::max_days_allowed.
 * Negative microseconds are allowed (so that a user can perform basic
 * operations like e.g. addition), but some functions expect only positive
 * microiseconds (microseconds::remove_days, microseconds::to_days).
 * microseconds is a class which represents a second subdivision (as is
 * dso::seconds, dso::milliseconds, etc); quite a few methods should be
 * common to all of these classes, all of which have a member variable
 * microseconds::is_of_sec_type which is set to true.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 * @see dso::seconds
 * @see dso::milliseconds
 */
class microseconds {
public:
  /** Microseconds are represented as long integers. */
  typedef int_fast64_t underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** string literal */
  static const char *unit_literal() noexcept { return "microsec"; }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /** Microseconds is a subdivision of seconds. */
  static constexpr bool is_of_sec_type = true;

  /** Max microseconds in day. */
  static constexpr underlying_type max_in_day{86'400L * 1'000'000L};
  static_assert(max_in_day < std::numeric_limits<underlying_type>::max());

  /** The scale factor to transform from seconds to microseconds. i.e.
   * microseconds = seconds * sec_factor()
   */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000);
  }

  /** The scale factor to transform from microseconds to seconds, i.e.
   * seconds = microseconds * sec_inv_factor()
   */
  static constexpr double sec_inv_factor() noexcept { return 1e-6; }

  /** Constructor; default microseconds is 0; any integral number will do */
  explicit constexpr microseconds(underlying_type i = 0L) noexcept
      : m_sec(i) {};

  /** Constructor from hours, minutes, microseconds. */
  explicit constexpr microseconds(hours h, minutes m, microseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr microseconds &operator=(Int i) noexcept {
    m_sec = i;
    return *this;
  }
#endif

  /** Cast to microseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

/** Cast to any arithmetic type. */
#if __cplusplus >= 202002L
  template <typename T>
    requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept {
    return static_cast<T>(m_sec);
  }

private:
  /** Microseconds as long ints. */
  underlying_type m_sec;
}; /* microseconds */

/** @brief A wrapper class for nanoseconds (i.e 10**-9 sec.).
 *
 * nanoseconds is just a wrapper class around long integer numbers, i.e. a
 * nanosecond is just a long int and can be either positive or negative.
 * Users are however restricted by integer overflow. The maximum number of days
 * that can be expressed in nanoseconds without fear of overflow is given by
 * the template function dso::max_days_allowed.
 * Negative nanoseconds are allowed (so that a user can perform basic
 * operations like e.g. addition), but some functions expect only positive
 * nanoseconds (nanoseconds::remove_days, nanoseconds::to_days).
 * nanoseconds is a class which represents a second subdivision (as is
 * dso::seconds, dso::milliseconds, etc); quite a few methods should be
 * common to all of these classes, all of which have a member variable
 * nanoseconds::is_of_sec_type which is set to true.
 * If the code is compiled with the switch USE_DATETIME_CHECKS, then the
 * nanoseconds (constructor) can only have zero or positive values.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 * @see dso::seconds
 * @see dso::milliseconds
 */
class nanoseconds {
public:
  /** Nanoseconds are represented as long integers. */
  typedef int_fast64_t underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** string literal */
  static const char *unit_literal() noexcept { return "nanosec"; }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /** Microseconds is a subdivision of seconds. */
  static constexpr bool is_of_sec_type = true;

  /** Max nanoseconds in day. */
  static constexpr underlying_type max_in_day{86'400L * 1'000'000'000L};
  static_assert(max_in_day < std::numeric_limits<underlying_type>::max());

  /** The scale factor to transform from seconds to nanoseconds. i.e.
   * nanoseconds = seconds * sec_factor()
   */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000'000);
  }

  /** The scale factor to transform from nanoseconds to seconds, i.e.
   * seconds = nanoseconds * sec_inv_factor()
   */
  static constexpr double sec_inv_factor() noexcept { return 1e-9; }

  /** Constructor; default nanoseconds is 0. **/
  explicit constexpr nanoseconds(underlying_type i = 0L) noexcept : m_sec(i) {};

  /** Constructor from hours, minutes, nanoseconds. */
  explicit constexpr nanoseconds(hours h, minutes m, nanoseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename Int>
    requires std::integral<Int>
#else
  template <typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
#endif
  constexpr nanoseconds &operator=(Int i) noexcept {
    m_sec = i;
    return *this;
  }
#endif

  /** Cast to nanoseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /** Cast to fractional hours */
  constexpr double to_fractional_hours() const noexcept {
    constexpr const underlying_type secinh =
        86400L * sec_factor<underlying_type>();
    return static_cast<double>(m_sec) / static_cast<double>(secinh);
  }

/** Cast to any arithmetic type. */
#if __cplusplus >= 202002L
  template <typename T>
    requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept {
    return static_cast<T>(m_sec);
  }

private:
  /** Nanoseconds as long ints. */
  underlying_type m_sec;
}; /* class nanoseconds */

/** @brief A wrapper class for picoseconds (i.e 10**-12 sec.).
 */
class picoseconds {
public:
  /** Picoseconds are represented as long integers. */
  typedef int_fast64_t underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /** string literal */
  static const char *unit_literal() noexcept { return "picosec"; }

  /** Picoseconds is a subdivision of seconds. */
  static constexpr bool is_of_sec_type = true;

  /** Max picoseconds in day. */
  static constexpr underlying_type max_in_day{86'400L * 1'000'000'000'000L};
  static_assert(max_in_day < std::numeric_limits<underlying_type>::max());

  /** The scale factor to transform from seconds to picoseconds. i.e.
   * picoseconds = seconds * sec_factor()
   */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000'000'000);
  }

  /** The scale factor to transform from picoseconds to seconds, i.e.
   * seconds = picoseconds * sec_inv_factor()
   */
  static constexpr double sec_inv_factor() noexcept { return 1e-12; }

  /** Constructor; default picoseconds is 0. **/
  explicit constexpr picoseconds(underlying_type i = 0L) noexcept : m_sec(i) {};

  /** Cast to picoseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

/** Cast to any arithmetic type. */
#if __cplusplus >= 202002L
  template <typename T>
    requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept {
    return static_cast<T>(m_sec);
  }

private:
  /** Picoseconds as long ints. */
  underlying_type m_sec;
}; /* class picoseconds */

} /* namespace dso */

#endif
