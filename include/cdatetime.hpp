/** @file
 * Fundamental (core) datetime constants.
 */

#ifndef __DSO_DATETIME_CONSTANTS_HPP__
#define __DSO_DATETIME_CONSTANTS_HPP__

namespace dso {

/** Jan 1st 1980 for GPS Time */
constexpr const int JAN61980 = 44'244L;

/** */
constexpr const int JAN11901 = 15'385L;

/** @brief Seconds per day.
 * @warning This is not always true in case of UTC dates; the day a leap
 * second is inserted has one more second!
 */
constexpr const double SEC_PER_DAY = 86400e0;

/** Days per Julian year. */
constexpr const double DAYS_IN_JULIAN_YEAR = 365.25e0;

/* Days per Julian century. */
constexpr const double DAYS_IN_JULIAN_CENT = 36525e0;

/** Reference epoch (J2000.0), Julian Date. */
constexpr const double J2000_JD = 2451545e0;

/** Reference epoch (J2000.0), Modified Julian Date. */
constexpr const double J2000_MJD = 51544.5e0;

/** Julian Date of Modified Julian Date zero. */
constexpr const double MJD0_JD = 2400000.5e0;

/** TT minus TAI in [sec] */
constexpr const double TT_MINUS_TAI = 32.184e0;

/** TT minus TAI in [nsec] */
constexpr const long TT_MINUS_TAI_IN_NANOSEC = 32184 * 1'000'000L;

} /* namespace dso */

#endif
