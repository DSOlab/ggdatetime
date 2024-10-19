/** @file
 *
 * Define date types/classes that are based on integral types (i.e. a year).
 *
 * The following table lists the classes defined here, along with their main
 * (internal) member functions/vars used by generic template function to
 * define their behavior. See core/fundamental_types_generic_utilities.hpp
 * --------------------+----------------+-------------+--------------+
 * Class Name          |is_dt_\         |__member_\   |__member_\    |
 *                     |fundamental_type|const_ref__()|_ref__()      |
 * --------------------+----------------+-------------+--------------+
 * year                | yes            | yes         | yes          |
 * month               | yes            | yes         | yes          |
 * gps_week            | yes            | yes         | yes          |
 * day_of_month        | yes            | yes         | yes          |
 * day_of_year         | yes            | yes         | yes          |
 * ymd_date            | no             | no          | no           |
 * ydoy_date           | no             | no          | no           |
 * modified_julian_day | yes            | yes         | yes          |
 * --------------------+----------------+-------------+--------------+
 *
 */

#ifndef __DSO_DATE_INTEGRAL_TYPES_HPP__
#define __DSO_DATE_INTEGRAL_TYPES_HPP__

#include "core/fundamental_calendar_utils.hpp"
#include "core/fundamental_types_generic_utilities.hpp"

namespace dso {

/* Forward declerations */
class year;
class month;
class gps_week;
class day_of_month;
class day_of_year;
class modified_julian_day;
class ymd_date;
class ydoy_date;

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
 *  - This version only works for post-1972 dates! For a more complete
 *      version, see the iauDat.c routine from IAU's SOFA.
 *  - No checks are performed for the validity of the input date.
 * @see IAU SOFA (iau-dat.c)
 * @see dso::dat
 *
 * @param[in] iy The year
 * @param[in] im The month
 * @return TAI-UTC up to the datetime (\p iy, \p im, 23:59:59)
 */
int dat(year iy, month im) noexcept;
int dat(const ymd_date &ymd) noexcept;

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
 * Overload of dso::core::dat) for MJD.
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
 * A year is not an integer; hence, operations with integral values are not
 * allowed (they will actually triger a compilation error).
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is year.
 *
 * @see core/fundamental_types_generic_utilities.hpp
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
   *
   * Note that the constrcutor is NOT explicit to allow construction from
   * int (aka to allow lines of codes of type: year y = 1901;)
   */
  constexpr year(underlying_type i = 1900) noexcept : m_year(i) {}

#ifdef ALLOW_DT_INTEGRAL_MATH
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
#endif

  /** Get the year as year::underlying_type. */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_year;
  }

  /** Check if year is leap (aka has 366 --integer-- days instead of 365) */
  constexpr bool is_leap() const noexcept { return dso::core::is_leap(m_year); }

  /** Return the corresponding two-digit year as integer */
  constexpr int to_two_digit() const noexcept {
    const int tdy = m_year - 1900;
    return tdy - (tdy >= 100) * 100;
  }

  static constexpr year from_two_digit(int yr) noexcept {
    yr += 1900;
    return year(yr + (yr >= 50) * 100);
  }

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
 * pre-decrement), '+=T' and '-=T' where T is month.
 *
 * @see core/fundamental_types_generic_utilities.hpp
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
   *
   * @warning No check is performed by default for the input value \p i , so
   *   you can practically assign month=123. If you want a validity check, use
   *   the month::is_valid function (after construction).
   */
  explicit constexpr month(underlying_type i = 1) noexcept : m_month(i) {};

  /** @brief Constructor from a c-string.
   *
   * Given a c-string (i.e. null-terminating char array), resolve the month.
   * The c-string can be either a short name (i.e. a 3-character name), e.g.
   * "Jan", or the whole, normal month name e.g. "January".
   * If a 3-char length string is passed in, we are going to compare using the
   * month::short_names array; else if the length of the input string is more
   * than 3-chars, the month::long_names array is used.
   * The function is case-insensitive, i.e. "January", "JANUARY" and "JanUAry"
   * are all considered the same.
   * If the input string cannot be matched to any of the strings in
   * short_names or long_names, then an exception is thrown of type:
   * std::invalid_argument. Note that the month will be returned in the
   * "normal" range [1,12], **not** [0-11].
   *
   * @param[in] str The month's name; The string should match a month in the
   *    month::short_names or month::long_names array. The string should be
   *    null-trerminated.
   *
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
   *
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
  constexpr static const int SHORT_NAMES_LEN =
      sizeof(short_names) / sizeof(short_names[0]);

  /** long month names. */
  constexpr static const char *long_names[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  constexpr static const int LONG_NAMES_LEN =
      sizeof(short_names) / sizeof(short_names[0]);

  /** The month as underlying_type. */
  underlying_type m_month;
}; /* month */

