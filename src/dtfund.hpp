///
/// @file  dtfund.hpp
///
/// @brief Fundamental (core) datetime constants, algorithms and types.
///
/// This file contains fundamental constants, algorithms and data structures,
/// for manipulating date and time, targeting GNSS applications.
///
/// @author xanthos
///
/// @bug No known bugs.
///
/// @date Wed 08 May 2019 11:26:07 AM EEST (last modified)
///

#ifndef __DTFUND_NGPT__HPP__
#define __DTFUND_NGPT__HPP__

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <tuple>
#ifdef DEBUG
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#endif

namespace dso {

#if __cplusplus >= 202002L
namespace gconcepts {
template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;
} // namespace gconcepts
#endif

/// Check if long is big enough to hold two days in microseconds.
static_assert(86400L * 1'000'000'000L * 2 < std::numeric_limits<long>::max(),
              "FUCK! Long is not big enough to hold two days in microseconds");

/// Jan 1st 1980 for GPS Time
constexpr long jan61980{44244L};

constexpr long jan11901{15385L};

/// Seconds per day.
/// @warning This is not always true in case of UTC dates; the day a leap second
///          is inserted has one more second!
constexpr double sec_per_day{86400e0};

/// Days per Julian year.
constexpr double days_in_julian_year{365.25e0};

/// Days per Julian century.
constexpr double days_in_julian_cent{36525e0};

/// Reference epoch (J2000.0), Julian Date.
constexpr double j2000_jd{2451545e0};

/// Reference epoch (J2000.0), Modified Julian Date.
constexpr double j2000_mjd{51544.5e0};

/// Julian Date of Modified Julian Date zero.
constexpr double mjd0_jd{2400000.5e0};

/// TT minus TAI in seconds.
constexpr double tt_minus_tai{32.184e0};

/// Forward declerations
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

/// @brief Calendar date to Modified Julian Day.
/// Given a calendar date (i.e. year, month and day of month), compute the
/// corresponding Modified Julian Day. The input date is checked and an
/// exception is thrown if it is invalid.
///
/// @param[in] iy The year (int).
/// @param[in] im The month (int).
/// @param[in] id The day of month (int).
/// @return    The Modified Julian Date (as long).
/// @throw     A runtime_error if the month and/or day is invalid.
///
/// @note The algorithm used is valid from -4800 March 1
///       Declared and defined here cause its constexpr.
///
/// @see dso::cal2mjd
///
/// Reference: iauCal2jd
constexpr long cal2mjd(int iy, int im, int id) {
  // Month lengths in days
  constexpr int mtab[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Validate month
  if (im < 1 || im > 12) {
    // throw in runtime, fail to compile at compile-time
    throw std::out_of_range("dso::cal2mjd -> Invalid Month.");
  }

  // If February in a leap year, 1, otherwise 0
  int ly = ((im == 2) && !(iy % 4) && (iy % 100 || !(iy % 400)));

  // Validate day, taking into account leap years
  if ((id < 1) || (id > (mtab[im - 1] + ly))) {
    throw std::out_of_range("dso::cal2mjd -> Invalid Day of Month.");
  }

  // Compute mjd
  int my{(im - 14) / 12};
  long iypmy{static_cast<long>(iy + my)};

  return (1461L * (iypmy + 4800L)) / 4L +
         (367L * static_cast<long>(im - 2 - 12 * my)) / 12L -
         (3L * ((iypmy + 4900L) / 100L)) / 4L + static_cast<long>(id) -
         2432076L;
}

/// @brief Check if year is leap.
///
/// @param[in] iy The year to check (int).
/// @return true if year is leap, false otherwise.
///
/// @throw Does not throw.
constexpr bool is_leap(int iy) noexcept {
  return !(iy % 4) && (iy % 100 || !(iy % 400));
}

/// Convert a pair of Year, Day of year to MJDay.
/// Convert a pair of year, day_of_year to a modified_julian_day. No check is
/// performed whatsoever, at the input arguments (e.g. to see if indeed
/// the given doy is within a valid range).
constexpr long ydoy2mjd(long iyr, long idoy) noexcept {
  return ((iyr - 1901) / 4) * 1461 + ((iyr - 1901) % 4) * 365 + idoy - 1 +
         dso::jan11901;
}

/// @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
///
/// The day of month is actually not needed, since all leap second insertions
/// happen at the begining, i.e. the first day of a month.
///
/// @note In case using MJD (and not calendar date) is more convinient, use the
///       overloaded function dso::dat
///
/// @warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// @see IAU SOFA (iau-dat.c)
/// @see dso::dat
int dat(year iy, month im) noexcept;

/// @overload dso::dat(dso::year iy, dso::month im) noexcept
///
/// @note In case using calendar date (and not MJD) is more convinient, use the
///       overloaded function dso::dat
///
/// @warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// @see IAU SOFA (iau-dat.c)
/// @see dso::dat
int dat(modified_julian_day mjd) noexcept;

constexpr void mjd2ymd(long mjd, int &iyear, int &imonth, int &idom) noexcept {
  // Express day in Gregorian calendar
  long l = mjd + (68569L + 2400000L + 1);
  long n = (4L * l) / 146097L;
  l -= (146097L * n + 3L) / 4L;
  long i = (4000L * (l + 1L)) / 1461001L;
  l -= (1461L * i) / 4L - 31L;
  long k = (80L * l) / 2447L;

  idom = l - (2447L * k) / 80L;
  l = k / 11L;
  imonth = k + 2L - 12L * l;
  iyear = 100L * (n - 49L) + i + l;

  return;
}

/// @class year
/// @brief A wrapper class for years.
///
/// A year is represented by just an integer number. There are no limits
/// (excpept from integer overflow) to the range of the year (integer).
///
/// A year is not an integer; hence, operations with integral values (aka
/// addition, subtraction, etc) are not allowed (they will actually triger a
/// compilation error). The only thing that is allowed, is assigning from
/// integral types.
/// @code
///   year yr {2019};
///   yr = year + 1; // error!
///   yr = 2018;     // ok
/// @endcode
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
/// The same goes for operators '++' (post- pre-increment) and '--' (post-
/// pre-decrement), '+=T' and '-=T' where T is either a year or any integral
/// type.
///
/// @example test_year.cpp
class year {
public:
  /// Years are represented as integers.
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  /// @return reference to the instance's member variable (as
  /// year::underlying_type)
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_year;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  /// @return const reference to the instance's member variable (as
  ///         year::underlying_type)
  constexpr underlying_type &__member_ref__() noexcept { return m_year; }

  /// Constructor; default year is 1900.
  /// Note that the constrcutor is NOT explicit to allow construction from
  /// int (aka to allow lines of codes of type: year y = 1901;)
  constexpr year(underlying_type i = 1900) noexcept : m_year(i) {}

/// Overload operator '=' where the the right-hand-side is any integral type.
/// @tparam  I any integral type, aka any type for which
///          std::is_integral_v<I> is true
/// @param   _intv Any integral value; set the instance's value equal to this
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

  /// Get the year as year::underlying_type.
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_year;
  }

  /// Check if year is leap (aka has 366 --integer-- days instead of 365)
  constexpr bool is_leap() const noexcept { return dso::is_leap(m_year); }

private:
  /// The year as underlying type.
  underlying_type m_year;

}; // year

/// @class month
/// @brief A wrapper class for months.
///
/// A month is represented by just an integer number. There are no limits
/// (excpept from integer overflow) to the range of the month (integer), i.e.
/// the month is not checked (by default) to be in the range [1,12]. So, a user
/// can construct a month from whatever integer, e.g. :
/// month m (-200);
/// will work fine! To check if the month is within the range [1,12], use the
/// month::is_valid method.
/// Most functions (within dso) accept months in the range [1,12]; do not
/// use the range [0,11], except if you realy know what you're doing.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
/// The same goes for operators '++' (post- pre-increment) and '--' (post-
/// pre-decrement), '+=T' and '-=T' where T is either a year or any integral
/// type.
///
/// @warning Most functions (within dso) accept months in the range [1,12];
///          do not use the range [0,11], except if you realy know what you're
///          doing.
///
/// @example test_month.cpp
class month {
public:
  /// Months are represented as int.
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_month;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_month; }

