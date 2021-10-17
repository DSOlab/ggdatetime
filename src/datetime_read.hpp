///
/// @file  datetime_read.hpp
///
/// @brief Functions to read in dso::datetime objects from various input
///        formats.
///
/// @see dso::datetime
///
/// @author xanthos
///
/// @bug No known bugs.
///

#ifndef __NGPT_DT_READERS__
#define __NGPT_DT_READERS__

#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include <cctype> // for std::isalpha
#include <cstdlib>
#include <stdexcept>

#ifdef DEBUG
#include <iostream>
#endif

namespace dso {

/// @brief Read from YYYY-MM-DD
///
/// Read and return a date from a c-string of type: YYYY-MM-DD, where the
/// delimeters can be whatever (but something, i.e. two numbers must be
/// seperated by some char -- 20150930 is wrong --).
/// Hours, minutes and seconds are set to 0.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T>
datetime<T> strptime_ymd(const char *str, char **stop = nullptr) {
  char *end;
  const char *start = str;
  int ints[3];

  for (int i = 0; i < 3; ++i) {
    ints[i] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
    if (errno == ERANGE || start == end) {
      errno = 0;
      throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                  "\" (argument #" + std::to_string(i + 1) +
                                  ").");
    }
    start = end + 1;
  }
  if (stop)
    *stop = end - 1;
  return datetime<T>{year{ints[0]}, month{ints[1]}, day_of_month{ints[2]},
                     hours{0},      minutes{0},     T{0}};
}

/// @brief Read from YYYY-DDD
///
/// Read and return a date from a c-string of type: YYYY-DDD, where the
/// delimeters can be whatever (but something, i.e. two numbers must be
/// seperated by some char -- 2015001 is wrong --).
/// Hours, minutes and seconds are set to 0.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T>
datetime<T> strptime_ydoy(const char *str, char **stop = nullptr) {
  char *end;
  const char *start = str;
  int ints[2];

  for (int i = 0; i < 2; ++i) {
    ints[i] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
    if (errno == ERANGE || start == end) {
      errno = 0;
      throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                  "\" (argument #" + std::to_string(i + 1) +
                                  ").");
    }
    start = end + 1;
  }
  if (stop)
    *stop = end - 1;
  return datetime<T>{year{ints[0]}, day_of_year{ints[1]}, hours{0}, minutes{0},
                     T{0}};
}

/// @brief Read from YYYY-MM-DD HH:MM:SS.SSSS
///
/// Read and return a date from a c-string of type: YYYY-MM-DD HH:MM:SS.SSSS,
/// where the delimeters can be whatever (but something, i.e. two numbers must
/// be seperated by some char -- 20150930 is wrong --).
/// Seconds can be fractional or integer.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T>
datetime<T> strptime_ymd_hms(const char *str, char **stop = nullptr) {
  char *end;
  const char *start = str;
  int ints[5];
  double secs;

  for (int i = 0; i < 5; ++i) {
    ints[i] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
    if (errno == ERANGE || start == end) {
      errno = 0;
      throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                  "\" (argument #" + std::to_string(i + 1) +
                                  ").");
    }
    start = end + 1;
  }
  secs = std::strtod(start, &end);
  if (errno == ERANGE) {
    errno = 0;
    throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                "\" (argument #6)");
  }
  if (stop)
    *stop = end;
  return datetime<T>{year{ints[0]},  month{ints[1]},   day_of_month{ints[2]},
                     hours{ints[3]}, minutes{ints[4]}, secs};
}

/// @brief Read from YYYY-OOO-DD HH:MM:SS.SSSS
///
/// Read and return a date from a c-string of type: YYYY-OOO-DD HH:MM:SS.SSSS,
/// where the delimeters can be whatever (i.e. the month is a three letter
/// identifier, in whatever case).
/// Seconds can be fractional or integer.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
/// The month string (aka 'OOO') can be in whatever case, i.e. 'Jan' or 'JAN'
/// or 'jan' or anything else.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T>
datetime<T> strptime_yod_hms(const char *str, char **stop = nullptr) {
  char *end;
  const char *start = str;
  int ints[5];
  char month_str[4];
  month_str[3] = '\0';
  double secs;

  ints[0] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
  if (errno == ERANGE || start == end) {
    errno = 0;
    throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                "\" (argument #1).");
  }
  start = end + 1;

  if (!std::isalpha(*start))
    ++start;
  std::memcpy(month_str, start, 3);
  month_str[0] = std::toupper(month_str[0]);
  month_str[1] = std::tolower(month_str[1]);
  month_str[2] = std::tolower(month_str[2]);
  month mnt{month_str};
  start += 4;

  for (int i = 2; i < 5; ++i) {
    ints[i] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
    if (errno == ERANGE || start == end) {
      errno = 0;
      throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                  "\" (argument #" + std::to_string(i + 1) +
                                  ").");
    }
    start = end + 1;
  }

  secs = std::strtod(start, &end);
  if (errno == ERANGE) {
    errno = 0;
    throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                "\" (argument #6)");
  }

  if (stop)
    *stop = end;

  return datetime<T>{year{ints[0]},         mnt,
                     day_of_month{ints[2]}, hours{ints[3]},
                     minutes{ints[4]},      secs};
}

/// @brief Read from YYYY-DDD HH:MM:SS.SSSS
///
/// Read and return a date from a c-string of type: YYYY-DDD HH:MM:SS.SSSS,
/// where the delimeters can be whatever (but something, i.e. two numbers must
/// be seperated by some char -- 2015001 is wrong --).
/// Seconds can be fractional or integer.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T>
datetime<T> strptime_ydoy_hms(const char *str, char **stop = nullptr) {
  char *end;
  const char *start = str;
  int ints[4];
  double secs;

  for (int i = 0; i < 4; ++i) {
    ints[i] = static_cast<int>(std::abs(std::strtol(start, &end, 10)));
    if (errno == ERANGE || start == end) {
      errno = 0;
      throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                  "\" (argument #" + std::to_string(i + 1) +
                                  ").");
    }
    start = end + 1;
  }
  secs = std::strtod(start, &end);
  if (errno == ERANGE) {
    errno = 0;
    throw std::invalid_argument("Invalid date format: \"" + std::string(str) +
                                "\" (argument #5)");
  }
  if (stop)
    *stop = end;
  return datetime<T>{year{ints[0]}, day_of_year{ints[1]}, hours{ints[2]},
                     minutes{ints[3]}, secs};
}

} // namespace dso

#endif
