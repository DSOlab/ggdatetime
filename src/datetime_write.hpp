/** @file
 * Functions to pretty print datetime instances
 */

#ifndef __NGPT_DT_WRITERS__
#define __NGPT_DT_WRITERS__

#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include "hms_time.hpp"
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
    throw std::runtime_error("[ERROR] Failed to fromat date to string\n");
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
  static const int numChars = 17;
  static int spit(const hms_time<S> &hms, char *buffer) noexcept {
    /* seconds of minute (real) */
    double sec = to_fractional_seconds(hms.nsec());
    return std::sprintf(
        buffer, "%02d:%02d:%011.9f", hms.hr().as_underlying_type(),
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
    throw std::runtime_error("[ERROR] Failed to fromat time to string\n");
  }
  return buffer;
}

} /* namespace dso */

#endif
