#include "datetime_io_core.hpp"
#include <cctype>
#include <charconv>
#include <cstdio>

namespace {
  /** just so it is not a magic number */
  constexpr const int MONTHS_IN_YEAR = 12;
}/* unnamed namespace */

inline const char *skipws(const char *line) noexcept {
  const char *c = line;
  while (*c && (*c == ' ' || *c == '/' || *c == '-' || *c == 'T' || *c == ':' ||
                *c == '_'))
    ++c;
  return c;
}

/** @brief Count decimal digits in a float
 *
 * Examples:
 * assert(count_decimal_digits(".123456789") == 9);
 * assert(count_decimal_digits("  .123456789") == 9);
 * assert(count_decimal_digits("12.123456789") == 9);
 * assert(count_decimal_digits("12.123456789.123") == 9);
 * assert(count_decimal_digits("12.123456789.A23") == 9);
 * assert(count_decimal_digits("12.123456789EA23") == 9);
 */
inline int count_decimal_digits(const char *fltstr) noexcept {
  /* go to decimal part */
  while (fltstr && *fltstr != '.')
    ++fltstr;
  if (fltstr && *fltstr == '.') {
    /* count digits */
    ++fltstr;
    const char *dgtc = fltstr;
    while (std::isdigit(*dgtc))
      ++dgtc;
    return (int)(dgtc - fltstr);
  }
  /* no decimal digits */
  return 0;
}

int dso::datetime_io_core::get_one_int(const char *str, int *ints,
                                       int max_chars,
                                       const char **end) noexcept {
  if (end)
    *end = str;
  const char *start = skipws(str);
  const char *c = str;

  /* resolve one int */
  auto tres = std::from_chars(skipws(c), start + max_chars, *ints);
  if (tres.ec != std::errc{}) {
    return 1;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = c;

  return 0;
}

int dso::datetime_io_core::get_two_ints(const char *str, int *ints,
                                        int max_chars,
                                        const char **end) noexcept {
  if (end)
    *end = str;
  const char *start = skipws(str);
  const char *c = str;

  /* resolve the three ints */
  for (int i = 0; i < 2; ++i) {
    auto tres = std::from_chars(skipws(c), start + max_chars, ints[i]);
    if (tres.ec != std::errc{}) {
      return i + 1;
    }
    c = tres.ptr;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = c;

  return 0;
}

int dso::datetime_io_core::get_three_ints(const char *str, int *ints,
                                          int max_chars,
                                          const char **end) noexcept {
  if (end)
    *end = str;
  const char *c = str;
  const char *start = skipws(str);

  /* resolve the three ints */
  for (int i = 0; i < 3; ++i) {
    auto tres = std::from_chars(skipws(c), start + max_chars, ints[i]);
    if (tres.ec != std::errc{}) {
      return i + 1;
    }
    c = tres.ptr;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = c;

  return 0;
}

int dso::datetime_io_core::get_three_ints(const char *str, long *ints,
                                          int max_chars,
                                          const char **end) noexcept {
  if (end)
    *end = str;
  const char *c = str;
  const char *start = skipws(str);

  /* resolve the three long ints */
  for (int i = 0; i < 3; ++i) {
    auto tres = std::from_chars(skipws(c), start + max_chars, ints[i]);
    if (tres.ec != std::errc{}) {
      return i + 1;
    }
    c = tres.ptr;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = c;

  return 0;
}

int dso::datetime_io_core::get_two_ints_double(const char *str, int *ints,
                                               double &flt, int max_chars,
                                               const char **end) noexcept {
  if (end)
    *end = str;
  const char *c = str;
  const char *start = skipws(str);

  /* resolve the two ints */
  for (int i = 0; i < 2; ++i) {
    auto tres = std::from_chars(skipws(c), start + max_chars, ints[i]);
    if (tres.ec != std::errc{}) {
      return i + 1;
    }
    c = tres.ptr;
  }

  /* before parsing the next floating point number, count its decimal digits
   * If more than nanoseconds, issue a warning
   */
  if (count_decimal_digits(skipws(c)) > MONTHS_IN_YEAR) {
    fprintf(stderr, "[WARNING] Reading in date with resolution larger than "
                    "nanoseconds will lead to loss of precision!\n");
    fprintf(stderr,
            "[WARNING] Date/Time resolved from string \'%s\' (traceback: %s)\n",
            str, __func__);
  }

  /* resolve one double */
  flt = 0e0;
  auto tres = std::from_chars(skipws(c), start + max_chars, flt);
  if (tres.ec != std::errc{}) {
    return 3;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = tres.ptr;

  return 0;
}
