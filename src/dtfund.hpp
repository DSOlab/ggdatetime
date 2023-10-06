/** @file
 * Fundamental (core) datetime constants, algorithms and types.
 * This file contains fundamental constants, algorithms and data structures,
 * for manipulating date and time, targeting GNSS applications.
 */

#ifndef __DSO_DATETIME_DTFUND_HPP__
#define __DSO_DATETIME_DTFUND_HPP__

#include "cdatetime.hpp"
#include "dtconcepts.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>

#ifdef DEBUG
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#endif

namespace dso {

/* Forward declerations */
class year;
class month;
class gps_week;
class day_of_month;
class day_of_year;
class modified_julian_day;
struct ymd_date;
struct ydoy_date;
class hours;
class minutes;
class seconds;
class milliseconds;
class microseconds;
class nanoseconds;

namespace core {
/** Number of days past at the end of non-leap and leap years. */
constexpr const long month_day[2][13] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

/* Month lengths in days */
constexpr const int mtab[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/** @brief Calendar date to Modified Julian Day.
 *
 * Given a calendar date (i.e. year, month and day of month), compute the
 * corresponding Modified Julian Day. The input date is checked and an
 * exception is thrown if it is invalid.
 *
 * @param[in] iy The year (int).
 * @param[in] im The month (int).
 * @param[in] id The day of month (int).
 * @return    The Modified Julian Date (as long).
 * @throw     A runtime_error if the month and/or day is invalid.
 *
 * @note The algorithm used is valid from -4800 March 1
 * @see IAU SOFA iauCal2jd
 */
constexpr long cal2mjd(int iy, int im, int id) {
  /* Validate month */
  if (im < 1 || im > 12) {
    throw std::out_of_range("[ERROR] dso::cal2mjd -> Invalid Month.\n");
  }

  /* If February in a leap year, 1, otherwise 0 */
  int ly = ((im == 2) && !(iy % 4) && (iy % 100 || !(iy % 400)));

  /* Validate day, taking into account leap years */
  if ((id < 1) || (id > (mtab[im - 1] + ly))) {
    throw std::out_of_range(
        "[ERROR] dso::cal2mjd() -> Invalid Day of Month.\n");
  }

  /* Compute mjd */
  const int my = (im - 14) / 12;
  const long iypmy = static_cast<long>(iy + my);

  return (1461L * (iypmy + 4800L)) / 4L +
         (367L * static_cast<long>(im - 2 - 12 * my)) / 12L -
         (3L * ((iypmy + 4900L) / 100L)) / 4L + static_cast<long>(id) -
         2432076L;
}

/** @brief Check if year is leap.
 * @param[in] iy The year to check (int).
 * @return true if year is leap, false otherwise.
 */
inline constexpr bool is_leap(int iy) noexcept {
  return !(iy % 4) && (iy % 100 || !(iy % 400));
}

/** @brief Convert a pair of Year, Day of year to MJDay.
 *
 * Convert a pair of year, day_of_year to a modified_julian_day. The input
 * date is checked to see if it is valid (i.e. Day of year is a positive
 * integer within the range [1, 365] or [1,366] if year is leap.
 *
 * @param[in] iyr Year
 * @param[in] idoy The day of year
 * @return The given date as Modified Julian Day
 * @throw An std::out_of_range is the given day of year is invalid
 */
inline constexpr long ydoy2mjd(long iyr, long idoy) {
  if (idoy <= 0 || idoy > 365 + is_leap(iyr)) {
    throw std::out_of_range(
        "[ERROR] dso::ydoy2mjd() -> Invalid Day of Year.\n");
  }
  return ((iyr - 1901L) / 4L) * 1461L + ((iyr - 1901L) % 4L) * 365L + idoy -
         1L + dso::JAN11901;
}

/* @brief Julian Date to Julian Epoch
 *
 * Julian epoch uses the Julian year of exactly 365.25 days, and the TT time
 * scale; Julian epoch 2000.0 is defined to be 2000 January 1.5, which is
 * JD 2451545.0 or MJD 51544.5. The epoch is denoted by a prefix ‘J’, hence
 * “J2000.0”.
 *
 * Convert a Julian date to a Julian Epoch. The date is passed in a two-part
 * fractional (double) numeric value, aka the Julian Date is available as a
 * single number by adding dj1 and dj2. The maximum resolution is achieved if
 * dj1 is 2451545.0 (J2000.0).
 *
 * @param[in] dj1 First part of julian date to convert (for maximum
 *                resolution, should be J2000.0)
 * @parampin] dj2 Second part of julian date to convert, aka JD = dj1 + dj2
 * @return The input date as Julian Epoch.
 *
 * @see IAU SOFA epj.c
 */
inline constexpr double jd2epj(double dj1, double dj2) noexcept {
  return 2000e0 + ((dj1 - J2000_JD) + dj2) / DAYS_IN_JULIAN_YEAR;
}

/** @brief Modified Julian Date to Julian Epoch
 *
 * Convert a Modified Julian date to a Julian Epoch.
 *
 * @see jd2epj
 *
 * @param[in] mjd The Modified Julian Date
 * @return The input date as Julian Epoch.
 */
inline constexpr double mjd2epj(double mjd) noexcept {
  return 2000e0 + (mjd - J2000_MJD) / DAYS_IN_JULIAN_YEAR;
}

/** @brief Julian Epoch to Modified Julian Date
 *
 * @param[in] epj The Julian Epoch to convert
 * @return The corresponding (fractional) Modified Julian Date
 *
 * @see IAU SOFA epj2jd
 */
inline constexpr double epj2mjd(double epj) noexcept {
  return J2000_MJD + (epj - 2000e0) * DAYS_IN_JULIAN_YEAR;
}
} /* namespace core */

/** @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
 *
 * The day of month is actually not needed, since all leap second insertions
 * happen at the begining, i.e. the first day of a month.
 * If the specified date is for a day which ends with a leap second, the
 * UTC-TAI value returned is for the period leading up to the leap second. If
 * the date is for a day which begins as a leap second ends, the UTC-TAI
 * returned is for the period following the leap second.
 * This version works for post 01.01.1972 dates.
 * @note In case using MJD (and not calendar date) is more convinient, use the
 *       overloaded function dso::dat
 * @warning
 *    - This version only works for post-1972 dates! For a more complete
 *      version, see the iauDat.c routine from IAU's SOFA.
 *    - No checks are performed for the validity of the input date.
 * @see IAU SOFA (iau-dat.c)
 * @see dso::dat
 *
 * @param[in] iy The year
 * @param[in] im The month
 * @return TAI-UTC up to the datetime (\p iy, \p im, 23:59:59)
 */
int dat(year iy, month im) noexcept;

/** @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
 *
 * Overload of dso::dat(dso::year, dso::month) for MJD.
 * If the specified date is for a day which ends with a leap second, the
 * UTC-TAI value returned is for the period leading up to the leap second. If
 * the date is for a day which begins as a leap second ends, the UTC-TAI
 * returned is for the period following the leap second.
 * This version works for post 01.01.1972 dates.
 * @warning
 *    - This version only works for post-1972 dates! For a more complete
 *      version, see the iauDat.c routine from IAU's SOFA.
 *
 * @see IAU SOFA (iau-dat.c)
 * @see dso::dat
 *
 * @param[in] mjd The date as MJD
 * @return TAI-UTC up to the datetime (\p mjd, 23:59:59)
 */
int dat(modified_julian_day mjd) noexcept;

/** @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
 *
 * Overload of dso::dat(dso::year, dso::month) for MJD.
 * If the specified date is for a day which ends with a leap second, the
 * UTC-TAI value returned is for the period leading up to the leap second. If
 * the date is for a day which begins as a leap second ends, the UTC-TAI
 * returned is for the period following the leap second.
 * This version works for post 01.01.1972 dates.
 * @warning
 *    - This version only works for post-1972 dates! For a more complete
 *      version, see the iauDat.c routine from IAU's SOFA.
 *
 * @see IAU SOFA (iau-dat.c)
 * @see dso::dat
 *
 * @param[in] mjd The date as MJD
 * @param[out] If this date is a day which ends with a leap second, the number
 *            of extra seconds to be added in the total amount of seconds in
 *            day. For non-leap second days, this will be 0. If \mjd is indeed
 *            a day which ends with a leap second, then it will be 1.
 * @return TAI-UTC up to the datetime (\p mjd, 23:59:59)
 */
int dat(modified_julian_day mjd, int &extra_sec_in_day) noexcept;

/** @brief A wrapper class for years.
 *
 * A year is represented by just an integer number. There are no limits
 * (excpept from integer overflow) to the range of the year.
 *
 * A year is not an integer; hence, operations with integral values (aka
 * addition, subtraction, etc) are not allowed (they will actually triger a
 * compilation error). The only thing that is allowed, is assigning from
 * integral types.
 * @code
 *   year yr {2019};
 *   yr = year + 1; // error!
 *   yr = 2018;     // ok
 * @endcode
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 */
class year {
public:
  /** Years are represented as integers. */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   *
   * @return reference to the instance's member variable (as
   * year::underlying_type)
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_year;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function.
   *
   * @return const reference to the instance's member variable (as
   *         year::underlying_type)
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_year; }

  /** Constructor; default year is 1900.
   * Note that the constrcutor is NOT explicit to allow construction from
   * int (aka to allow lines of codes of type: year y = 1901;)
   */
  constexpr year(underlying_type i = 1900) noexcept : m_year(i) {}

  /** Overload operator '=' where the the right-hand-side is any integral type.
   *
   * @tparam  I any integral type, aka any type with std::is_integral_v<I> is
   *          true
   * @param  _intv Any integral value; set the instance's value equal to this
   */
