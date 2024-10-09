/** @file
 * Fundamental (core) datetime constants, algorithms and types.
 * This file contains fundamental constants, algorithms and data structures,
 * for manipulating date and time, targeting Space Geodesy applications.
 * Time scales and calendars are not considered here.
 */

#ifndef __DSO_DATETIME_DTFUND_HPP__
#define __DSO_DATETIME_DTFUND_HPP__

#include "cdatetime.hpp"
#include "datetime_tops.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <cstdint>

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
class hours;
class minutes;
class seconds;
class milliseconds;
class microseconds;
class nanoseconds;

namespace core {
/** Number of days past at the end of non-leap and leap years. */
constexpr const int month_day[2][13] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

/** Month lengths in days */
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
 * The function assumes the TT time-scale (for input and output dates).
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
 * The function assumes the TT time-scale (for input and output dates).
 *
 * Convert a Modified Julian date to a Julian Epoch. The MJD can be given as a
 * single value (i.e. in parameter \p mjd0) or can be split into two parts,
 * (e.g. the first being the intergal part of MJD and the second be fractional
 * day).
 *
 * @param[in] mjd0 The Modified Julian Date (first part or whole number)
 * @param[in] mjd1 Second part of MJD (if any), such that MJD = mjd0 + mjd1
 * @return The input date as Julian Epoch.
 */
inline constexpr double mjd2epj(double mjd0, double mjd1 = 0e0) noexcept {
  return 2000e0 + ((mjd0 - J2000_MJD) + mjd1) / DAYS_IN_JULIAN_YEAR;
}

/** @brief Julian Epoch to Modified Julian Date
 * The function assumes the TT time-scale (for input and output dates).
 *
 * @param[in] epj The Julian Epoch to convert
 * @return The corresponding (fractional) Modified Julian Date
 *
 * @see IAU SOFA epj2jd
 */
inline constexpr double epj2mjd(double epj) noexcept {
  return J2000_MJD + (epj - 2000e0) * DAYS_IN_JULIAN_YEAR;
}

/** @brief Julian Epoch to two-part Modified Julian Date
 *
 * This function returns the correponding MJD the input Julian Epoch as a
 * two-part MJD, where the first, "big" part is the MJDay and the
 * rest ("small" part) is returned in the parameter \p fday, representing the
 * fractional part of the MJday.
 * So, the actual MJD = BigPart (i.e. Day) + SmallPart (i.e. fraction of day).
 * This is meant to better preserve accuracy and convieniently place the
 * result in a TwoPartDate.
 * The function assumes the TT time-scale (for input and output dates).
 *
 * @param[in] epj The Julian Epoch to convert
 * @param[out] fday Fractional part of MJDay
 * @return Integral part of MJD, i.e. the MJDay, such the the MJD
 *         corresponding to the input Julian Epoch is
 *         MJD = BigPart + SmallPart
 */
inline double epj2mjd(double epj, double &fday) noexcept {
  /* lose the .5 part */
  const double ipart = static_cast<int>(J2000_MJD);
  fday = (epj - 2000e0) * DAYS_IN_JULIAN_YEAR + .5e0;
  double iextra;
  fday = std::modf(fday, &iextra);
  return ipart + iextra;
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

  explicit FractionalSeconds(double _fsec = 0e0) noexcept : fsec(_fsec) {};
  double seconds() const noexcept { return fsec; }
  double &seconds() noexcept { return fsec; }
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

  explicit FractionalDays(double _fdays = 0e0) noexcept : fdays(_fdays) {};
  double days() const noexcept { return fdays; }
  double &days() noexcept { return fdays; }
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
  double years() const noexcept { return fyears; }
  double &years() noexcept { return fyears; }
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

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
FractionalSeconds to_fractional_seconds(S nsec) noexcept {
  const double sec = nsec.S::template cast_to<double>() * S::sec_inv_factor();
  return FractionalSeconds(sec);
}

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
  explicit constexpr month(underlying_type i = 1) noexcept : m_month(i) {};

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
class ymd_date {
public:
  /** @brief ymd_date constructor
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ymd_date::is_valid
   */
  constexpr ymd_date(year y = year{}, month m = month{},
                     day_of_month d = day_of_month{}) noexcept
      : __year(y), __month(m), __dom(d) {}

  ymd_date(const ydoy_date &ydoy);

  /** @brief Check if the date is a valid calendar date
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

  /** @brief Transform to year and day-of-year
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
 * ymd_date::is_valid function.
 */
class ydoy_date {
public:
  /** @brief ymd_date constructor
   * No check for validity will be performed. If you want to check the
   * validity of the created instance, use ymd_date::is_valid
   */
  constexpr ydoy_date(year y = year{}, day_of_year d = day_of_year{}) noexcept
      : __year(y), __doy(d) {}

  /** @brief Constructor from a Year/Month/DayOfMonth instance
   * In case the input argument \p ymd is not a valid date, the constructor
   * will throw.
   */
  ydoy_date(const ymd_date &ymd)
      : __year(ymd.yr()), __doy(ymd.to_ydoy().dy()) {}

  /** @brief Check if the date is a valid calendar date
   * @return True if the date is valid, false otherwise.
   */
  constexpr bool is_valid() const noexcept { return __doy.is_valid(__year); }

  /** @brief Transform to year, month, day-of-month */
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

  /** @brief Max possible modified_julian_day
   *
   * Note that we are return the maximum allowed integer here (not
   * long/unsigned, etc..). This is for easy comparisson (i.e. guarding
   * against overflow when comparing with ints).
   */
  constexpr static modified_julian_day max() noexcept {
    return modified_julian_day{std::numeric_limits<int>::max()};
  }

  /** @brief Min possible modified_julian_day
   *
   * Note that we are return the minimum allowed integer here (not
   * long/unsigned, etc..). This is for easy comparisson (i.e. guarding
   * against overflow when comparing with ints).
   */
  constexpr static modified_julian_day min() noexcept {
    return modified_julian_day{std::numeric_limits<int>::min()};
  }

  /** Constructor; default Modified Julian Day is 1.
   * This is a non-explicit constructor, hence we can perform:
   * modified_julian_day mjd = 123456;
   */
  constexpr modified_julian_day(underlying_type i = 1) noexcept : m_mjd(i) {};

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
      : m_mjd(core::ydoy2mjd(iy.as_underlying_type(),
                             id.as_underlying_type())) {};
  modified_julian_day(const ydoy_date &ydoy)
      : m_mjd(core::ydoy2mjd(ydoy.yr().as_underlying_type(),
                             ydoy.dy().as_underlying_type())) {};

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
                            d.as_underlying_type())) {};
  modified_julian_day(const ymd_date &ymd)
      : m_mjd(core::cal2mjd(ymd.yr().as_underlying_type(),
                            ymd.mn().as_underlying_type(),
                            ymd.dm().as_underlying_type())) {};

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
  explicit constexpr hours(underlying_type i = 0) noexcept : m_hours(i) {};

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
  explicit constexpr minutes(underlying_type i = 0) noexcept : m_min(i) {};

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

  /** The scale factor to transform from seconds to seconds. */
  static constexpr double sec_inv_factor() noexcept { return 1e0; }

  /** Constructor; default seconds is 0, but any integral will do */
  explicit constexpr seconds(underlying_type i = 0) noexcept : m_sec(i) {};

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

namespace dtextra {

/** @brief A wrapper class for picoseconds (i.e 10**-12 sec.).
 *
 * Please revert from using this class if possible! It cannot hold negative
 * numbers hence, arithmetic operations are dangerous!
 */
class picoseconds {
public:
  /** Picoseconds are represented as long integers. */
  typedef uint_fast64_t underlying_type;

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

private:
  /** Picoseconds as long ints. */
  underlying_type m_sec;
}; /* class picoseconds */

} /* namespace dtextra */

} /* namespace dso */

#endif
