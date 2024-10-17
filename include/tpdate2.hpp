/** @file
 * A utility class to hold datetime instances, in a continuous time-scale
 * (e.g. TT, TAI, etc). In construst to datetime<S>, this is not a template
 * class and uses a storage method of two numeric values (one for MJD and one
 * for seconds of day) to represent datetime epochs.
 */

#ifndef __DSO_DATETIME_TWOPARTDATES_ALTERNATIVE_HPP__
#define __DSO_DATETIME_TWOPARTDATES_ALTERNATIVE_HPP__

#include "dtdatetime.hpp"

namespace dso {

/** A datetime class to represent epochs in any continuous system.
 *
 * A TwoPartDate instance conviniently splits a datetime into two numeric
 * values:
 * - the Modified Julian Day (i.e. an integral value), and
 * - the time of day, which is stored in fractional seconds of day
 *
 * The methods of the class, invluding constructors, take special care to
 * always keep the seconds as seconds of day, i.e. in the range [0,86400) and
 * correspondingly increase/decrease the day count.
 *
 */
class TwoPartDate2 {
private:
  using FDOUBLE = /*long*/ double;
  int _mjd;      /** Mjd */
  FDOUBLE _fday; /** fractional part of day */

  /** Construct from MJD and fractional days.
   *
   * This is only private and should be used in rare cases. Normal users,
   * should explicitely cast the second argument to FractionalSeconds or
   * FractionalDays to avoid misconceptions (i.e. is the parameters fractional
   * seconds or fractional days?).
   */
  explicit TwoPartDate2(int mjd, FDOUBLE fday) noexcept
      : _mjd(mjd), _fday(fday) {
    normalize();
  }

public:
  /** Constructor from datetime<T> */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  TwoPartDate2(const datetime<T> &d) noexcept
      : _mjd(d.imjd().as_underlying_type()),
        _fday(to_fractional_days<T, FDOUBLE>(d.sec())) {
    this->normalize();
  }

  /** Constructor from a pair of doubles, such that TODO */
  explicit TwoPartDate2(int b = 0,
                        FractionalSeconds s = FractionalSeconds{0}) noexcept
      : _mjd(b), _fday(s.seconds() / SEC_PER_DAY) {
    this->normalize();
  }

  /** Get the MJD as an intgral number, i.e. no fractional part */
  int imjd() const noexcept { return _mjd; }

  /** Get the fractional seconds of the MJD */
  FDOUBLE seconds() const noexcept { return _fday * SEC_PER_DAY; }

  FDOUBLE fractional_days() const noexcept { return _fday; }

  /** @brief Transform the (integral part of the) date to Year Month Day */
  ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

  /** Add seconds to instance.
   * @warning Does not take into account leap seconds.
   */
  void add_seconds(FDOUBLE sec) noexcept {
    _fday += sec / SEC_PER_DAY;
    this->normalize();
  }
  void add_seconds2(FDOUBLE sec) noexcept {
    _fday *= SEC_PER_DAY;
    _fday += sec;
    _fday /= SEC_PER_DAY;
    this->normalize();
  }

  /** Difference between two dates as integral number of days and seconds of
   * day
   *
   * This is not an 'actual date' but rather a datetime interval, but can be
   * represented by a TwoPartDate instance. If the calling instance is prior to
   * the operand (i.e. d1-d2 with d2>d1) the interval is signed as 'negative'.
   * This means that the number of days can be negative, but the fractional
   * day will always be positive
   */
  TwoPartDate2 operator-(const TwoPartDate2 &d) const noexcept {
    return TwoPartDate2(_mjd - d._mjd, _fday - d._fday);
  }

  /** Add two instances.
   *
   * In this case, the second instance can be though of a datetime interval
   * (rather than an actual datetime instance). The right operand can be a
   * negative interval, which means that we are going backwards in time.
   */
  TwoPartDate2 operator+(const TwoPartDate2 &d) const noexcept {
    return TwoPartDate2(_mjd + d._mjd, _fday + d._fday);
  }

  /** Get the difference between two datetime instances an an arithmetic value.
   *
   * The difference can be obtained as a fractional days or fractional seconds,
   * depending on the template parameter \p DT.
   * If called as diff(d1,d2), the computation is d1-d2; the difference can be
   * negative if d2>d1.
   *
   * @warning Does not take into account leap seconds.
   */
  template <DateTimeDifferenceType DT>
  FDOUBLE diff(const TwoPartDate2 &d) const noexcept {
    if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
      /* difference as fractional days */
      return (_mjd - d._mjd) + (_fday - d._fday);
    } else if constexpr (DT == DateTimeDifferenceType::FractionalSeconds) {
      /* difference as fractional seconds */
      return (_fday - d._fday) * SEC_PER_DAY + (_mjd - d._mjd) * SEC_PER_DAY;
    } else {
      /* difference as fractional (julian) years */
      return this->diff<DateTimeDifferenceType::FractionalDays>(d) /
             DAYS_IN_JULIAN_YEAR;
    }
  }

  /** Get the date as (fractional) Julian Date */
  FDOUBLE julian_date() const noexcept { return _fday + (_mjd + dso::MJD0_JD); }

  FDOUBLE as_mjd() const noexcept { return _fday + _mjd; }

  FDOUBLE jcenturies_sinceJ2000() const noexcept {
    return (_mjd - J2000_MJD) / DAYS_IN_JULIAN_CENT +
           _fday / DAYS_IN_JULIAN_CENT;
  }

  bool operator>(const TwoPartDate2 &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fday > d._fday));
  }
  bool operator>=(const TwoPartDate2 &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fday >= d._fday));
  }
  bool operator<(const TwoPartDate2 &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fday < d._fday));
  }
  bool operator<=(const TwoPartDate2 &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fday <= d._fday));
  }

  /** @brief Normalize an instance.
   */
  void normalize() noexcept {
    double extra_days;
    _fday = std::modf(_fday, &extra_days);
    _mjd += extra_days;
#ifdef DEBUG
    assert(_fday >= 0e0 && _fday < 1e0);
#endif
    /* all done */
    return;
  }

  TwoPartDate2 normalized() const noexcept {
    TwoPartDate2 d(*this);
    d.normalize();
    return d;
  }

  /** @brief Equality operator */
  bool operator==(const TwoPartDate2 &d) const noexcept {
    return (_mjd == d._mjd) && (_fday == d._fday);
  }

  /** @brief In-equality operator */
  bool operator!=(const TwoPartDate2 &d) const noexcept {
    return !(this->operator==(d));
  }

}; /* class TwoPartDate2 */

} /* namespace dso */

#endif