#if __cplusplus >= 202002L
  template <typename I>
    requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr year &operator=(I _intv) noexcept {
    __member_ref__() = static_cast<underlying_type>(_intv);
    return *this;
  }

  /** Get the year as year::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_year;
  }

  /** Check if year is leap (aka has 366 --integer-- days instead of 365) */
  constexpr bool is_leap() const noexcept { return dso::core::is_leap(m_year); }

private:
  /** The year as underlying type. */
  underlying_type m_year;

}; /* year */

/** @brief A wrapper class for months.
 *
 * A month is represented by just an integer number. There are no limits
 * (excpept from integer overflow) to the range of the month (integer), i.e.
 * the month is not checked (by default) to be in the range [1,12]. So, a user
 * can construct a month from whatever integer, e.g. :
 * month m (-200);
 * will work fine! To check if the month is within the range [1,12], use the
 * month::is_valid method.
 * Most functions (within dso) accept months in the range [1,12]; do not
 * use the range [0,11], except if you realy know what you're doing.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 *
 * @warning Most functions (within dso) accept months in the range [1,12];
 *          do not use the range [0,11], except if you realy know what you're
 *          doing.
 */
class month {
public:
  /** Months are represented as int. */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type{true};

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_month;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_month; }

  /** Constructor; default month is 1.
   *
   * This is an explicit constructor, we do not want users to be able to do
   * month m = 1;
   * @warning No check is performed by default for the input value \p i , so
   *   you can practically assign month=123. If you want a validity check, use
   *   the month::is_valid function (after construction).
   */
  explicit constexpr month(underlying_type i = 1) noexcept : m_month(i){};

  /** @brief Constructor from a c-string.
   * Given a c-string (i.e. null-terminating char array), resolve the month.
   * The c-string can be either a short name (i.e. a 3-character name), e.g.
   * "Jan", or the whole, normal month name e.g. "January".
   * If a 3-char length string is passed in, we are going to compare using the
   * month::short_names array; else if the length of the input string is more
   * than 3-chars, the month::long_names array is used.
   * The function is case-insensitive, i.e. "January", "JANUARY" and "JanUAry"
   * are all considered the same.
   * If the input string cannot be matced to any of the strings in short_names
   * and long_names, then an exception is thrown of type: std::invalid_argument
   * Note that the month will be returned in the "normal" range [1,12],
   * **not** [0-11].
   *
   * @param[in] str The month's name; The string should match a month in the
   *    month::short_names or month::long_names array. The string should be
   *    null-trerminated.
   * @throw An std::invalid_argument exception is thrown if a) no
   *    match is found, or b) the input string is too short.
   */
  explicit month(const char *str);

  /** Get the month as month::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_month;
  }

  /** Return the corresponding short name (i.e. 3-char name) e.g. "Jan".
   *
   * The function will first perform a validity check on the instance (i.e.
   * make sure the month is within [1,12]; if the instance is invalid, it will
   * throw.
   *
   * @return Returns a pointer to the class's (static member) short_names
   *         string array.
   */
  const char *short_name() const;

  /** Return the corresponding long name (i.e. normal month name) e.g.
   * "January".
   *
   * The function will first perform a validity check on the instance (i.e.
   * make sure the month is within [1,12]; if the instance is invalid, it will
   * throw.
   * @return Returns a pointer to the class's (static member) month::long_names
   *         string array.
   */
  const char *long_name() const;

  /** Check if the month is within the interval [1,12]. */
  constexpr bool is_valid() const noexcept {
    return m_month > 0 && m_month <= 12;
  }

