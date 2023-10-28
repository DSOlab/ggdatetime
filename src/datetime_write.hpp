/** @file
 * Functions to pretty print datetime instances
 */

#ifndef __NGPT_DT_WRITERS__
#define __NGPT_DT_WRITERS__

#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include "hms_time.hpp"
#include "tpdate.hpp"
#include <stdexcept>
#include <cstdio>

namespace dso {

enum class YMDFormat { YYYYMMDD, DDMMYYYY, YYMMDD, DDMMYY };

template <YMDFormat F> class SpitDate {};
template<> class SpitDate<YMDFormat::YYYYMMDD> {
public:
  static const int numChars = 10;
  static int spit(const ymd_date &ymd, char *buffer) noexcept {
    return std::sprintf(buffer, "%4d/%02d/%02d", ymd.yr().as_underlying_type(),
                        ymd.mn().as_underlying_type(),
                        ymd.dm().as_underlying_type());
  }
};
template<> class SpitDate<YMDFormat::DDMMYYYY> {
public:
  static const int numChars = 10;
  static int spit(const ymd_date &ymd, char *buffer) noexcept {
    return std::sprintf(buffer, "%02d/%02d/%4d", ymd.dm().as_underlying_type(),
                        ymd.mn().as_underlying_type(),
                        ymd.yr().as_underlying_type());
  }
};

/** Format a ymd_date instace to a string and write it to buffer.
 *
 * The way the date is formated, is dictated by the teplate parameter \p F.
 * The buffer must be of size able to store all characters specified by \p F 
 * Note that months and day of months are written using width of 2 chatacters, 
 * that is if < 10, a '0' is prepended.
 * For example ymd_date(year(2023), month(10), day_of_month(3)) will be:
 * '2023/10/03' in the YMDFormat::YYYYMMDD format.
 *
 * @param[in] ymd The ymd_date instance to format
 * @param[out] buffer The non-null terminated C-string holding the ymd_date 
 *             instance in the specified format. Its size is dicated by \p F 
 *             and (at input) must be long enough to hold all charactes needed
 *             to represent the date
 * @return On success, a pointer to \p buffer
 */
template<YMDFormat F>
const char *to_char(const ymd_date &ymd, char *buffer) {
  if (SpitDate<F>::spit(ymd, buffer) != SpitDate<F>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format date to string\n");
  }
  return buffer;
}

enum class HMSFormat { HHMMSS, HHMMSSF };

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S, HMSFormat F>
#else
template <typename S, HMSFormat F, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class SpitTime {};

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class SpitTime<S, HMSFormat::HHMMSS> {
  typedef typename S::underlying_type SecIntType;
public:
  static const int numChars = 8;
  static int spit(const hms_time<S> &hms, char *buffer) noexcept {
    /* (integral) seconds of minute */
    SecIntType sec = cast_to<S, seconds>(hms.nsec()).as_underlying_type();
    return std::sprintf(
        buffer, "%02d:%02d:%02ld", hms.hr().as_underlying_type(),
        hms.mn().as_underlying_type(), sec);
  }
};

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S>
#endif
class SpitTime<S, HMSFormat::HHMMSSF> {
public:
  static const int numChars = 18;
  static int spit(const hms_time<S> &hms, char *buffer) noexcept {
    /* seconds of minute (real) */
    double sec = to_fractional_seconds(hms.nsec());
    return std::sprintf(
        buffer, "%02d:%02d:%012.9f", hms.hr().as_underlying_type(),
        hms.mn().as_underlying_type(), sec);
  }
};

/** Format a hms_time<S> instace to a string and write it to buffer.
 *
 * The way the time is formated, is dictated by the teplate parameter \p F.
 * The buffer must be of size able to store all characters specified by \p F 
 * Note that hours and minutes of day are written using width of 2 chatacters, 
 * that is if < 10, a '0' is prepended.
 *
 * @param[in] hms The hms_time<S> instance to format
 * @param[out] buffer The non-null terminated C-string holding the hms_time<S> 
 *             instance in the specified format. Its size is dicated by \p F 
 *             and (at input) must be long enough to hold all characters 
 *             needed to represent the time
 * @return On success, a pointer to \p buffer
 */
template<HMSFormat F, typename S>
const char *to_char(const hms_time<S> &hms, char *buffer) {
  if (SpitTime<S,F>::spit(hms, buffer) != SpitTime<S,F>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format time to string\n");
  }
  return buffer;
}


/** Format a datetime<S> instace to a string and write it to buffer.
 *
 * The way the instance is formated, is dictated by the teplate parameters 
 * \p FT and \p FD. The buffer must be of size able to store all characters 
 * specified by the format (date, time plus an extra whitespace character to 
 * be placed between date and time).
 *
 * @param[in] d The datetime<S> instance to format
 * @param[out] buffer The non-null terminated C-string holding the datetime<S> 
 *             instance in the specified format. Its size is dicated by \p FD 
 *             and \p FT and (at input) must be long enough to hold all 
 *             characters needed to represent the datetime
 * @return On success, a pointer to \p buffer
 */
template<YMDFormat FD, HMSFormat FT, typename S>
const char *to_char(const datetime<S> &d, char *buffer) {
  /* write date to buffer */
  ymd_date ymd(d.as_ymd());
  if (SpitDate<FD>::spit(ymd, buffer) != SpitDate<FD>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format date to string\n");
  }
  /* move pointer to write time */
  char *ptr = buffer + SpitDate<FD>::numChars;
  *ptr = ' ';
  ++ptr;
  /* write time of day to buffer */
  hms_time<S> hms(d.sec());
  if (SpitTime<S,FT>::spit(hms, ptr) != SpitTime<S,FT>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format time to string\n");
  }
  return buffer;
}

template <YMDFormat FD, HMSFormat FT>
const char *to_char(const TwoPartDate &d, char *buffer) {
  /* write date to buffer */
  ymd_date ymd(d.to_ymd());
  if (SpitDate<FD>::spit(ymd, buffer) != SpitDate<FD>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format date to string\n");
  }
  /* move pointer to write time */
  char *ptr = buffer + SpitDate<FD>::numChars;
  *ptr = ' ';
  ++ptr;
  /* write time of day to buffer */
  const nanoseconds ns(static_cast<nanoseconds::underlying_type>(
      std::round(d.sec_of_day<nanoseconds>())));
  hms_time<nanoseconds> hms(ns);
  if (SpitTime<nanoseconds, FT>::spit(hms, ptr) !=
      SpitTime<nanoseconds, FT>::numChars) {
    throw std::runtime_error("[ERROR] Failed to format time to string\n");
  }
  return buffer;
}

} /* namespace dso */

#endif