  /// Constructor; default month is 1.
  /// This is an explicit constructor, we do not want users to be able to do
  /// month m = 1;
  explicit constexpr month(underlying_type i = 1) noexcept : m_month(i){};

  /// @brief Constructor from a c-string.
  ///
  /// @param[in] str The month's name; The string should match a month in the
  ///                month::short_names or month::long_names array.
  /// @throw         An std::invalid_argument exception is thrown if a) no
  ///                match is found, or b) the input string is too short.
  /// @note The matching is performed in a case-insensitive way, aka "JUL",
  /// "Jul"
  ///       "JULY", etc, will all be matched to the month July.
  explicit month(const char *str);

  /// Get the month as month::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_month;
  }

  /// Return the corresponding short name (i.e. 3-char name) e.g. "Jan".
  /// @return Returns a pointer to the class's (static member)
  /// month::short_names
  ///         string array.
  /// @warning No check is performed for the validity of the month; if the
  ///          month is not valie (e.g. it is not in range [0,12], then the
  ///          function has undefined behaviour
  /// @todo maybe throw here, instead of UB
  const char *short_name() const noexcept { return short_names[m_month - 1]; }

  /// Return the corresponding long name (i.e. normal month name) e.g.
  /// "January".
  /// @return Returns a pointer to the class's (static member) month::long_names
  ///         string array.
  /// @warning No check is performed for the validity of the month; if the
  ///          month is not valie (e.g. it is not in range [0,12], then the
  ///          function has undefined behaviour
  /// @todo maybe throw here, instead of UB
  const char *long_name() const noexcept { return long_names[m_month - 1]; }

  /// Check if the month is within the interval [1,12].
  constexpr bool is_valid() const noexcept {
    return m_month > 0 && m_month <= 12;
  }

private:
  /// @brief Decleration of short month names.
  /// @note  Note that we do need a definition in the .cpp file.
  constexpr static const char *short_names[] = {"Jan", "Feb", "Mar", "Apr",
                                                "May", "Jun", "Jul", "Aug",
                                                "Sep", "Oct", "Nov", "Dec"};

  /// @brief  Decleration of long month names.
  /// @note   Note that we do need a definition in the .cpp file.
  constexpr static const char *long_names[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};

  /// The month as underlying_type.
  underlying_type m_month;

}; // month

/// @class gps_week
/// @brief A wrapper class for GPS Week.
///
/// The GPS Week Number count began at approximately midnight on the evening
/// of 05 January 1980 / morning of 06 January 1980. Since that time, the
/// count has been incremented by 1 each week, and broadcast as part of the GPS
/// message.
/// A gps week is represented by just an integer number. There are no limits
/// (excpept from integer overflow) to the range of the month (integer), i.e.
/// the week is not checked (by default) to be in any range. So, a user
/// can construct a gps_week from whatever integer.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
/// The same goes for operators '++' (post- pre-increment) and '--' (post-
/// pre-decrement), '+=T' and '-=T' where T is either a year or any integral
/// type.
///
/// @example test_gps_week.cpp
class gps_week {
public:
  /// gps weeks are represented as int.
  typedef long underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_week;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_week; }

  /// Constructor; default week is 1.
  /// This is an explicit constructor, we do not want users to be able to do
  /// gps_week w = 1;
  explicit constexpr gps_week(underlying_type i = 1) noexcept : m_week(i){};

/// Overload operator '=' where the the right-hand-side is any integral type.
/// @tparam  I any integral type, aka any type for which
///          std::is_integral_v<I> is true
/// @param   _intv Any integral value; set the instance's value equal to this
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

  /// Get the month as month::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_week;
  }

  /// Check if the instance is valid, aka if the week is the range [0,+Inf)
  constexpr bool is_valid() const noexcept { return m_week >= 0; }

private:
  /// The month as underlying_type.
  underlying_type m_week;

}; // gps_week

/// @class day_of_month
/// @brief A wrapper class for day of month.
///
/// A day_of_month is just an integer, representing the day of month. No limits
/// are set though, so the user can construct a day_of_month from whatever
/// integer. Inluding negative numbers!
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
/// The same goes for operators '++' (post- pre-increment) and '--' (post-
/// pre-decrement), '+=T' and '-=T' where T is either a year or any integral
/// type.
///
/// @example test_day_of_month.cpp
class day_of_month {
public:
  /// Days are represented as ints.
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_dom;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_dom; }

  /// Constructor; default day of month is 1.
  explicit constexpr day_of_month(underlying_type i = 1) noexcept : m_dom(i){};

/// Overload operator '=' where the the right-hand-side is any integral type.
/// @tparam  I any integral type, aka any type for which
///          std::is_integral_v<I> is true
/// @param   _intv Any integral value; set the instance's value equal to this
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

  /// Get the day_of_month as day_of_month::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_dom;
  }

  /// Check if a given instance is valid.
  /// Validate a given day_of_month. To do this, we obviously need the month the
  /// dom refers to (to see how many day the month actualy has) and the year, to
  /// check if it is leap or not.
  /// @param[in] y  The year the dom referes to (will check for leap)
  /// @param[in] m  The month the dom refers to; range [1,12]
  /// @return       If the dom is valid (considering the year and month) true
  ///               is returned; else, the function will return false.
  constexpr bool is_valid(dso::year y, dso::month m) const noexcept {
    if (m_dom <= 0 || m_dom >= 32)
      return false;
    if (!m.is_valid())
      return false;

    auto iy = y.as_underlying_type();
    auto im = m.as_underlying_type();

    // Month lengths in days
    constexpr int mtab[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // If February in a leap year, 1, otherwise 0
    int ly((im == 2) && is_leap(iy));

    // Validate day, taking into account leap years
    return (m_dom <= mtab[im - 1] + ly);
  }

private:
  /// The day of month as underlying_type.
  underlying_type m_dom;

}; // day_of_month

/// @class day_of_year
/// @brief A wrapper class for day of year.
///
/// A day of year (doy) is represented by an integer; any integer will do, no
/// automatic check is performed to see if the doy is within the range
/// [1,365/6]. In fact, one can actually use a negative integer to represent
/// a doy. It the user wishes to check wether the given doy is indeed valid,
/// then the method day_of_year::validate() can be used.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
///
/// @example test_day_of_month.cpp
class day_of_year {
public:
  /// Day of year represented as int.
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_doy;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_doy; }

  /// Constructor; default day of year is 0.
  explicit constexpr day_of_year(underlying_type i = 0) noexcept : m_doy(i){};

/// assignment operator from any integral type
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

  /// Cast to underlying type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_doy;
  }

  /// Check validity (doy must belong to a year to check this)
  /// A day of yeay is valid if it is in the range [0,365] or [0,366] for
  /// leap years.
  constexpr bool is_valid(year y) const noexcept {
    return m_doy > 0 && m_doy < (365 + y.is_leap() + 1);
  }

private:
  /// The day_of_year as day_of_year::underlying_type.
  underlying_type m_doy;
}; // day_of_year

/// @struct ymd_date
/// @brief This struct represent a date in Year-Month-Day of Month format.
///
/// This struct is only designed to ease the input/output parameters to various
/// functions. Users can actually construct any date, even non-valid ones
/// (e.g. set month to 0). The constructor will not check the input parameters.
/// If users want to check the instance for validity, then they should use the
/// ymd_date::is_valid function.
struct ymd_date {
  /// @brief ymd_date constructor; can have any number of arguments from 0 to 3.
  /// No check for validity will be performed. If you want to check the
  /// validity of the created instance, use ymd_date::is_valid
  constexpr ymd_date(year y = year{}, month m = month{},
                     day_of_month d = day_of_month{}) noexcept
      : __year(y), __month(m), __dom(d) {}