private:
  /** short month names. */
  constexpr static const char *short_names[] = {"Jan", "Feb", "Mar", "Apr",
                                                "May", "Jun", "Jul", "Aug",
                                                "Sep", "Oct", "Nov", "Dec"};

  /** long month names. */
  constexpr static const char *long_names[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};

  /** The month as underlying_type. */
  underlying_type m_month;

}; /* month */

/** @brief A wrapper class for GPS Week.
 *
 * The GPS Week Number count began at approximately midnight on the evening
 * of 05 January 1980 / morning of 06 January 1980. Since that time, the
 * count has been incremented by 1 each week, and broadcast as part of the GPS
 * message.
 * A gps week is represented by just an integer number. There are no limits
 * (excpept from integer overflow) to the range of the month (integer), i.e.
 * the week is not checked (by default) to be in any range. So, a user
 * can construct a gps_week from whatever integer.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 */
class gps_week {
public:
  /** gps weeks are represented as long int. */
  typedef long underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type{true};

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_week;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_week; }

  /** Constructor; default week is 1.
   * This is an explicit constructor, we do not want users to be able to do
   * gps_week w = 1;
   */
  explicit constexpr gps_week(underlying_type i = 1) noexcept : m_week(i){};

/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename I>
    requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr gps_week &operator=(I _intv) noexcept {
    __member_ref__() = static_cast<underlying_type>(_intv);
    return *this;
  }

  /** Get the month as month::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_week;
  }

  /** Check if the instance is valid, aka if the week is the range [0,+Inf) */
  constexpr bool is_valid() const noexcept { return m_week >= 0; }

