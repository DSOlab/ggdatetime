/** @file
 *
 * Fundamental (core) datetime algorithms. The functions in here are agnostic
 * w.r.t the classes defined by the project (i.e. year, mnoth, etc). They act
 * mostly on integral types, hence they should be used with care.
 *
 * Once the relevant datetime classes are available (in another hpp file),
 * they should be wrapped around the "correct" implementation enabling type
 * safety.
 */

#ifndef __DSO_NONTYPE_CALENDAR_UTILS_HPP__
#define __DSO_NONTYPE_CALENDAR_UTILS_HPP__

#include "cdatetime.hpp"
#include <cmath>
#include <stdexcept>

namespace dso::core {
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
 *
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
} /* namespace dso::core */

#endif