  /// @brief Check if the date is a valid calendar date
  /// @return True if the date is valid, false otherwise.
  constexpr bool is_valid() const noexcept {
    return __dom.is_valid(__year, __month);
  }

  /// @brief Transform to year and day-of-year
  constexpr ydoy_date to_ydoy() const noexcept;

  year __year;        ///< the year
  month __month;      ///< the month
  day_of_month __dom; ///< day of month
};                    // ymd_date

/// @class modified_julian_day
/// @brief A wrapper class for Modified Julian Day.
///
/// A Modified Julian Day is represented by a long integer (there is no
/// fractional part). Thus, a modified_julian_day only represents a date *not*
/// a datetime.
/// The Modified Julian Day, was introduced by space scientists in the late
/// 1950's. It is defined as \f$MJD = JD - 2400000.5\f$ The half day (used in
/// JD) is subtracted so that the day starts at midnight in conformance with
/// civil time reckoning.
/// The MJD is a convenient dating system with only 5 digits, sufficient for
/// most modern purposes. To convert between MJD and JD we need the Julian
/// Date of Modified Julian Date zero, aka dso::mjd0_jd, which is 2400000.5
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
/// The same goes for operators '++' (post- pre-increment) and '--' (post-
/// pre-decrement), '+=T' and '-=T' where T is either a year or any integral
/// type.
/// Operators "modified_julian_day::operator+(modified_julian_day)" and
/// respectively for operator "-" are also defined, so users can add/subtract
/// modified_julian_day instances.
///
/// @see http://tycho.usno.navy.mil/mjd.html
/// @example test_modified_julian_day.cpp
class modified_julian_day {
public:
  /// MJDs are represented as long ints.
  typedef long underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_mjd;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_mjd; }

  /// Max possible modified_julian_day
  constexpr static modified_julian_day max() noexcept {
    return modified_julian_day{std::numeric_limits<underlying_type>::max()};
  }

  /// Min possible modified_julian_day
  constexpr static modified_julian_day min() noexcept {
    return modified_julian_day{std::numeric_limits<underlying_type>::min()};
  }

  /// Constructor; default Modified Julian Day is 1.
  /// This is a non-explicit constructor, hence we can perform:
  /// modified_julian_day mjd = 123456;
  constexpr modified_julian_day(underlying_type i = 1) noexcept : m_mjd(i){};

  /// @brief Constructor from Year and DayOfYear.
  ///
  /// @param[in] iy The year.
  /// @param[in] id The day of year.
  ///
  /// @see "Remondi Date/Time Algorithms",
  /// http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
  constexpr modified_julian_day(year iy, day_of_year id) noexcept
      : m_mjd{ydoy2mjd(iy.as_underlying_type(), id.as_underlying_type())} {};

  /// @brief Constructor from  calendar date
  ///
  /// @param[in] y The year.
  /// @param[in] m The month.
  /// @param[in] d The day of month
  ///
  /// @see "Remondi Date/Time Algorithms",
  /// http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
  constexpr modified_julian_day(year y, month m, day_of_month d)
      : m_mjd{cal2mjd(y.as_underlying_type(), m.as_underlying_type(),
                      d.as_underlying_type())} {};

/// Overload operator '=' where the the right-hand-side is any integral type.
/// @tparam  I any integral type, aka any type for which
///          std::is_integral_v<I> is true
/// @param   _intv Any integral value; set the instance's value equal to this
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

  /// Get the modified_julian_day as modified_julian_day::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_mjd;
  }

  /// Operator - (subtraction) between two modified_julian_day instances
  constexpr modified_julian_day
  operator-(modified_julian_day mjd) const noexcept {
    return modified_julian_day{m_mjd - mjd.m_mjd};
  }

/// Prohibit implicit conversion in operator '-' (subtraction).
/// Why have this ? Well, because the constructor is not explicit, and we
/// have defined the "modified_julian_day::operator-(modified_julian_day)",
/// that enables the expression:
/// modified_julian_day dm2 {...};
/// dm2 = dm2 - 1;
/// modified_julian_day dm21 = dm1 - 1;
/// via implicit conversion of the right-hand-side to a modified_julian_day
/// instance (from an int/IntegralType). We define this template function
/// here, to prohibit this behaviour, aka the implicit conversion when using
/// the "-" operator. Now the "-" can only work if the right-hand-side is
/// a modified_julian_day instance.
///
/// @tparam     I    Any integral type
/// @param[in] _intv Any instance of type I
#if __cplusplus >= 202002L
  template <typename I>
  requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr modified_julian_day operator-(I _intv) const noexcept = delete;

  /// Operator + (addition) between two modified_julian_day instances
  constexpr modified_julian_day
  operator+(modified_julian_day mjd) const noexcept {
    return modified_julian_day{m_mjd + mjd.m_mjd};
  }

/// Prohibit implicit conversion in operator '+' (addition).
/// Why have this ? Well, because the constructor is not explicit, and we
/// have defined the "modified_julian_day::operator+(modified_julian_day)",
/// that enables the expression:
/// modified_julian_day dm2 {...};
/// dm2 = dm2 + 1;
/// modified_julian_day dm21 = dm1 + 1;
/// via implicit conversion of the right-hand-side to a modified_julian_day
/// instance (from an int/IntegralType). We define this template function
/// here, to prohibit this behaviour, aka the implicit conversion when using
/// the "+" operator. Now the "+" can only work if the right-hand-side is
/// a modified_julian_day instance.
///
/// @tparam     I    Any integral type
/// @param[in] _intv Any instance of type I
#if __cplusplus >= 202002L
  template <typename I>
  requires std::integral<I>
#else
  template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
#endif
  constexpr modified_julian_day operator+(I _intv) const noexcept = delete;

  /// Transform to Julian Day
  /// The Julian Day is returned as a double; computed by the formula:
  /// MJD = JD − 2400000.5
  /// see https://en.wikipedia.org/wiki/Julian_day
  constexpr double to_julian_day() const noexcept {
    return static_cast<double>(m_mjd) + mjd0_jd;
  }

  /// @brief Convert a Modified Julian Day to Year and Day of year.
  ///
  /// @return A tuple with two elements: (year, day_of_year)
  /// @throw  Does not throw.
  ///
  /// @warning No check if performed to see if the resulting day of year is
  ///          valid! If you want to be sure, check the returned value(s).
  ///
  /// @see    "Remondi Date/Time Algorithms",
  ///          http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
  constexpr ydoy_date to_ydoy() const noexcept;

  /// @brief Convert a Modified Julian Day to Calendar Date.
  ///
  /// @return A tuple with three elements: (year, month, day_of_month)
  /// @throw  Does not throw.
  ///
  /// @warning No check if performed to see if the resulting month and day of
  ///          month is valid! If you want to be sure, check the returned
  ///          value(s).
  ///
  /// @see    "Remondi Date/Time Algorithms",
  ///          http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
  /// @todo change return type
  // std::tuple<year, month, day_of_month>
  constexpr ymd_date to_ymd() const noexcept {
    ymd_date ymd;
    mjd2ymd(m_mjd, ymd.__year.__member_ref__(), ymd.__month.__member_ref__(),
            ymd.__dom.__member_ref__());
    return ymd;
  }

private:
  /// The modified julian day as underlying type.
  underlying_type m_mjd;

}; // modified_julian_day