private:
  /** The month as underlying_type. */
  underlying_type m_week;

}; /* gps_week */

/** @brief A wrapper class for day of month.
 *
 * A day_of_month is just an integer, representing the day of month. No limits
 * are set though, so the user can construct a day_of_month from whatever
 * integer. Inluding negative numbers!
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 */
class day_of_month {
public:
  /** Days are represented as ints. */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type{true};

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_dom;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_dom; }

  /** Constructor; default day of month is 1.
   * Note that no validation is performed when constructing an instance on the
   * input parameter \p i. I.e. it could be a negative integer. To perform an
   * actual validation test, the month and year are needed. Given this
   * information, one can use the is_valid method to test if the day of month
   * is valid.
   */
  explicit constexpr day_of_month(underlying_type i = 1) noexcept : m_dom(i){};

/** Overload operator '=' where the the right-hand-side is any integral type.
 * @tparam I any integral type, aka any type for which std::is_integral_v<I>
 *         is true
 * @param  _intv Any integral value; set the instance's value equal to this
 */
#if __cplusplus >= 202002L
  template <typename I>
    requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr day_of_month &operator=(I _intv) noexcept {
    __member_ref__() = static_cast<underlying_type>(_intv);
    return *this;
  }

  /** Get the day_of_month as day_of_month::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_dom;
  }

  /** @brief Check if a given instance is valid.
   *
   * Validate a given day_of_month. To do this, we obviously need the month
   * the instance refers to (to see how many days the month actualy has) and
   * the year, to check if it is leap or not.
   *
   * @param[in] y  The year the dom referes to (will check for leap)
   * @param[in] m  The month the dom refers to; range [1,12]
   * @return       If the dom is valid (considering the year and month) true
   *               is returned; else, the function will return false.
   */
  constexpr bool is_valid(dso::year y, dso::month m) const noexcept;

private:
  /** The day of month as underlying_type. */
  underlying_type m_dom;

}; /* day_of_month */

/** @brief A wrapper class for day of year.
 *
 * A day of year (doy) is represented by an integer; any integer will do, no
 * automatic check is performed to see if the doy is within the range
 * [1,365/6]. In fact, one can actually use a negative integer to represent
 * a doy. It the user wishes to check wether the given doy is indeed valid,
 * then the method day_of_year::validate() can be used.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 */
