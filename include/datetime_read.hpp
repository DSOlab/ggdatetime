/** @file
 * Functions to read in datetime<S>, TwoPartDate and TwoPartDateUTC instances
 * from various input formats.
 */

#ifndef __DSO_DATETIME_IO_READ_HPP__
#define __DSO_DATETIME_IO_READ_HPP__

#include "core/datetime_io_core.hpp"
#include "datetime_utc.hpp"
#include "tpdate.hpp"
#include <stdexcept>

namespace dso {

/** A generic Date parser, based on the specified Date format \p YMDFormat */
template <YMDFormat F> class ReadInDate {};

/** A Date parser expecting a Date-format of type YYYY:MM:DD
 *
 * The delimeter character (here denoted ':'), can actually be any character
 * of: ' ', '/', '-', 'T', '_' and ':', so that a date can be given as e.g.
 * "2023:10:07", or
 * "2023/10/07", or
 * "2023 10 07", or
 * "2023/10:07", or
 * "2023 10:07", ...etc
 * The instance expects that the string has a minimum size of \p SZ characters
 * (i.e. 10).
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a ymd_date instance.
 */
template <> class ReadInDate<YMDFormat::YYYYMMDD> {
  static constexpr const int SZ = 10;

public:
  /** Read in and parse the date.
   *
   * No validation is performed at the resolved date (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * SZ characters are the date representation.
   * The string does not have to be null-terminated; only SZ characters are
   * checked after the first valid character.
   *
   * @param[in] str A string representing a date of type: "YYYYMMDD"
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static ymd_date read(const char *str, const char **end) {
    int ints[3];
    if (datetime_io_core::get_three_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYMMDD from string %.10s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    return ymd_date(year(ints[0]), month(ints[1]), day_of_month(ints[2]));
  }
}; /* ReadInDate<YMDFormat::YYYYMMDD> */

/** A Date parser expecting a Date-format of type DD:MM:YYYY
 *
 * The delimeter character (here denoted ':'), can actually be any character
 * of: ' ', '/', '-', 'T', '_' and ':', so that a date can be given as e.g.
 * "07:10:2023" or
 * "07/10/2023" or
 * "07 10 2023" or
 * "07:10/2023" or
 * "07:10 2023" ..etc
 * The instance expects that the string has a minimum size of \p SZ characters
 * (i.e. 10).
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a ymd_date instance.
 */
template <> class ReadInDate<YMDFormat::DDMMYYYY> {
  static constexpr const int SZ = 10;

public:
  /** Read in and parse the date.
   *
   * No validation is performed at the resolved date (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * SZ characters are the date representation.
   * The string does not have to be null-terminated; only SZ characters are
   * checked after the first valid character.
   *
   * @param[in] str A string representing a date of type: "DDMMYYYY"
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static ymd_date read(const char *str, const char **end) {
    int ints[3];
    if (datetime_io_core::get_three_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYMMDD from string %.10s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    return ymd_date(year(ints[2]), month(ints[1]), day_of_month(ints[0]));
  }
}; /* ReadInDate<YMDFormat::DDMMYYYY> */

/** A Date parser expecting a Date-format of type YYYY:DDD where DDD is Day Of
 *  Year
 *
 * The delimeter character (here denoted ':'), can actually be any character
 * of: ' ', '/', '-', 'T', '_' and ':', so that a date can be given as e.g.
 * "2023:365" or
 * "2023-365" or
 * "2023/365" or
 * "2023 365" or
 * "2023T365" ..etc
 * The instance expects that the string has a minimum size of \p SZ characters
 * (i.e. 8).
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a ymd_date instance.
 */
template <> class ReadInDate<YMDFormat::YYYYDDD> {
  static constexpr const int SZ = 8;

public:
  /** Read in and parse the date.
   *
   * No validation is performed at the resolved date (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * SZ characters are the date representation.
   * The string does not have to be null-terminated; only SZ characters are
   * checked after the first valid character.
   *
   * @param[in] str A string representing a date of type: "YYYYDDD"
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static ymd_date read(const char *str, const char **end) {
    int ints[2];
    if (datetime_io_core::get_two_ints(str, ints, SZ + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving YYYYDDD from string %.8s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving date\n");
    }
    ydoy_date ydoy{year(ints[0]), day_of_year(ints[1])};
    return ydoy.to_ymd();
  }
}; /* ReadInDate<YMDFormat::YYYYDDD> */

/** A generic Time-Of-Day parser */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S, HMSFormat F>
#else
template <typename S, HMSFormat F,
          typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class ReadInTime {
};

/** A Time-Of-Day parser expecting a Time-format of type HHMMSS
 *
 * The delimeter character (here denoted ':'), can actually be any character
 * of: ' ', '/', '-', 'T', '_' and ':', so that a date can be given as e.g.
 * "23:59:59", or
 * "23/59/59", or
 * "23T59T59", or
 * "23 59 59", or
 * "23:59 59", ...etc
 * The instance expects that the string has a minimum size of \p SZ characters
 * (i.e. 8).
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a hms_time<S> instance.
 */
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
  /** Read in and parse the time-of-day.
   *
   * No validation is performed at the resolved time (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * SZ characters are the time representation.
   * The string does not have to be null-terminated; only SZ characters are
   * checked after the first valid character.
   *
   * @param[in] str A string representing a time-of-day of type: "HHMMSS"
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static hms_time<S> read(const char *str, const char **end) {
    long ints[3];
    if (datetime_io_core::get_three_ints(str, ints, numChars + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving HHMMSS from string %.8s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving time\n");
    }
    return hms_time<S>(dso::hours(ints[0]), dso::minutes(ints[1]),
                       S(ints[2] * scale));
  }
}; /* ReadInTime<S, HMSFormat::HHMMSS> */

/** A Time-Of-Day parser expecting a Time-format of type SSSSS.
 *
 * That is, only integral seconds of day are given.
 *
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a hms_time<S> instance.
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class ReadInTime<S, HMSFormat::ISECDAY> {
  typedef typename S::underlying_type SecIntType;
  static constexpr SecIntType scale = S::template sec_factor<SecIntType>();

public:
  static const int numChars = 5;
  /** Read in and parse the time-of-day.
   *
   * No validation is performed at the resolved time (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * (at max) SZ characters are the time representation.
   * The string does not have to be null-terminated; only numeric values (i.e.
   * digits) are considered.
   *
   * @param[in] str A string representing a time-of-day of type: "SSSSSS"
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static hms_time<S> read(const char *str, const char **end) {
    int ints;
    if (datetime_io_core::get_one_int(str, &ints, numChars + 1, end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving SSSSS from string %.8s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving time\n");
    }
    return hms_time<S>(S(ints * scale));
  }
}; /* ReadInTime<S, HMSFormat::HHMMSS> */

/** A Time-Of-Day parser expecting a Time-format of type HHMMSS.SSSSSSS...
 *
 * The delimeter character (here denoted ':'), can actually be any character
 * of: ' ', '/', '-', 'T', '_' and ':', so that a date can be given as e.g.
 * "23:59:59.012345678912", or
 * "23/59/59.012345678912", or
 * "23T59T59.012345678912", or
 * "23 59 59.012345678912", or
 * "23:59 59.012345678912", ...etc
 * The instance expects that the string has a minimum size of \p SZ characters
 * (i.e. 8+12).
 * The only usable member function is read, which will actually try to parse
 * the string and resolve it to a hms_time<S> instance.
 *
 * @warning This class can only resolve seconds up to (and including)
 *          nanosecond precision. Anything after the 1e-12 part, will be
 *          ignored!
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class ReadInTime<S, HMSFormat::HHMMSSF> {
  typedef typename S::underlying_type SecIntType;
  static constexpr long double scale = S::template sec_factor<long double>();

public:
  /** Read in and parse the time-of-day.
   *
   * No validation is performed at the resolved time (user can do that on
   * return). The string can start with any number of whitespace characters.
   * Once however the first digit is encountered, it is expected that the next
   * SZ characters are the time representation.
   * The string does not have to be null-terminated; only SZ characters are
   * checked after the first valid character.
   *
   * @param[in] str A string representing a time-of-day of type: "HHMMSS.SS..."
   *            Anything after the 1e-12 part, will be ignored!
   * @param[out] end If not nullptr, end will point at the first character not
   *             resolved
   */
  static const int numChars = 8 + 12;
  static hms_time<S> read(const char *str, const char **end) {
    int ints[2];
    double fsec;
    if (datetime_io_core::get_two_ints_double(str, ints, fsec, numChars + 1,
                                              end)) {
      fprintf(stderr,
              "[ERROR] Failed resolving HHMMSSF from string %.17s "
              "(traceback: %s)\n",
              str, __func__);
      throw std::runtime_error("[ERROR] Failed resolving time\n");
    }
    return dso::hms_time<S>(dso::hours(ints[0]), dso::minutes(ints[1]),
                            S(static_cast<SecIntType>(fsec * scale)));
  }
}; /* ReadInTime<S, HMSFormat::HHMMSSF> */

/** Read in a Date and Time of Day string and resolve it to a datetime<S>
 *  instance.
 *
 * The template parameters dictate how the date and time-of-day strings are
 * expected. The datetime instance will be validated before return (i.e.
 * only valid epochs are returned).
 *
 * @param[in] str A string respresenting a Date forlloed by a time-of-day part
 *            i.e. "[ ]*2023/10/07[ ]*13:56:59.012345678912[ ]*"
 *            or   "[ ]*2023/10/07[ ]*13:56:59[ ]*". For a
 *            detailed description of the allowed delimeters see ReadInTime<>
 *            and ReadInDate<> classes.
 * @param[out] end If not nullptr, end will point at the first character not
 *            resolved
 * @return The parsed string as a datetime<S> instance
 */
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
  if (end)
    *end = stop;
  /* compile datetime instance */
  return datetime<S>(ymd, hms);
}

template <YMDFormat FD, HMSFormat FT, typename S>
dso::datetime_utc<S> from_utc_char(const char *str,
                                   const char **end = nullptr) {
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
  if (end)
    *end = stop;
  /* compile datetime instance */
  return datetime_utc<S>(ymd, hms);
}

/** Read in a Date and Time of Day string and resolve it to a TwoPartDate
 *  instance.
 *
 * The template parameters dictate how the date and time-of-day strings are
 * expected. The TwoPartDate instance will be validated before return (i.e.
 * only valid epochs are returned).
 *
 * @warning If decimal seconds are included in the string, the parsing
 *  resolution is up to 1e-12 seconds, i.e. 1 picosecond. Any decimal part
 *  after this, will be ignored!
 *
 * @param[in] str A string respresenting a Date forlloed by a time-of-day part
 *            i.e. "[ ]*2023/10/07[ ]*13:56:59.012345678912[ ]*"
 *            or   "[ ]*2023/10/07[ ]*13:56:59[ ]*". For a
 *            detailed description of the allowed delimeters see ReadInTime<>
 *            and ReadInDate<> classes.
 * @param[out] end If not nullptr, end will point at the first character not
 *            resolved
 * @return The parsed string as a TwoPartDate instance
 */
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
  const hms_time<picoseconds> hms(
      ReadInTime<picoseconds, FT>::read(str, &stop));
  if (!hms.is_valid()) {
    fprintf(stderr, "[ERROR] Failed to resolved read-in time (traceback: %s)\n",
            __func__);
    throw std::runtime_error("[ERROR] Failed to resolved read-in time\n");
  }
  /* set output pointer */
  if (end)
    *end = stop;
  /* compile datetime instance */
  return TwoPartDate(modified_julian_day(ymd).as_underlying_type(),
                     hms.fractional_seconds<seconds>());
}

/** Read in a Date and Time of Day string and resolve it to a TwoPartDateUTC
 *  instance.
 *
 * The template parameters dictate how the date and time-of-day strings are
 * expected. The TwoPartDateUTC instance will be validated before return (i.e.
 * only valid epochs are returned).
 *
 * @warning If decimal seconds are included in the string, the parsing
 *  resolution is up to 1e-12 seconds, i.e. 1 picosecond. Any decimal part
 *  after this, will be ignored!
 *
 * @param[in] str A string respresenting a Date forlloed by a time-of-day part
 *            i.e. "[ ]*2023/10/07[ ]*13:56:59.012345678912[ ]*"
 *            or   "[ ]*2023/10/07[ ]*13:56:59[ ]*". For a
 *            detailed description of the allowed delimeters see ReadInTime<>
 *            and ReadInDate<> classes.
 * @param[out] end If not nullptr, end will point at the first character not
 *            resolved
 * @return The parsed string as a TwoPartDateUTC instance
 */
template <YMDFormat FD, HMSFormat FT>
dso::TwoPartDateUTC from_utc_char(const char *str, const char **end = nullptr) {
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
  const hms_time<picoseconds> hms(
      ReadInTime<picoseconds, FT>::read(str, &stop));
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
  if (end)
    *end = stop;
  /* compile datetime instance */
  return TwoPartDateUTC(modified_julian_day(ymd).as_underlying_type(),
                        hms.fractional_seconds<seconds>());
}
} /* namespace dso */

#endif