/** @brief A wrapper class for GPS Week.
 *
 * The GPS Week Number count began at approximately midnight on the evening
 * of 05 January 1980 / morning of 06 January 1980. Since that time, the
 * count has been incremented by 1 each week, and broadcast as part of the GPS
 * message.
 *
 * A gps week is represented by just an integer number. There are no limits
 * (excpept from integer overflow) to the range of gps_week, i.e. the week is
 * not checked (by default) to be in any range. So, a user can construct a
 * gps_week from whatever integer. You can use the is_valid member function,
 * but this will only check it the GPS Week is > 0.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is gps_weeek.
 *
 * @see core/fundamental_types_generic_utilities.hpp type.
 */
class gps_week {
public:
  /** gps weeks are represented as long int. */
  typedef long underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

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
  explicit constexpr gps_week(underlying_type i = 1) noexcept : m_week(i) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
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
#endif

  /** Get the month as month::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_week;
  }

  /** Check if the instance is valid, aka if the week is the range [0,+Inf) */
  constexpr bool is_valid() const noexcept { return m_week > 0; }

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
 * A day_of_month is just a plain integer, and there is no limimtation/check
 * on its values. E.g., client code can be:
 * day_of_month day(-1312);
 * which is perfectly valid! If you want to check whether a given isntance is
 * indded valid, you case use the is_valid() function, which expects (also)
 * the month and year, to fully perform the check.
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' where T is either a year or any integral
 * type.
 *
 * @see core/fundamental_types_generic_utilities.hpp
 */
class day_of_month {
public:
  /** Days are represented as ints. */
  typedef int underlying_type;

  /** Is fundamental datetime type */
  static constexpr bool is_dt_fundamental_type = true;

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
  explicit constexpr day_of_month(underlying_type i = 1) noexcept : m_dom(i) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
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
#endif

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
  constexpr bool is_valid(dso::year y, dso::month m) const noexcept {
    if (m_dom < 1 || m_dom >= 32 || (!m.is_valid()))
      return false;
    /* If February in a leap year, 1, otherwise 0 */
    int ly = ((m.as_underlying_type() == 2) && y.is_leap());
    /* Validate day, taking into account leap years */
    return (m_dom <= dso::core::mtab[m.as_underlying_type() - 1] + ly);
  }

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
 *
 * @see core/fundamental_types_generic_utilities.hpp
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
  explicit constexpr day_of_year(underlying_type i = 0) noexcept : m_doy(i) {};

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
  constexpr day_of_year &operator=(Int i) noexcept {
    m_doy = i;
    return *this;
  }
#endif

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
class ymd_date {
public:
  /** @brief ymd_date constructor.
   *
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ymd_date::is_valid
   */
  constexpr ymd_date(year y = year{}, month m = month{},
                     day_of_month d = day_of_month{}) noexcept
      : __year(y), __month(m), __dom(d) {}

  /** @brief Constructor from a year, day of year.
   *
   * This constrcutor will first check to see if the input parameter is valid
   * (via ydoy_data::isvalid()) and then constrcuct the corresponding date as
   * ymd_date instance.
   */
  ymd_date(const ydoy_date &ydoy);

  /** @brief Check if the date is a valid calendar date.
   *
   * @return True if the date is valid, false otherwise.
   */
  constexpr bool is_valid() const noexcept {
    return __dom.is_valid(__year, __month);
  }

  /** operator '==' for ymd_date instances */
  bool operator==(const ymd_date &d) const noexcept {
    return ((__year == d.yr()) && (__dom == d.dm()) && (__month == d.mn()));
  }

  /** operator '!=' for ymd_date instances */
  bool operator!=(const ymd_date &d) const noexcept {
    return !(this->operator==(d));
  }

  /** @brief Transform to year and day-of-year.
   *
   * The function will first check that the instance is a valid date, before
   * performing the transformation (to year and day of year). This is done
   * because an invalid ymd_date can result in a seamingly valid ydoy_date
   * (e.g. constructing a 29/2 date on a non-leap year).
   */
  ydoy_date to_ydoy() const;

  /** get/set year */
  constexpr year &yr() noexcept { return __year; }
  /** get/set month */
  constexpr month &mn() noexcept { return __month; }
  /** get/set day of month */
  constexpr day_of_month &dm() noexcept { return __dom; }
  /** get year */
  constexpr year yr() const noexcept { return __year; }
  /** get month */
  constexpr month mn() const noexcept { return __month; }
  /** get day of month */
  constexpr day_of_month dm() const noexcept { return __dom; }

private:
  year __year;        /** the year */
  month __month;      /** the month */
  day_of_month __dom; /** day of month */
}; /* ymd_date */

/** @brief This struct represent a date in Year-Day of Year format.
 *
 * This struct is only designed to ease the input/output parameters to various
 * functions. Users can actually construct any date, even non-valid ones
 * (e.g. set year to 0). The constructor will not check the input parameters.
 * If users want to check the instance for validity, then they should use the
 * ydoy_date::is_valid function.
 */
class ydoy_date {
public:
  /** @brief ydoy_date constructor.
   *
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ydoy_date::is_valid.
   */
  constexpr ydoy_date(year y = year{}, day_of_year d = day_of_year{}) noexcept
      : __year(y), __doy(d) {}