class day_of_year {
public:
  /** Day of year represented as int. */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type{true};

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_doy;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_doy; }

  /** Constructor; default day of year is 0.
   * No validation test is performed on the input parameter. If you want to
   * check for an invalid day of year, then use the is_valid method (after
   * instance construction).
   * */
  explicit constexpr day_of_year(underlying_type i = 0) noexcept : m_doy(i){};

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
  constexpr day_of_year &operator=(Int i) noexcept {
    m_doy = i;
    return *this;
  }

  /** Cast to underlying type **/
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_doy;
  }

  /** Check validity; doy must belong to a year to check this.
   * A day of yeay is valid if it is in the range [1,365] or [1,366] for
   * leap years.
   */
  constexpr bool is_valid(year y) const noexcept {
    return (m_doy > 0) && (m_doy <= (365 + y.is_leap()));
  }

private:
  /** The day_of_year as day_of_year::underlying_type. */
  underlying_type m_doy;
}; /* day_of_year */

/** @brief This struct represents a date in Year-Month-Day of Month format.
 *
 * This struct is only designed to ease the input/output parameters to various
 * functions. Users can actually construct any date, even non-valid ones
 * (e.g. set month to 0). The constructor will not check the input parameters.
 * If users want to check the instance for validity, then they should use the
 * ymd_date::is_valid function.
 */
struct ymd_date {
  /** @brief ymd_date constructor
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ymd_date::is_valid
   */
  constexpr ymd_date(year y = year{}, month m = month{},
                     day_of_month d = day_of_month{}) noexcept
      : __year(y), __month(m), __dom(d) {}

  /** @brief Check if the date is a valid calendar date
   * @return True if the date is valid, false otherwise.
   */
  constexpr bool is_valid() const noexcept {
    return __dom.is_valid(__year, __month);
  }

  /** @brief Transform to year and day-of-year
   * Note that no validation checks are performed on the instance. If needed,
   * (e.g. before the conversion), use the is_valid method on the instance.
   */
  constexpr ydoy_date to_ydoy() const noexcept;

  year __year;        /** the year */
  month __month;      /** the month */
  day_of_month __dom; /** day of month */
};                    /* ymd_date */

namespace core {
/** @brief Modified Julian Day to calendar date
 *
 * Note that the \p mjd parameter, should represent an integral day, i.e. no
 * fractional part (of day) is considered.
 *
 * @param[in] mjd The MJDay
 * @param[out] iyear The year
 * @param[out] imonth The month
 * @param[out] idom The day of month
 */
constexpr ymd_date mjd2ymd(long mjd) noexcept {
  long l = mjd + (68569L + 2400000L + 1);
  long n = (4L * l) / 146097L;
  l -= (146097L * n + 3L) / 4L;
  long i = (4000L * (l + 1L)) / 1461001L;
  l -= (1461L * i) / 4L - 31L;
  long k = (80L * l) / 2447L;

  int idom = l - (2447L * k) / 80L;
  l = k / 11L;
  int imonth = k + 2L - 12L * l;
  int iyear = 100L * (n - 49L) + i + l;

  return ymd_date(year(iyear), month(imonth), day_of_month(idom));
}
} /* namespace core */

/** @brief A wrapper class for Modified Julian Day (i.e. integral days).
 *
 * A Modified Julian Day is represented by a long integer (there is no
 * fractional part). Thus, a modified_julian_day only represents a date *not*
 * a datetime.
 * The Modified Julian Day, was introduced by space scientists in the late
 * 1950's. It is defined as \f$MJD = JD - 2400000.5\f$ The half day (used in
 * JD) is subtracted so that the day starts at midnight in conformance with
 * civil time reckoning.
 * The MJD is a convenient dating system with only 5 digits, sufficient for
 * most modern purposes. To convert between MJD and JD we need the Julian
 * Date of Modified Julian Date zero, aka MJD0_JD, which is 2400000.5
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 *
 * @see http://tycho.usno.navy.mil/mjd.html
 */
