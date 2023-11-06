/** @file
 * Functions to read in datetime<S>, TwoPartDate and TwoPartDateUTC instances
 * from various input formats.
 */

#ifndef __NGPT_DT_READERS__
#define __NGPT_DT_READERS__

#include "datetime_io_core.hpp"
#include "dtcalendar.hpp"
#include "hms_time.hpp"
#include "tpdate.hpp"
#include <charconv>
#include <stdexcept>
#include <cctype>

namespace dso {

namespace {
inline const char *skipws(const char *line) noexcept {
  const char *c = line;
  while (*c && (*c == ' ' || *c == '/' || *c == '-' || *c == 'T' || *c == ':'))
    ++c;
  return c;
}

inline int count_decimal_digits(const char *fltstr) noexcept {
  /* go to decimal part */
  while (fltstr && *fltstr != '.') ++fltstr;
  if (fltstr && *fltstr == '.') {
    /* count digits */
    ++fltstr;
    const char *dgtc = fltstr;
    while (std::isdigit(*dgtc)) ++dgtc;
    return (dgtc - fltstr);
  } else {
    /* no decimal digits */
    return 0;
  }
}

int get_two_ints(const char *str, int *ints, int max_chars,
                   const char **end) {
  if (*end)
    *end = str;
  const char *c = str;

  /* resolve the three ints */
  for (int i = 0; i < 2; ++i) {
    auto tres = std::from_chars(skipws(c), str + max_chars, ints[i]);
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

int get_three_ints(const char *str, int *ints, int max_chars,
                   const char **end) {
  if (*end)
    *end = str;
  const char *c = str;

  /* resolve the three ints */
  for (int i = 0; i < 3; ++i) {
    auto tres = std::from_chars(skipws(c), str + max_chars, ints[i]);
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

int get_three_ints(const char *str, long *ints, int max_chars,
                   const char **end) {
  if (*end)
    *end = str;
  const char *c = str;

  /* resolve the three long ints */
  for (int i = 0; i < 3; ++i) {
    auto tres = std::from_chars(skipws(c), str + max_chars, ints[i]);
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

int get_two_ints_double(const char *str, int *ints, double &flt, int max_chars,
                        const char **end) {
  if (*end)
    *end = str;
  const char *c = str;

  /* resolve the two ints */
  for (int i = 0; i < 2; ++i) {
    auto tres = std::from_chars(skipws(c), str + max_chars, ints[i]);
    if (tres.ec != std::errc{}) {
      return i + 1;
    }
    c = tres.ptr;
  }

  /* resolve one double */
  auto tres = std::from_chars(skipws(c), str + max_chars, flt);
  if (tres.ec != std::errc{}) {
    return 3;
  }

  /* assign pointer to first non-parsed character */
  if (end)
    *end = tres.ptr;

  return 0;
}
} /* anonymous namespace */

template <YMDFormat F> class ReadInDate {};
template <> class ReadInDate<YMDFormat::YYYYMMDD> {
private:
  static constexpr const int SZ = 10;

public:
  static ymd_date read(const char *str, const char **end) {
    int ints[3];
    if (get_three_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYMMDD from string %.10s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    return ymd_date(year(ints[0]), month(ints[1]),
                    day_of_month(ints[2]));
  }
}; /**/

template <> class ReadInDate<YMDFormat::DDMMYYYY> {
private:
  static constexpr const int SZ = 10;

public:
  static ymd_date read(const char *str, const char **end) {
    int ints[3];
    if (get_three_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYMMDD from string %.10s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    return ymd_date(year(ints[2]), month(ints[1]),
                    day_of_month(ints[0]));
  }
}; /**/

template <> class ReadInDate<YMDFormat::YYYYDDD> {
private:
  static constexpr const int SZ = 8;

public:
  static ymd_date read(const char *str, const char **end) {
    int ints[2];
    if (get_two_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYDDD from string %.8s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    ydoy_date ydoy{year(ints[0]), day_of_year(ints[1])};
    return ydoy.to_ymd();
  }
}; /**/

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S, HMSFormat F>
#else
template <typename S, HMSFormat F,
          typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class ReadInTime {
};

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class ReadInTime<S, HMSFormat::HHMMSS> {
  typedef typename S::underlying_type SecIntType;
  static constexpr SecIntType scale = S::template sec_factor<SecIntType>();

public:
  static const int numChars = 8;
  static hms_time<S> read(const char *str, const char **end) {
    long ints[3];
    if (get_three_ints(str, ints, numChars + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving HHMMSS from string %.8s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving time\n");
    }
    return dso::hms_time<S>(dso::hours(ints[0]), dso::minutes(ints[1]),
                            S(ints[2] * scale));
  }
};

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class ReadInTime<S, HMSFormat::HHMMSSF> {
  typedef typename S::underlying_type SecIntType;
  static constexpr long double scale = S::template sec_factor<long double>();

public:
  static const int numChars = 8 + 12;
  static hms_time<S> read(const char *str, const char **end) {
    int ints[2];
    double fsec;
    if (get_two_ints_double(str, ints, fsec, numChars + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving HHMMSSF from string %.17s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving time\n");
    }
    return dso::hms_time<S>(dso::hours(ints[0]), dso::minutes(ints[1]),
                            S(static_cast<SecIntType>(fsec * scale)));
  }
};

template <YMDFormat FD, HMSFormat FT, typename S>
dso::datetime<S> from_char(const char *str, const char **end = nullptr) {
  const char *stop;
  /* resolve date part */
  const ymd_date ymd(ReadInDate<FD>::read(str, &stop));
  if (!ymd.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in date (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in date\n");
  }
  /* resolve time */
  str = stop;
  const hms_time<S> hms(ReadInTime<S, FT>::read(str, &stop));
  if (!hms.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in time (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in time\n");
  }
  /* set output pointer */
  if (*end)
    *end = stop;
  /* compile datetime instance */
  return datetime<S>(ymd, hms);
}

template <YMDFormat FD, HMSFormat FT>
dso::TwoPartDate from_char(const char *str, const char **end = nullptr) {
  const char *stop;
  /* resolve date part */
  const ymd_date ymd(ReadInDate<FD>::read(str, &stop));
  if (!ymd.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in date (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in date\n");
  }
  /* resolve time */
  str = stop;
  const hms_time<nanoseconds> hms(
      ReadInTime<nanoseconds, FT>::read(str, &stop));
  if (!hms.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in time (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in time\n");
  }
  /* set output pointer */
  if (*end)
    *end = stop;
  /* compile datetime instance */
  return TwoPartDate(modified_julian_day(ymd).as_underlying_type(),
                     hms.fractional_seconds<seconds>());
}

template <YMDFormat FD, HMSFormat FT>
dso::TwoPartDateUTC from_char(const char *str, const char **end = nullptr) {
  const char *stop;
  /* resolve date part */
  const ymd_date ymd(ReadInDate<FD>::read(str, &stop));
  if (!ymd.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in date (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in date\n");
  }
  /* resolve time */
  str = stop;
  const hms_time<nanoseconds> hms(
      ReadInTime<nanoseconds, FT>::read(str, &stop));
  if (!hms.is_valid()) {
    /* not always an error, if seconds are 60, it could be ok on a leap 
     * insertion day 
     */
    int leap;
    dat(modified_julian_day(ymd), leap);
    if (!hms.is_valid(leap)) {
      fprintf(stderr,
              "[ERROR] Failed to resolved read-in time (traceback: %s)\n",
              __func__);
      throw std::runtime_error("[ERROR] Failed to resolved read-in time\n");
    }
  }
  /* set output pointer */
  if (*end)
    *end = stop;
  /* compile datetime instance */
  return TwoPartDateUTC(modified_julian_day(ymd).as_underlying_type(),
                     hms.fractional_seconds<seconds>());
}
} /* namespace dso */

#endif