/// @struct ydoy_date
/// @brief This struct represent a date in Year-Day of Year format.
///
/// This struct is only designed to ease the input/output parameters to various
/// functions. Users can actually construct any date, even non-valid ones
/// (e.g. set year to 0). The constructor will not check the input parameters.
/// If users want to check the instance for validity, then they should use the
/// ymd_date::is_valid function.
struct ydoy_date {
  /// @brief ymd_date constructor; can have any number of arguments from 0 to 2.
  /// No check for validity will be performed. If you want to check the
  /// validity of the created instance, use ymd_date::is_valid
  constexpr ydoy_date(year y = year{}, day_of_year d = day_of_year{}) noexcept
      : __year(y), __doy(d) {}

  /// @brief Check if the date is a valid calendar date
  /// @return True if the date is valid, false otherwise.
  constexpr bool is_valid() const noexcept { return __doy.is_valid(__year); }

  /// @brief Transform to year, month, day-of-month
  constexpr ymd_date to_ymd() const noexcept;

  year __year;       ///< the year
  day_of_year __doy; ///< day of year
};                   // ydoy_date

/// @brief A wrapper class for hours.
///
/// An hour is just an integer number representing hours; there is no
/// 'valid range' for hours, meaning that any integer value will do!
/// No checks are provided for hours cause, well, i don't know what to check.
/// Note however, that hours are always integer; there is *no fractional part*.
/// hours are designed to be part of a (date)time system of type
/// hours/minutes/(milli|nano|...)seconds, so they only represent integeral
/// hours
/// -- normally hour of day, although no such restriction exists --.
/// In case a subdivision is needed (e.g. minutes, seconds, etc), then use
/// the corresponsing classes (ndso::minutes, dso::seconds, etc...).
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// hours (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
class hours {
public:
  /// Hours are represented by ints.
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_hours;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_hours; }

  /// Constructor; default hours is 0.
  explicit constexpr hours(underlying_type i = 0) noexcept : m_hours(i){};

/// assignment operator from any integral type
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

  /*
  /// @brief Equality operator; right-hand-side can be an instance of hours, or
  ///        any integral value.
  ///        Example:
  ///        dso::hours h(10);
  ///        assert(h == dso::hours(10));
  ///        assert(h == 10);
  #if __cplusplus >= 202002L
  template<typename T> requires std::integral<T> || std::same_as<T, hours>
  #else
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T> ||
                                                    std::is_same_v<T, hours>>>
  #endif
  constexpr bool operator==(T hr) const noexcept {
    if constexpr (std::is_same_v<T,hours>)
      return m_hours == hr.m_hours;
    else
      return m_hours == hr;
  }

  /// @brief InEquality operator; right-hand-side can be an instance of hours,
  or
  ///        any integral value.
  ///        Example:
  ///        dso::hours h(10);
  ///        assert(h != dso::hours(10));
  ///        assert(h != 10);
  #if __cplusplus >= 202002L
  template<typename T> requires std::integral<T> || std::same_as<T, hours>
  #else
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T> ||
                                                    std::is_same_v<T, hours>>>
  #endif
  constexpr bool operator==(T hr) const noexcept {
    return !(*this == hr);
  }
  */

  /// Get the hours as hours::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_hours;
  }

  /// Remove whole days and only keep the remainding hours.
  ///
  /// This function will actually divide the hours with 24, return the
  /// quatient and keep the remainder. After the function invocation, the
  /// hours will be:
  /// hours <- hours % 24
  ///
  /// @return The whole days (priorly) included within the instance, aka
  ///         hours / 24 (integer division)
  constexpr int remove_days() noexcept {
    int days = m_hours / 24;
    m_hours = m_hours % 24;
    return days;
  }

private:
  /// The hours as hours::underlying_type
  underlying_type m_hours;

}; // hours

/// @brief A wrapper class for minutes.
///
/// A minutes is just an integer number representing minutes; there is no
/// 'valid range', meaning that any integer value will do!
/// No checks are provided (for the minute range) cause, well, i don't know
/// what to check.
/// Note however, that minutes are always integer; there is *no fractional
/// part*. They are designed to be part of a (date)time system of type
/// hours/minutes/(milli|nano|...)seconds, so they only represent integeral
/// minutes -- normally min of hours, although no such restriction exists --.
/// In case a subdivision is needed (e.g. seconds, milliseconds etc), then use
/// the corresponsing classes (dso::seconds, dso::milliseconds, etc...).
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// minutes (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
class minutes {
public:
  /// Minutes are represented by ints
  typedef int underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_min;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_min; }

  /// Constructor
  explicit constexpr minutes(underlying_type i = 0) noexcept : m_min(i){};

/// assignment operator from any integral type
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

  /*
  /// @brief Equality operator; right-hand-side can be an instance of minutes,
  ///        or any integral value.
  ///        Example:
  ///        dso::minutes m(10);
  ///        assert(m == dso::minutes(10));
  ///        assert(m == 10);
  #if __cplusplus >= 202002L
  template<typename T> requires std::integral<T> || std::same_as<T, minutes>
  #else
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T> ||
                                                    std::is_same_v<T, minutes>>>
  #endif
  constexpr bool operator==(T mn) const noexcept {
    if constexpr (std::is_same_v<T,minutes>)
      return m_min == mn.m_min;
    else
      return m_min == mn.m_min;
  }

  /// @brief InEquality operator; right-hand-side can be an instance of minutes,
  ///        or any integral value.
  ///        Example:
  ///        dso::minutes m(10);
  ///        assert(m != dso::minutes(10));
  ///        assert(m != 10);
  #if __cplusplus >= 202002L
  template<typename T> requires std::integral<T> || std::same_as<T, minutes>
  #else
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T> ||
                                                    std::is_same_v<T, minutes>>>
  #endif
  constexpr bool operator==(T mn) const noexcept {
    return !(*this == mn);
  }
  */

  /// Get the minutes as minutes::underlying_type
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_min;
  }

  /// Remove whole hours and only keep the remainding minutes.
  ///
  /// This function will actually divide the minutes with 60, return the
  /// quatient and keep the remainder. After the function invocation, the
  /// minutes will be:
  /// minutes <- minutes % 60
  ///
  /// @return The whole hours (priorly) included within the instance, aka
  ///         minutes / 60 (integer division)
  ///
  /// @note Cannot be constexpr cause of non-‘constexpr’ function
  ///       ‘div_t div(int, int)’, at least in g++.
  hours remove_hours() noexcept {
    int hrs = m_min / 60;
    m_min = m_min % 60;
    return hours{hrs};
  }

private:
  /// The minutes as underlying type.
  underlying_type m_min;

}; // minutes

/// @brief A wrapper class for seconds.
///
/// seconds is just a wrapper class around long integer numbers, i.e. a second
/// is just a long int and can be either positive or negative. Users are however
/// restricted by integer overflow. The maximum number of days that can be
/// expressed in seconds without fear of overflow is given by the template
/// function dso::max_days_allowed.
/// Negative seconds are allowed (so that a user can perform basic operations
/// like e.g. addition), but some functions expect only positive seconds
/// (seconds::remove_days, seconds::to_days).
/// seconds is a class which represents a second subdivision (as is
/// dso::milliseconds, dso::microseconds, etc); quite a few methods should be
/// common to all of these classes, all of which have a member variable
/// seconds::is_of_sec_type which is set to true.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// seconds (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
///
/// @warning The maximum number of days that can be expressed in seconds without
///          fear of overflow is given by the template function
///          dso::max_days_allowed
///
/// @see dso::milliseconds
/// @see dso::microseconds
class seconds {
public:
  /// Seconds are represented as long ints.
  typedef int_fast64_t underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /// Seconds is a subdivision of seconds.
  static constexpr bool is_of_sec_type{true};

  /// Seconds in day.
  static constexpr underlying_type max_in_day{86400};