class modified_julian_day {
public:
  /** MJDs are represented as long ints. */
  typedef long underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_mjd;
  }

  /** If fundamental type, the class should have an
   * "expose the only member var" function
   */
  constexpr underlying_type &__member_ref__() noexcept { return m_mjd; }

  /** Max possible modified_julian_day */
  constexpr static modified_julian_day max() noexcept {
    return modified_julian_day{std::numeric_limits<underlying_type>::max()};
  }

  /** Min possible modified_julian_day */
  constexpr static modified_julian_day min() noexcept {
    return modified_julian_day{std::numeric_limits<underlying_type>::min()};
  }

  /** Constructor; default Modified Julian Day is 1.
   * This is a non-explicit constructor, hence we can perform:
   * modified_julian_day mjd = 123456;
   */
  constexpr modified_julian_day(underlying_type i = 1) noexcept : m_mjd(i){};

  /** @brief Constructor from Year and DayOfYear.
   * The passed in date (year and doy) are tested to see if they represent a
   * valid date. If not, the constructor will throw!
   *
   * @param[in] iy The year.
   * @param[in] id The day of year.
   *
   * @see "Remondi Date/Time Algorithms",
   * http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
   */
  constexpr modified_julian_day(year iy, day_of_year id)
      : m_mjd(
            core::ydoy2mjd(iy.as_underlying_type(), id.as_underlying_type())){};

  /** @brief Constructor from  calendar date
   * The passed in date is tested to see if they represent a valid date. If
   * not, the constructor will throw!
   *
   * @param[in] y The year.
   * @param[in] m The month.
   * @param[in] d The day of month
   *
   * @see "Remondi Date/Time Algorithms",
   * http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
   */
  constexpr modified_julian_day(year y, month m, day_of_month d)
      : m_mjd(core::cal2mjd(y.as_underlying_type(), m.as_underlying_type(),
                            d.as_underlying_type())){};

  /** Overload operator '=' where the the right-hand-side is any integral type.
   * @tparam I any integral type, aka any type for which std::is_integral_v<I>
   *         is true
   * @param  _intv Any integral value; set the instance's value equal to this
   */
#if __cplusplus >= 202002L
  template <typename I>
    requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr modified_julian_day &operator=(I _intv) noexcept {
    __member_ref__() = static_cast<underlying_type>(_intv);
    return *this;
  }

  /** Get the modified_julian_day as modified_julian_day::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_mjd;
  }

  /** Transform to Julian Day
   * The Julian Day is returned as a double; computed by the formula:
   * MJD = JD − 2400000.5
   * see https://en.wikipedia.org/wiki/Julian_day
   */
  constexpr double to_julian_day() const noexcept {
    return static_cast<double>(m_mjd) + MJD0_JD;
  }

  /** @brief Convert a Modified Julian Day to Year and Day of year.
   *
   * @return A ydoy_date instance
   *
   * @warning No check if performed to see if the resulting day of year is
   *          valid! If you want to be sure, check the returned value(s).
   *
   * @see "Remondi Date/Time Algorithms",
   * http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
   */
  ydoy_date to_ydoy() const noexcept;

  /** @brief Convert a Modified Julian Day to Calendar Date.
   *
   * @return A ymd_date instance
   *
   * @warning No check if performed to see if the resulting month and day of
   *          month is valid! If you want to be sure, check the returned
   *          value(s).
   *
   * @see "Remondi Date/Time Algorithms",
   *      http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
   */
  constexpr ymd_date to_ymd() const noexcept { return core::mjd2ymd(m_mjd); }

private:
  /** The modified julian day as underlying type. */
  underlying_type m_mjd;

}; /* modified_julian_day */

/** @brief This struct represent a date in Year-Day of Year format.
 *
 * This struct is only designed to ease the input/output parameters to various
 * functions. Users can actually construct any date, even non-valid ones
 * (e.g. set year to 0). The constructor will not check the input parameters.
 * If users want to check the instance for validity, then they should use the
 * ymd_date::is_valid function.
 */
struct ydoy_date {
  /** @brief ymd_date constructor
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ymd_date::is_valid
   */
  constexpr ydoy_date(year y = year{}, day_of_year d = day_of_year{}) noexcept
      : __year(y), __doy(d) {}

  /** @brief Check if the date is a valid calendar date
   * @return True if the date is valid, false otherwise.
   */
  constexpr bool is_valid() const noexcept { return __doy.is_valid(__year); }