  /** @brief Constructor from a Year/Month/DayOfMonth instance.
   *
   * In case the input argument ymd is not a valid date, the constructor
   * will throw.
   */
  ydoy_date(const ymd_date &ymd)
      : __year(ymd.yr()), __doy(ymd.to_ydoy().dy()) {}

  /** @brief Check if the date is a valid calendar date.
   *
   * @return True if the date is valid, false otherwise.
   */
  constexpr bool is_valid() const noexcept { return __doy.is_valid(__year); }

  /** @brief Transform to year, month, day-of-month.
   *
   * No validation test performed on the calling instance.
   */
  ymd_date to_ymd() const noexcept;

  /** operator '==' for ydoy_date instances */
  bool operator==(const ydoy_date &d) const noexcept {
    return (__year == d.yr()) && (__doy == d.dy());
  }

  /** operator '!=' for ydoy_date instances */
  bool operator!=(const ydoy_date &d) const noexcept {
    return !(this->operator==(d));
  }

  /** get/set year */
  year &yr() noexcept { return __year; }
  /** get/set day of year */
  day_of_year &dy() noexcept { return __doy; }
  /** get year */
  constexpr year yr() const noexcept { return __year; }
  /** get day of year */
  constexpr day_of_year dy() const noexcept { return __doy; }

private:
  year __year;       /** the year */
  day_of_year __doy; /** day of year */
}; /* ydoy_date */

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
 * A Modified Julian Day is represented by an integral number (there is no
 * fractional part). Thus, a modified_julian_day only represents a date *not*
 * a datetime.
 *
 * The Modified Julian Day, was introduced by space scientists in the late
 * 1950's. It is defined as \f$MJD = JD - 2400000.5\f$ The half day (used in
 * JD) is subtracted so that the day starts at midnight in conformance with
 * civil time reckoning.
 *
 * The MJD is a convenient dating system with only 5 digits, sufficient for
 * most modern purposes. To convert between MJD and JD we need the Julian
 * Date of Modified Julian Date zero, aka MJD0_JD, which is 2400000.5
 *
 * This is a fundamental class, which means it only has one arithmetic member
 * variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
 * '>=') are going to be implemented using kinda reflection, using template
 * function overloadnig outside the class.
 * The same goes for operators '++' (post- pre-increment) and '--' (post-
 * pre-decrement), '+=T' and '-=T' .
 *
 * @see core/fundamental_types_generic_utilities.hpp
 * @see http://tycho.usno.navy.mil/mjd.html
 */
class modified_julian_day {
public:
  /** Decide on the INT (underlying) type of modified_julian_day.
   *
   * underlying_type should be at least as long as int_32_t. Else
   * underlying_type will be a long.
   * A 32bit int, has a range of -2147483648 to 2147483647. Should be enough!
   */
  typedef std::conditional<sizeof(int) >= sizeof(int32_t), int, int64_t>::type
      underlying_type;

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

  /** @brief Max possible modified_julian_day.
   *
   * Note that we are returning the maximum allowed integer here (not
   * long/unsigned, etc..). This is for easy comparisson (i.e. guarding
   * against overflow when comparing with ints).
   */
  constexpr static modified_julian_day max() noexcept {
    return modified_julian_day{std::numeric_limits<int>::max()};
  }

  /** @brief Min possible modified_julian_day.
   *
   * Note that we are return the minimum allowed integer here (not
   * long/unsigned, etc..). This is for easy comparisson (i.e. guarding
   * against overflow when comparing with ints).
   */
  constexpr static modified_julian_day min() noexcept {
    return modified_julian_day{std::numeric_limits<int>::min()};
  }

  /** @brief Constructor; default Modified Julian Day is 1. */
  constexpr explicit modified_julian_day(underlying_type i = 1) noexcept
      : m_mjd(i) {};

  /** @brief Constructor from Year and DayOfYear.
   *
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
      : m_mjd(core::ydoy2mjd(iy.as_underlying_type(),
                             id.as_underlying_type())) {};

  /** @brief Constructor from Year and DayOfYear.
   *
   * The passed in date (year and doy) are tested to see if they represent a
   * valid date. If not, the constructor will throw!
   *
   * @param[in] iy The year.
   * @param[in] id The day of year.
   *
   * @see "Remondi Date/Time Algorithms",
   * http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
   */
  modified_julian_day(const ydoy_date &ydoy)
      : m_mjd(core::ydoy2mjd(ydoy.yr().as_underlying_type(),
                             ydoy.dy().as_underlying_type())) {};

  /** @brief Constructor from  calendar date.
   *
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
                            d.as_underlying_type())) {};

  /** @brief Constructor from  calendar date.
   *
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
  modified_julian_day(const ymd_date &ymd)
      : m_mjd(core::cal2mjd(ymd.yr().as_underlying_type(),
                            ymd.mn().as_underlying_type(),
                            ymd.dm().as_underlying_type())) {};

#ifdef ALLOW_DT_INTEGRAL_MATH
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
#endif

  /** Get the modified_julian_day as modified_julian_day::underlying_type */
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_mjd;
  }

  /** @brief Transform to Julian Day.
   *
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

} /* namespace dso */

#endif