  /// The scale factor to transform from seconds to seconds.
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1);
  }

  /// Constructor; default seconds is 0.
  explicit constexpr seconds(underlying_type i = 0) noexcept : m_sec(i){};

  /// Constructor from hours, minutes, seconds.
  explicit constexpr seconds(hours h, minutes m, seconds c) noexcept
      : m_sec{c.as_underlying_type() + m.as_underlying_type() * 60L +
              h.as_underlying_type() * 3600L} {};

  /// Constructor from hours, minutes, fractional seconds.
  ///
  /// @note Only the integral part of the input seconds (fs) is considered;
  ///       the fractional part is ommited.
  explicit constexpr seconds(hours h, minutes m, double fs) noexcept
      : m_sec{static_cast<underlying_type>(fs) + m.as_underlying_type() * 60L +
              h.as_underlying_type() * 3600L} {}

/// assignment operator from any integral type
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

  /// Overload - operator (subtraction).
  constexpr seconds operator-(const seconds &n) const noexcept {
    return seconds{m_sec - n.m_sec};
  }

  /// Overload + operator (addition).
  constexpr seconds operator+(const seconds &sec) const noexcept {
    return seconds{m_sec + sec.m_sec};
  }

  /// Do the secods sum up to more than one day?
  constexpr bool more_than_day() const noexcept { return m_sec > max_in_day; }

  /// Get the seconds as seconds::underlying_type .
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

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
    return days;
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
  ///
  /// Interpret (cast) the seconds as fractional days; returns a double.
  constexpr double fractional_days() const noexcept {
    return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
  }

  /// Cast to double (i.e. fractional seconds).
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec);
  }

  /// Translate to hours, minutes, seconds and fractional seconds
  ///
  /// Translate to hours, minutes, seconds and fractional seconds. Fractional
  /// seconds, in this case, are obviously always 0 (aka the last element of
  /// the returned tuple, holds 0L).
  /// @todo don;t like this; make better
  constexpr std::tuple<hours, minutes, seconds, long> to_hmsf() const noexcept {
    return std::make_tuple(hours{static_cast<int>(m_sec / 3600L)},
                           minutes{static_cast<int>((m_sec % 3600L) / 60L)},
                           seconds{(m_sec % 3600L) % 60L}, 0L);
  }

private:
/// Cast to any arithmetic type.
#if __cplusplus >= 202002L
  template <typename T>
  requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept { return static_cast<T>(m_sec); }

  /// The seconds as underlying type.
  underlying_type m_sec;

}; // seconds

/// @brief A wrapper class for milliseconds (i.e. 10**-3 sec).
///
/// milliseconds is just a wrapper class around long integer numbers, i.e. a
/// millisecond is just a long int and can be either positive or negative.
/// Users are however restricted by integer overflow. The maximum number of days
/// that can be expressed in milliseconds without fear of overflow is given by
/// the template function dso::max_days_allowed.
/// Negative milliseconds are allowed (so that a user can perform basic
/// operations like e.g. addition), but some functions expect only positive
/// milliseconds (milliseconds::remove_days, milliseconds::to_days).
/// milliseconds is a class which represents a second subdivision (as is
/// dso::seconds, dso::microseconds, etc); quite a few methods should be
/// common to all of these classes, all of which have a member variable
/// milliseconds::is_of_sec_type which is set to true.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// milliseconds (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
///
/// @note milliseconds can be cast to dso::seconds (via a static_cast or
/// a C-type cast) but the opposite is not true; i.e. dso::seconds cannot be
/// cast to milliseconds. This is still an open question!
///
/// @see dso::seconds
/// @see dso::microseconds
///
class milliseconds {
public:
  /// MilliSeconds are represented as long ints.
  typedef int_fast64_t underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /// MilliSeconds are a subdivision of seconds.
  static constexpr bool is_of_sec_type{true};

  /// Max milliseconds in one day.
  static constexpr underlying_type max_in_day{86400L * 1000L};

  /// The scale factor to transform from seconds to milliseconds.
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1000);
  }

  /// Constructor; default milliseconds is 0.
  explicit constexpr milliseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /// Constructor from hours, minutes, milliseconds.
  explicit constexpr milliseconds(hours h, minutes m, milliseconds c) noexcept
      : m_sec{c.as_underlying_type() +
              m.as_underlying_type() * 60L * sec_factor<underlying_type>() +
              h.as_underlying_type() * 3600L *
                  sec_factor<underlying_type>()} {};

  /// @brief Constructor from hours, minutes, fractional seconds.
  ///
  /// Note that the fractional seconds are only considered with millisecond
  /// precision; i.e. anything smaller than 10e-3 will not be considered (for
  /// the fs input argument).
  explicit constexpr milliseconds(hours h, minutes m, double fs) noexcept
      : m_sec{static_cast<underlying_type>(fs * sec_factor<double>()) +
              (m.as_underlying_type() * 60L + h.as_underlying_type() * 3600L) *
                  sec_factor<underlying_type>()} {};

/// assignment operator from any integral type
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

  /// @brief Cast to dso::seconds.
  /// Milliseconds can be cast to seconds (with a loss of precission).
  constexpr explicit operator seconds() const noexcept {
    return seconds{m_sec / sec_factor<underlying_type>()};
  }

  /// Addition operator.
  constexpr milliseconds operator+(const milliseconds &sec) const noexcept {
    return milliseconds{m_sec + sec.m_sec};
  }

  /// Subtraction operator.
  constexpr milliseconds operator-(const milliseconds &n) const noexcept {
    return milliseconds{m_sec - n.m_sec};
  }

  /// Do the milliseconds sum up to more than one day ?
  constexpr bool more_than_day() const noexcept { return m_sec > max_in_day; }

  /// Get the milliseconds as milliseconds::underlying_type.
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /// @brief Normalize milliseconds and return the integeral days.
  ///
  /// If the milliseconds sum up to more (or equal to) one day, remove the
  /// integer days and return them as integer; reset the milliseconds to
  /// milliseconds of the new day.
  ///
  /// @return The integer number of days (if the milliseconds are more than a
  /// day).
  /// @throw  Does not throw.
  constexpr int remove_days() noexcept {
    int days = m_sec / max_in_day;
    m_sec = m_sec % max_in_day;
    return days;
  }

  /// @brief Cast to days.
  ///
  /// If the milliseconds sum up to more (or equal to) one day, return the
  /// (integral) number of days.
  ///
  /// @return The integer number of days (if the milliseconds are more than a
  /// day).
  /// @throw  Does not throw.
  ///
  /// @note   The number of days returned can be negative (!!), if the seconds
  ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
  ///
  constexpr int to_days() const noexcept {
    return int{static_cast<int>(m_sec / max_in_day)};
  }

  /// Cast to fractional days.
  constexpr double fractional_days() const noexcept {
    return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
  }

  /// Cast to fractional dso::seconds
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) * 1.0e-3;
  }

  /// @brief Resolve to (integer) seconds and fractional seconds.
  ///
  /// Split the milliseconds to dso::seconds and fractional seconds.
  ///
  /// @param[out] fraction Fractional seconds
  /// @return              Integral seconds
  /// @throw               Does not throw
  ///
  /// @note Only works for positive milliseconds.
  constexpr seconds resolve_sec(double &fraction) const noexcept {
    seconds sec{m_sec / sec_factor<underlying_type>()};
    fraction =
        static_cast<double>(m_sec % sec_factor<underlying_type>()) * 1e-3;
    return sec;
  }

  /// Translate to hours, minutes, seconds and milliseconds
  ///
  /// The last element in the returned tuple, is the fractional part of the
  /// seconds, expressed as milliseconds. So, e.g. to transform this instance
  /// to hours, minutes and fracional seconds (of day), one can do:
  /// \code{.cpp}
  ///   std::tuple<hours, minutes, seconds, long> hmsf;
  ///   hmsf = some_millisecond_instance.to_hmsf();
  ///   auto secnds = std::get<3>(hmsf);
  ///   double fractional_seconds = (double)secnds /
  ///   milliseconds::sec_factor<double>(); fractional_seconds +=
  ///   (double)std::get<2>(hmsf).as_underlying_type();
  /// \endcode
  /// @bug need more documentation
  constexpr std::tuple<hours, minutes, seconds, long> to_hmsf() const noexcept {
    long hr{m_sec / 3600000L};                             // hours
    long mn{(m_sec % 3600000L) / 60000L};                  // minutes
    long sc{((m_sec % 3600000L) % 60000L) / 1000L};        // seconds
    long ms{m_sec - ((hr * 60L + mn) * 60L + sc) * 1000L}; // milliseconds.

    return std::make_tuple(hours{static_cast<hours::underlying_type>(hr)},
                           minutes{static_cast<minutes::underlying_type>(mn)},
                           seconds{sc}, ms);
  }