  /** @brief Transform to year, month, day-of-month */
  constexpr ymd_date to_ymd() const noexcept;

  year __year;       /** the year */
  day_of_year __doy; /** day of year */
};                   /* ydoy_date */

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
 * If the code is compiled with the switch USE_DATETIME_CHECKS, then the
 * hours (constructor) can only have zero or positive values.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
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
  explicit constexpr hours(underlying_type i = 0) noexcept : m_hours(i){};

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
 * If the code is compiled with the switch USE_DATETIME_CHECKS, then the
 * minutes (constructor) can only have zero or positive values.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
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
  explicit constexpr minutes(underlying_type i = 0) noexcept : m_min(i){};

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
 * seconds is just a wrapper class around long integer numbers, i.e. a second
 * is just a long int and can be either positive or negative. Users are however
 * restricted by integer overflow. The maximum number of days that can be
 * expressed in seconds without fear of overflow is given by the template
 * function dso::max_days_allowed.
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

  /** Constructor; default seconds is 0, but any integral will do */
  explicit constexpr seconds(underlying_type i = 0) noexcept : m_sec(i){};

  /** Constructor from hours, minutes, seconds. */
  explicit constexpr seconds(hours h, minutes m, seconds c) noexcept
      : m_sec{static_cast<underlying_type>(c.as_underlying_type()) +
              static_cast<underlying_type>(m.as_underlying_type()) * 60L +
              static_cast<underlying_type>(h.as_underlying_type()) * 3600L} {};

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

  /** Get the seconds as seconds::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /*
    /// @brief Normalize seconds and return the integeral days.
    ///
    /// If the seconds sum up to more (or equal to) one day, remove the integer
    /// days and return them as integer; reset the seconds to seconds of the
    /// new day.
    ///
    /// @return The integer number of days (if the seconds are more than a day).
    /// @throw  Does not throw.
    ///
    constexpr int remove_days() noexcept {
      underlying_type days = m_sec / max_in_day;
      m_sec = m_sec % max_in_day;
      return static_cast<int>(days);
    }

    /// @brief Cast to days.
    ///
    /// If the seconds sum up to more (or equal to) one day, return the
    /// (integral) number of days.
    ///
    /// @return The integer number of days (if the seconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @warning Negative seconds are not handled.
    ///
    constexpr int to_days() const noexcept {
      return static_cast<int>(m_sec / max_in_day);
    }

    /// @brief Seconds as fractional days.
    /// Interpret (cast) the seconds as fractional days; returns a double.
    constexpr double fractional_days() const noexcept {
      return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
    }
  */

  /** Cast to double (i.e. fractional seconds). */
  constexpr double to_fractional_seconds() const noexcept {
    return this->cast_to<double>();
  }

private:
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
 * If the code is compiled with the switch USE_DATETIME_CHECKS, then the
 * milliseconds (constructor) can only have zero or positive values.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
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

  /** The scale factor to transform from seconds to milliseconds. */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1000);
  }

  /** Max milliseconds in one day. */
  static constexpr underlying_type max_in_day = 86400L * 1000L;
  static_assert(max_in_day < std::numeric_limits<underlying_type>::max());

  /** Constructor; default milliseconds is 0. */
  explicit constexpr milliseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /** Constructor from hours, minutes, milliseconds. */
  explicit constexpr milliseconds(hours h, minutes m, milliseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L){};

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

  /** Get the milliseconds as milliseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /** Cast to fractional days. */
  constexpr double fractional_days() const noexcept {
    return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
  }

  /** Cast to fractional dso::seconds */
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) / sec_factor<double>();
  }

  /** Cast to fractional hours */
  constexpr double to_fractional_hours() const noexcept {
    constexpr const underlying_type secinh =
        86400L * sec_factor<underlying_type>();
    return static_cast<double>(m_sec) / static_cast<double>(secinh);
  }

private:
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
 * If the code is compiled with the switch USE_DATETIME_CHECKS, then the
 * microseconds (constructor) can only have zero or positive values.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 *
 * @note microseconds can be cast to dso::seconds and dso::milliseconds (via
 * a static_cast or a C-type cast) but the opposite is not true; i.e.
 * dso::seconds cannot be cast to microseconds.This is still an open question!
 *
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

  /** The scale factor to transform from seconds to microseconds. */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000);
  }

  /** Constructor; default microseconds is 0; any integral number will do */
  explicit constexpr microseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /** Constructor from hours, minutes, microseconds. */
  explicit constexpr microseconds(hours h, minutes m, microseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L){};

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

  /** Cast to microseconds::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /** Cast to fractional seconds */
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) / sec_factor<double>();
  }

