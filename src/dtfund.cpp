///
/// @file  dtfund.cpp
///
/// @brief Implementation file for header dtfund.hpp.
///
/// This file contains fundamental constants and algorithms for manipulating
/// date and time, targeting GNSS applications.
///
/// @author xanthos
///
/// @bug No known bugs.
///

#include "dtfund.hpp"
#include <cassert>

/// Definition for static month array (short names).
/// @see ngpt::month
constexpr const char *ngpt::month::short_names[];

/// Definition for static month array (long names).
/// @see ngpt::month
constexpr const char *ngpt::month::long_names[];

/// Number of days past at the end of non-leap and leap years.
constexpr static long month_day[2][13] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

int __lower_strncmp__(const char *str1, const char *str2, std::size_t n = 0) {
  if (!n)
    n = std::max(std::strlen(str1), std::strlen(str2));
  for (std::size_t i = 0; i < n; i++)
    if (!(std::tolower(str1[i]) == std::tolower(str2[i])))
      return 1;
  return 0;
}

///
/// Given a c-string (i.e. null-terminating char array), resolve the month.
/// The c-string can be either a short name (i.e. a 3-character name), e.g.
/// "Jan", or the whole, normal month name e.g. "January".
/// If a 3-char length string is passed in, we are going to compare using the
/// month::short_names array; else if the length of the input string is more
/// than 3-chars, the month::long_names array is used.
/// The comparisson is performed with both strings (aka the input string and
/// each of the long/short_names arrays) beeing already transformed to lowercase
/// That means that this function is actually case-insensitive!
/// If the input string cannot be matced to any of the strings in short_names
/// and long_names, then an exception is thrown of type: std::invalid_argument
/// Note that the month will be returned in the "normal" range [1,12],
/// **not** [0-11].
///
ngpt::month::month(const char *str) {
  m_month = 0;

  if (std::strlen(str) == 3) {
    for (int i = 0; i < 12; ++i) {
      if (!__lower_strncmp__(short_names[i], str, 3)) {
        m_month = i + 1;
        break;
      }
    }
  } else if (std::strlen(str) > 3) {
    for (int i = 0; i < 12; ++i) {
      if (!__lower_strncmp__(long_names[i], str)) {
        m_month = i + 1;
        break;
      }
    }
  }

  if (!m_month || std::strlen(str) < 3) {
    throw std::invalid_argument("Failed to set month from string \"" +
                                std::string(str) + "\"");
  }
}

///
/// Given a modified_julian_day convert it to a tuple (i.e. a pair) of
/// year and day_of_year.
///
constexpr ngpt::ydoy_date ngpt::modified_julian_day::to_ydoy() const noexcept {
  long days_fr_jan1_1901{m_mjd - ngpt::jan11901};
  long num_four_yrs{days_fr_jan1_1901 / 1461L};
  long years_so_far{1901L + 4 * num_four_yrs};
  long days_left{days_fr_jan1_1901 - 1461 * num_four_yrs};
  long delta_yrs{days_left / 365 - days_left / 1460};

  ydoy_date ydoy;
  ydoy.__doy = day_of_year{static_cast<day_of_year::underlying_type>(
      days_left - 365 * delta_yrs + 1)};
  ydoy.__year =
      year{static_cast<year::underlying_type>(years_so_far + delta_yrs)};

  return ydoy;
}

///
constexpr ngpt::ydoy_date ngpt::ymd_date::to_ydoy() const noexcept {
  ydoy_date yd;
  yd.__year = __year;
  int leap = __year.is_leap();
  int md = __month.as_underlying_type() - 1;
#ifdef DEBUG
  assert(md >= 0 && md < 12);
#endif
  yd.__doy = month_day[leap][md] + __dom.as_underlying_type();
  return yd;
}

constexpr ngpt::ymd_date ngpt::ydoy_date::to_ymd() const noexcept {
  ymd_date yd;
  yd.__year = __year;
  int guess = __doy.as_underlying_type() * 0.032;
  int leap = __year.is_leap();
#ifdef DEBUG
  assert(guess >= 0 && guess < 11);
#endif
  int more = ((__doy.as_underlying_type() - month_day[leap][guess + 1]) > 0);
  yd.__month = month{guess + more + 1};
#ifdef DEBUG
  assert(guess + more >= 0 && guess + more < 12);
#endif
  yd.__dom =
      day_of_month(__doy.as_underlying_type() - month_day[leap][guess + more]);
  return yd;
}