private:
/// Cast to any arithmetic type.
#if __cplusplus >= 202002L
  template <typename T>
  requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept { return static_cast<T>(m_sec); }

  /// Milliseconds as underlying type.
  underlying_type m_sec;

}; /// class milliseconds

/// @brief A wrapper class for microseconds (i.e 10**-6 sec.).
///
/// microseconds is just a wrapper class around long integer numbers, i.e. a
/// microsecond is just a long int and can be either positive or negative.
/// Users are however restricted by integer overflow. The maximum number of days
/// that can be expressed in microseconds without fear of overflow is given by
/// the template function dso::max_days_allowed.
/// Negative microseconds are allowed (so that a user can perform basic
/// operations like e.g. addition), but some functions expect only positive
/// microiseconds (microseconds::remove_days, microseconds::to_days).
/// microseconds is a class which represents a second subdivision (as is
/// dso::seconds, dso::milliseconds, etc); quite a few methods should be
/// common to all of these classes, all of which have a member variable
/// microseconds::is_of_sec_type which is set to true.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// microseconds (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
///
/// @note microseconds can be cast to dso::seconds and dso::milliseconds (via
/// a static_cast or a C-type cast) but the opposite is not true; i.e.
/// dso::seconds cannot be cast to microseconds.This is still an open question!
///
/// @see dso::seconds
/// @see dso::milliseconds
///
class microseconds {
public:
  /// Microseconds are represented as long integers.
  typedef int_fast64_t underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /// Microseconds is a subdivision of seconds.
  static constexpr bool is_of_sec_type{true};

  /// Max microseconds in day.
  static constexpr underlying_type max_in_day{86400L * 1'000'000L};

  /// The scale factor to transform from seconds to microseconds.
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000);
  }

  /// Constructor; default microseconds is 0.
  explicit constexpr microseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /// Constructor from hours, minutes, microseconds.
  explicit constexpr microseconds(hours h, minutes m, microseconds c) noexcept
      : m_sec{c.as_underlying_type() +
              (m.as_underlying_type() * 60L + h.as_underlying_type() * 3600L) *
                  sec_factor<underlying_type>()} {};

  /// Constructor from hours, minutes, fractional seconds.
  /// @note Fractional seconds with precission larger than microseconds are
  ///       not taken into account; i.e. we only consider the fractiona part
  ///       up to 10e-6.
  explicit constexpr microseconds(hours h, minutes m, double fs) noexcept
      : m_sec{static_cast<underlying_type>(fs * sec_factor<double>()) +
              (m.as_underlying_type() * 60L + h.as_underlying_type() * 3600L) *
                  sec_factor<underlying_type>()} {};

  /// assignment operator from any integral type
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

  /// Microseconds can be cast to milliseconds will a loss of accuracy.
  constexpr explicit operator milliseconds() const {
    return milliseconds(m_sec / 1000L);
  }

  /// Microseconds can be cast to seconds will a loss of accuracy.
  constexpr explicit operator seconds() const {
    return seconds(m_sec / sec_factor<underlying_type>());
  }

  /// Addition between microseconds.
  constexpr microseconds operator+(const microseconds &sec) const noexcept {
    return microseconds{m_sec + sec.m_sec};
  }

  /// Subtraction between microseconds.
  constexpr microseconds operator-(const microseconds &n) const noexcept {
    return microseconds{m_sec - n.m_sec};
  }

  /// Do the microseconds sum up to more than one day?
  constexpr bool more_than_day() const noexcept { return m_sec > max_in_day; }

  /// Cast to microseconds::underlying_type.
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /// @brief Normalize microseconds and return the integeral days.
  ///
  /// If the microseconds sum up to more (or equal to) one day, remove the
  /// integer days and return them as integer; reset the microseconds to
  /// microseconds of the new day.
  ///
  /// @return The integer number of days (if the microseconds are more than a
  /// day).
  /// @throw  Does not throw.
  ///
  /// @note   The number of days returned can be negative (!!), if the
  /// microseconds
  ///         are negative (i.e. if m_sec = -86400, -1 will be returned and
  ///         m_sec will be set to 0).
  ///
  constexpr int remove_days() noexcept {
    int days = m_sec / max_in_day;
    m_sec = m_sec % max_in_day;
    return days;
  }

  /// @brief Cast to days.
  ///
  /// If the microseconds sum up to more (or equal to) one day, return the
  /// (integral) number of days.
  ///
  /// @return The integer number of days (if the microseconds are more than a
  /// day).
  /// @throw  Does not throw.
  ///
  /// @note   The number of days returned can be negative (!!), if the seconds
  ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
  ///
  constexpr int to_days() const noexcept {
    return static_cast<int>(m_sec / max_in_day);
  }

  /// Cast to fractional days.
  constexpr double fractional_days() const noexcept {
    return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
  }

  /// Cast to fractional seconds
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) * 1.0e-6;
  }

  /// Translate to hours, minutes, seconds and microseconds.
  /// @bug need more documentation
  constexpr std::tuple<hours, minutes, seconds, long> to_hmsf() const noexcept {
    long hr{m_sec / 3600000000L};                             // hours
    long mn{(m_sec % 3600000000L) / 60000000L};               // minutes
    long sc{((m_sec % 3600000000L) % 60000000L) / 1000000L};  // seconds
    long ns{m_sec - ((hr * 60L + mn) * 60L + sc) * 1000000L}; // microsec.
    return std::make_tuple(hours{static_cast<hours::underlying_type>(hr)},
                           minutes{static_cast<minutes::underlying_type>(mn)},
                           seconds{sc}, ns);
  }

private:
/// Cast to any arithmetic type.
#if __cplusplus >= 202002L
  template <typename T>
  requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept { return static_cast<T>(m_sec); }

  /// Microseconds as long ints.
  underlying_type m_sec;

}; // microseconds

/// @brief A wrapper class for nanoseconds (i.e 10**-9 sec.).
///
/// nanoseconds is just a wrapper class around long integer numbers, i.e. a
/// nanosecond is just a long int and can be either positive or negative.
/// Users are however restricted by integer overflow. The maximum number of days
/// that can be expressed in nanoseconds without fear of overflow is given by
/// the template function dso::max_days_allowed.
/// Negative nanoseconds are allowed (so that a user can perform basic
/// operations like e.g. addition), but some functions expect only positive
/// nanoseconds (nanoseconds::remove_days, nanoseconds::to_days).
/// nanoseconds is a class which represents a second subdivision (as is
/// dso::seconds, dso::milliseconds, etc); quite a few methods should be
/// common to all of these classes, all of which have a member variable
/// nanoseconds::is_of_sec_type which is set to true.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// nanoseconds (constructor) can only have zero or positive values.
///
/// This is a fundamental class, which means it only has one arithmetic member
/// variable. The classe's bollean operators (aka '==', '!=', '<', '<=', '>',
/// '>=') are going to be implemented using kinda reflection, using template
/// function overloadnig outside the class.
///
/// @note nanoseconds can be cast to dso::seconds, dso::milliseconds and
/// dso::microseconds (via a static_cast or a C-type cast) but the opposite is
/// not true; i.e. dso::seconds cannot be cast to nanoseconds.This is still
/// an open question!
///
/// @see dso::seconds
/// @see dso::milliseconds
///
class nanoseconds {
public:
  /// Nanoseconds are represented as long integers.
  typedef int_fast64_t underlying_type;