private:
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
 * @note nanoseconds can be cast to dso::seconds, dso::milliseconds and
 * dso::microseconds (via a static_cast or a C-type cast) but the opposite is
 * not true; i.e. dso::seconds cannot be cast to nanoseconds.This is still
 * an open question!
 *
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

  /** The scale factor to transform from seconds to nanoseconds. */
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000'000);
  }

  /** Constructor; default nanoseconds is 0. **/
  explicit constexpr nanoseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /** Constructor from hours, minutes, nanoseconds. */
  explicit constexpr nanoseconds(hours h, minutes m, nanoseconds c) noexcept
      : m_sec(c.as_underlying_type() +
              (static_cast<underlying_type>(m.as_underlying_type()) +
               static_cast<underlying_type>(h.as_underlying_type()) * 60L) *
                  sec_factor<underlying_type>() * 60L){};

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

  /** Cast to fractional seconds */
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) / sec_factor<double>();
  }

private:
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

  /** Nanoseconds as long ints. */
  underlying_type m_sec;

}; /* class nanoseconds */

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

/** @todo do we want to allow this?
 *
 * Overload bool operator '==' for comparing datetime fundamental types with
 * integers, aka
 * month m(3);
 * assert(m == 3)
 * This function will be resolved for any type DType, which
 * 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
 * 2. has a member function named DType::__member_const_ref__()
 */
/*
#if __cplusplus >= 202002L
template <gconcepts::is_fundamental_and_has_const_ref DType, std::integral DInt>
#else
template <typename DType,
          typename DInt,
          typename = std::enable_if_t<std::is_integral_v<DInt>>,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_const_ref__)>::value>>
#endif
          constexpr bool operator==(DType a, DInt b) noexcept {
  return a.__member_const_ref__() == static_cast<typename
DType::underlying_type>(b);
}
*/

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
  // this is tricky! We must first compute the numerator and then the fraction.
  // why? check this out
  // seconds _s1 = cast_to<milliseconds, seconds>(milliseconds{2000L});
  // this is: (1/1000)*2000 which is 0 because 1/1000 is 0, but
  // (2000*1)/1000 = 2 which is correct
  const auto numerator = s.__member_ref__() * Strg::template sec_factor<long>();
  return Strg(numerator / Ssrc::template sec_factor<long>());
}

/// For user-defined literals, i am going to replace long with
/// unsigned long long int.
namespace ddetail {
using ulli = unsigned long long int;
}

/// A year can be constructed via "_Y".
constexpr year operator"" _Y(ddetail::ulli i) noexcept {
  return year{static_cast<year::underlying_type>(i)};
}

/// A month can be constructed via "_M".
constexpr month operator"" _M(ddetail::ulli i) noexcept {
  return month{static_cast<month::underlying_type>(i)};
}

/// A day of month can be constructed via "_D".
constexpr day_of_month operator"" _D(ddetail::ulli i) noexcept {
  return day_of_month{static_cast<day_of_month::underlying_type>(i)};
}

/// An hour can be constructed via "_h".
constexpr hours operator"" _h(ddetail::ulli i) noexcept {
  return hours{static_cast<hours::underlying_type>(i)};
}

/// A minute can be constructed via "_m".
constexpr minutes operator"" _m(ddetail::ulli i) noexcept {
  return minutes{static_cast<minutes::underlying_type>(i)};
}

/// Seconds can be constructed via "_sec".
constexpr seconds operator"" _sec(ddetail::ulli i) noexcept {
  return seconds{static_cast<seconds::underlying_type>(i)};
}

/// MilliSeconds can be constructed via "_millisec".
constexpr milliseconds operator"" _millisec(ddetail::ulli i) noexcept {
  return milliseconds{static_cast<milliseconds::underlying_type>(i)};
}

/// MicroSeconds can be constructed via "_microsec".
constexpr microseconds operator"" _microsec(ddetail::ulli i) noexcept {
  return microseconds{static_cast<microseconds::underlying_type>(i)};
}

} /* namespace dso */

#endif