  /// Is fundamental datetime type
  static constexpr bool is_dt_fundamental_type{true};

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type __member_const_ref__() const noexcept {
    return m_sec;
  }

  /// If fundamental type, the class should have an "expose the only member var"
  /// function
  constexpr underlying_type &__member_ref__() noexcept { return m_sec; }

  /// Microseconds is a subdivision of seconds.
  static constexpr bool is_of_sec_type{true};

  /// Max nanoseconds in day.
  static constexpr underlying_type max_in_day{86400L * 1000000000L};

  /// The scale factor to transform from seconds to nanoseconds.
  template <typename T> static constexpr T sec_factor() noexcept {
    return static_cast<T>(1'000'000'000);
  }

  /// Constructor; default nanoseconds is 0.
  explicit constexpr nanoseconds(underlying_type i = 0L) noexcept : m_sec(i){};

  /// Constructor from hours, minutes, nanoseconds.
  explicit constexpr nanoseconds(hours h, minutes m, nanoseconds c) noexcept
      : m_sec{c.as_underlying_type() +
              (m.as_underlying_type() * 60L + h.as_underlying_type() * 3600L) *
                  sec_factor<underlying_type>()} {};

  /// Constructor from hours, minutes, fractional seconds.
  /// @note Fractional seconds with precission larger than microseconds are
  ///       not taken into account; i.e. we only consider the fractiona part
  ///       up to 10e-9.
  explicit constexpr nanoseconds(hours h, minutes m, double fs) noexcept
      : m_sec{static_cast<underlying_type>(fs * sec_factor<double>()) +
              (m.as_underlying_type() * 60L + h.as_underlying_type() * 3600L) *
                  sec_factor<underlying_type>()} {};

/// assignment operator from any integral type
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

  /// Nanoseconds can be cast to microseconds will a loss of accuracy.
  constexpr explicit operator microseconds() const {
    return microseconds(m_sec / 1000L);
  }

  /// Nanoseconds can be cast to milliseconds will a loss of accuracy.
  constexpr explicit operator milliseconds() const {
    return milliseconds(m_sec / 1000000L);
  }

  /// Microseconds can be cast to seconds will a loss of accuracy.
  constexpr explicit operator seconds() const {
    return seconds(m_sec / sec_factor<underlying_type>());
  }

  /// Addition between nanoseconds.
  constexpr nanoseconds operator+(const nanoseconds &sec) const noexcept {
    return nanoseconds{m_sec + sec.m_sec};
  }

  /// Subtraction between nanoseconds.
  constexpr nanoseconds operator-(const nanoseconds &n) const noexcept {
    return nanoseconds{m_sec - n.m_sec};
  }

  /// Do the nanoseconds sum up to more than one day?
  constexpr bool more_than_day() const noexcept { return m_sec > max_in_day; }

  /// Cast to nanoseconds::underlying_type.
  constexpr underlying_type as_underlying_type() const noexcept {
    return m_sec;
  }

  /// @brief Normalize nanoseconds and return the integeral days.
  ///
  /// If the nanoseconds sum up to more (or equal to) one day, remove the
  /// integer days and return them as integer; reset the nanoseconds to
  /// nanoseconds of the new day.
  ///
  /// @return The integer number of days (if the nanoseconds are more than a
  /// day).
  /// @throw  Does not throw.
  ///
  /// @note   The number of days returned can be negative (!!), if the
  /// nanoseconds
  ///         are negative (i.e. if m_sec = -86400, -1 will be returned and
  ///         m_sec will be set to 0).
  ///
  constexpr int remove_days() noexcept {
    int days = m_sec / max_in_day;
    m_sec = m_sec % max_in_day;
    return days;
  }

  /// @brief Cast to days.
  ///
  /// If the nanoseconds sum up to more (or equal to) one day, return the
  /// (integral) number of days.
  ///
  /// @return The integer number of days (if the nanoseconds are more than a
  /// day).
  /// @throw  Does not throw.
  ///
  /// @note   The number of days returned can be negative (!!), if the seconds
  ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
  ///
  constexpr int to_days() const noexcept {
    return static_cast<int>(m_sec / max_in_day);
  }

  /// Cast to fractional days.
  constexpr double fractional_days() const noexcept {
    return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
  }

  /// Cast to fractional seconds
  constexpr double to_fractional_seconds() const noexcept {
    return static_cast<double>(m_sec) * 1.0e-9;
  }

  /// Translate to hours, minutes, seconds and microseconds.
  /// @bug need more documentation
  constexpr std::tuple<hours, minutes, seconds, long> to_hmsf() const noexcept {
    long hr{m_sec / (3600L * 1000000000L)};                  // hours
    long mn{(m_sec % (3600L * 1000000000L)) / 60000000000L}; // minutes
    long sc{((m_sec % (3600L * 1000000000L)) % 60000000000L) /
            1000000000L};                                        // seconds
    long ns{m_sec - ((hr * 60L + mn) * 60L + sc) * 1000000000L}; // microsec.
    return std::make_tuple(hours{static_cast<hours::underlying_type>(hr)},
                           minutes{static_cast<minutes::underlying_type>(mn)},
                           seconds{sc}, ns);
  }

private:
/// Cast to any arithmetic type.
#if __cplusplus >= 202002L
  template <typename T>
  requires gconcepts::arithmetic<T>
#else
  template <typename T,
            typename = std::enable_if_t<std::is_arithmetic<T>::value>>
#endif
  constexpr T cast_to() const noexcept { return static_cast<T>(m_sec); }

  /// Microseconds as long ints.
  underlying_type m_sec;

}; // class nanoseconds

/// Overload operator '=' where the left-hand-side is any fundamental type and
/// the right-hand-side is any integral type. That is, i want to be able to do:
/// month m {};
/// m = -189;
/// Note that this operator (obviously) does not mean that we can construct-
/// initilize fundamental types using this operator, aka
/// month m = -189; // compilation error!
/// this is dependent on each classes' constructor.
///
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// 3. right operand is any Integral type
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// mjd = 124;
/// Now mjd's internal member, will have a value of 124.
/*
template<typename DType,
         typename I,
         typename = std::enable_if_t<DType::is_dt_fundamental_type>,
         typename = std::enable_if_t<std::is_member_function_pointer
                  <decltype(&DType::__member_ref__)>::value>,
         typename = std::enable_if_t<std::is_integral_v<I>>
         >
  inline DType&
  operator=(DType& _a, I _intv) noexcept
{
  _a.__member_ref__() = _intv;
  return _a;
}
*/

#if __cplusplus >= 202002L
namespace gconcepts {
/// @brief concept has_const_ref_dt -> Make sure that the class has a
/// member function __member_const_ref__()
template <typename DType>
concept has_const_ref_dt = requires(DType t) {
  t.__member_const_ref__();
};

/// @brief concept has_const_ref_dt -> Make sure that the class has a
/// member function __member_ref__()
template <typename DType>
concept has_ref_dt = requires(DType t) {
  t.__member_ref__();
};

/// @brief concept is_fundamental_dt -> Make sure that the class has a
/// static member variable called DType::is_dt_fundamental_type and it is
/// set to true
template <typename DType>
concept is_fundamental_dt = requires(DType) {
  DType::is_dt_fundamental_type == true;
};

/// @brief concept is_sec_dt -> Make sure that the class has a
/// static member variable called DType::is_of_sec_type and it is
/// set to true
template <typename DType>
concept is_sec_dt = requires(DType) {
  DType::is_of_sec_type == true;
};

template <typename DType>
concept is_fundamental_and_has_const_ref =
    is_fundamental_dt<DType> && has_const_ref_dt<DType>;

template <typename DType>
concept is_fundamental_and_has_ref =
    is_fundamental_dt<DType> && has_ref_dt<DType>;
} // namespace gconcepts
#endif

/// Overload bool operator '==' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
/// @note that this function only allows comparisson when both right/left sides
///       of the expression satisfly (1) and (2)
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

/// @todo do we want to allow this?
///
/// Overload bool operator '==' for comparing datetime fundamental types with
/// integers, aka
/// month m(3);
/// assert(m == 3)
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '!=' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '>' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '>=' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '<' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '<=' for datetime fundamental types.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_const_ref__()
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

/// Overload bool operator '+=' for datetime fundamental types when the right
/// operand is any integral type.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// 3. right operand is any Integral type
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// mjd += 1;
/// Now mjd's internal member, will have a value of 124.
template <typename DType, typename I,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>,
          typename = std::enable_if_t<std::is_integral_v<I>>>
constexpr DType &operator+=(DType &_a, I _intv) noexcept {
  _a.__member_ref__() += _intv;
  return _a;
}

/// Overload bool operator '+=' for datetime fundamental types when the right
/// operand is the same type as the calling instance.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// 3. right and left operands are of the same type
/// This function will allow e.g.
/// modified_julian_day mjd1 (123), mjd2 (132);
/// mjd1 += mjd2;
/// Now mjd's internal member, will have a value of 155.
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

/// Overload bool operator '-=' for datetime fundamental types when the right
/// operand is any integral type.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// 3. right operand is any Integral type
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// mjd -= 1;
/// Now mjd's internal member, will have a value of 122.
template <typename DType, typename I,
          typename = std::enable_if_t<DType::is_dt_fundamental_type>,
          typename = std::enable_if_t<std::is_member_function_pointer<
              decltype(&DType::__member_ref__)>::value>,
          typename = std::enable_if_t<std::is_integral_v<I>>>
constexpr DType &operator-=(DType &_a, I _intv) noexcept {
  _a.__member_ref__() -= _intv;
  return _a;
}

/// Overload bool operator '-=' for datetime fundamental types when the right
/// operand is of the same type as the calling instance.
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// 3. right and left hand sides are of the same type
/// This function will allow e.g.
/// modified_julian_day mjd1 (123), mjd2 (100);
/// mjd1 -= mjd2;
/// Now mjd's internal member, will have a value of 23.
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

/// Overload prefix increment operator '++' for datetime fundamental types
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// ++mjd;
/// Now mjd's internal member, will have a value of 124.
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

/// Overload postfix increment operator '++' for datetime fundamental types
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// mjd++;
/// Now mjd's internal member, will have a value of 124.
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

/// Overload prefix decrement operator '--' for datetime fundamental types
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// --mjd;
/// Now mjd's internal member, will have a value of 122.
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

/// Overload postfix decrement operator '--' for datetime fundamental types
/// This function will be resolved for any type DType, which
/// 1. has a member (variable) DType::is_dt_fundamental_type set to true, and
/// 2. has a member function named DType::__member_ref__()
/// This function will allow e.g.
/// modified_julian_day mjd (123);
/// mjd--;
/// Now mjd's internal member, will have a value of 122.
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
constexpr DType operator+(DType _a, DType _b) noexcept {
  return DType(_a.__member_const_ref__() + _b.__member_const_ref__());
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

/// @brief Number of expressible days for any second type.
///
/// This (template) function will return the number of whole days that can be
/// expressed using any instance of a second type (i.e. dso::seconds,
/// dso::milliseconds, etc). For any of these types, trying to hold more days
/// than this limit may result in overflow.
///
/// @tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
constexpr typename S::underlying_type max_days_allowed() {
  return std::numeric_limits<typename S::underlying_type>::max() /
         S::max_in_day;
}

/// @brief Express the difference between two Modified Julian Days as any second
///        type.
///
/// @tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
/// @param[in] d1 The first dso::modified_julian_day (difference is d1-d2)
/// @param[in] d2 The second dso::modified_julian_day (difference is d1-d2)
/// @return       The difference between d1 and d2 in the second type S (e.g.
///               dso::seconds, dso::milliseconds, etc)
///
/// @note
///       - The difference between two Modified Julian Days is always an
///       integral
///         number of days (no fractional part!).
///       - If the code is compiled with the switch USE_DATETIME_CHECKS, then
///       the
///         difference can only produce a positive amount; hence be sure that
///         d1>=d2
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
constexpr S mjd_sec_diff(modified_julian_day d1,
                         modified_julian_day d2) noexcept {
  modified_julian_day d{d1 - d2};
  return S{d.as_underlying_type() * S::max_in_day};
}

/// Cast any second type to another second type.
///
/// Cast an instance of any second type (aka any instance for which
/// is_of_sec_type is defined and is true) to any other second type. E.g.,
/// cast seconds to milliseconds or cast microseconds to seconds. Be warned,
/// that casting to less precission (e.g. microseconds to seconds) will cause
/// loss of precission (1 microsecond is not 1e-6 seconds, it is just 0
/// seconds, remember?).
///
/// @tparam Ssrc Any class of second type, i.e. any class S that has a (static)
///         member variable S::is_of_sec_type set to true.
/// @tparam Strg Any class of second type, i.e. any class S that has a (static)
///         member variable S::is_of_sec_type set to true.
/// @param[in] s An instance of type Ssrc to be cast to an instance of Strg
/// @return      The input s instance, as an instance of type Strg
///
/// @warning this may cause loss of precission when e.g. casting milliseconds to
///          seconds. E.g.
///          cast_to<seconds, milliseconds>(seconds {1}) // result is 1000
///          cast_to<milliseconds, seconds>(milliseconds {1}) // result is 0
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
  auto numerator{s.__member_ref__() * Strg::template sec_factor<long>()};
  return Strg{numerator / Ssrc::template sec_factor<long>()};
}

/// @class t_hmsf A simple wrapper class to hold time as hours, minutes, seconds
///        and fractional seconds.
struct t_hmsf {
  hours _hours;
  minutes _minutes;
  seconds _seconds;
  double _fraction;

/// @brief Constructor from any type that has S::is_of_sec_type member, aka
///        any sec-type
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt S>
#else
  template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
  constexpr t_hmsf(S sec) noexcept {
    long fac = S::template sec_factor<long>();
    typename S::underlying_type fac_36L = fac * 3600L;
    typename S::underlying_type _sec = sec.as_underlying_type();

    long hr{_sec / fac_36L};               // hours
    long mn{(_sec % fac_36L) / 60L / fac}; // minutes
    typename S::underlying_type sc{(_sec - (hr * 3600L + mn * 60L) * fac) /
                                   fac};
#ifdef DEBUG
    // this way is a tiny bit slower
    typename S::underlying_type sc1{(_sec % (60L * fac)) / fac}; // seconds
    assert(sc1 != sc);
#endif
    typename S::underlying_type fs{_sec - ((hr * 60L + mn) * 60L + sc) *
                                              fac}; // remaining S's

    /*
    printf("-->debug on hmsf constructor: got seconds: %ld\n", _sec);
    printf("\thours    : %d\n", (int)hr);
    printf("\tminutes  : %d\n", (int)mn);
    printf("\tseconds  : %ld\n", sc);
    printf("\tfraction : %ld\n", fs);
    */

    _hours = hours{hr};
    _minutes = minutes{mn};
    _seconds = seconds(sc);
    _fraction = static_cast<double>(fs) / S::template sec_factor<double>();
    // printf("\tfseconds : %.9f\n", _fraction);
  }

  constexpr bool operator==(const t_hmsf &other) const noexcept {
    return (_hours == other._hours && _minutes == other._minutes &&
            _seconds == other._seconds && _fraction == other._fraction);
  }

  constexpr bool operator!=(const t_hmsf &other) const noexcept {
    return !(*this == other);
  }
}; // hmsf

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

} // namespace dso

#endif // define DATETIME
