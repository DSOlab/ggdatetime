/** @file
 * A utility class to hold datetime instances, in a continuous time-scale 
 * (e.g. TT, TAI, etc). In construst to datetime<S>, this is not a template 
 * class and uses a storage method of two numeric values (one for MJD and one 
 * for seconds of day) to represent datetime epochs.
 */

#ifndef __DSO_DATETIME_TWOPARTDATES2_HPP__
#define __DSO_DATETIME_TWOPARTDATES2_HPP__

#include "dtcalendar.hpp"

namespace dso {
class TwoPartDateUTC {
private:
  using FDOUBLE = /*long*/ double;
  int _mjd;  /** Mjd */
  FDOUBLE _fsec; /** fractional seconds of day */
  int _secInDay; /** number of seconds in day, i.e. 86400 or 86401 */

  /** Check the integral (MJD) part of the date; if we are on a leap 
   * insertation day, return 1, otherwise return 0
   */
  int extra_seconds_in_day() const noexcept {
    int extra;
    dat(_mjd, extra);
    return extra;
  }

public:
  /** Constructor from datetime<T> */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  TwoPartDateUTC(const datetime<T> &d) noexcept
      : _mjd(d.imjd().as_underlying_type()),
        _fsec(to_fractional_seconds<T,FDOUBLE>(d.sec()))
  {
    this->normalize();
    _secInDay = 86400 + this->extra_seconds_in_day();
  }

  /** Constructor from a pair of doubles, such that MJD = a + b */
  explicit TwoPartDateUTC(int b = 0, FDOUBLE s = 0) noexcept
      : _mjd(b), _fsec(s) {
    this->normalize();
    _secInDay = 86400 + this->extra_seconds_in_day();
  }

  /** Get the MJD as an intgral number, i.e. no fractional part */
  int imjd() const noexcept { return _mjd; }

  /** Get the fractional seconds of the MJD */
  FDOUBLE seconds() const noexcept { return _fsec; }
  
  /** @brief Get the fractional seconds of day as some multiple of seconds.
   *
   * 'Some multiple of seconds' can be any type defined with a member 
   * is_of_sec_type set to true. In practical terms, this means that you can
   * use seconds, milliseconds, microseconds or nanoseconds.
   * So, in essence this function will give you the seconds of day, or the 
   * milliseconds of day, or ...
   * The function will return the [whatever]seconds of day as a floating point 
   * number. 
   *
   * @return Fractional day as fractional T-type second multiples. I.e. 
   *         fractional seconds of day, or milliseconds of day, or 
   *         microseconds of day, etc ...
   */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  FDOUBLE sec_of_day() const noexcept {
    return seconds() * T:: template sec_factor<FDOUBLE>();
  }

  /** @brief Transform the (integral part of the) date to Year Month Day */
  ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

  /** Add seconds to instance, taking into account leap seconds.
   */
  void add_seconds(FDOUBLE sec) noexcept {
    _fsec += sec;
    this->normalize();
    _secInDay = 86400 + this->extra_seconds_in_day();
  }

  bool operator>(const TwoPartDateUTC &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec > d._fsec));
  }
  bool operator>=(const TwoPartDateUTC &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec >= d._fsec));
  }
  bool operator<(const TwoPartDateUTC &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec < d._fsec));
  }
  bool operator<=(const TwoPartDateUTC &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec <= d._fsec));
  }

  /** @brief Equality operator */
  bool operator==(const TwoPartDateUTC &d) const noexcept {
    return (_mjd == d._mjd) && (_fsec == d._fsec);
  }

  /** @brief In-equality operator */
  bool operator!=(const TwoPartDateUTC &d) const noexcept {
    return !(this->operator==(d));
  }

  /** @brief Normalize an instance. 
   *
   * Normalize here is meant in the sense that the fractional seconds of day 
   * (\p _fsec) are always in the range [0,86400), while the integral part 
   * (\p _mjd) is always a valid integer.
   * The only case where negative seconds are allowed, is when the MJD part is 
   * zero. In this case, the seconds of day are allowed to be negative, so 
   * that they can hold the sign.
   */
  void normalize() noexcept {
    assert(_fsec >= 0e0);
    int extra_sec_in_day;
    dat(_mjd, extra_sec_in_day);
    /* for each MJD, remove integral days. Each MJD may have a different 
     * number of seconds, since we are in UTC time scale. Hence, iteratively 
     * remove whole days using the number of seconds for each day
     */
    while (_fsec >= 86400e0+extra_sec_in_day) {
      _fsec -= (86400e0+extra_sec_in_day);
      ++_mjd;
      dat(_mjd, extra_sec_in_day);
    }
#ifdef DEBUG
    if (_mjd) assert(_fsec >= 0e0 && _fsec < 86400e0);
    else assert(_fsec < 0e0 && _fsec > -86400e0);
#endif
    /* all done */
    return;
  }

  /** @brief Transform a UTC date to a TAI date.
   *
   * The TAI date is returned in two parts:
   * 1. The MJD part (integral), and
   * 2. The seconds of day part (fractional)
   * Note that the fractional seconds part may be over 86400.
   */
  int utc2tai(FDOUBLE &taisec) const noexcept {
    taisec = _fsec + dat(modified_julian_day(_mjd));
    return _mjd;
  }

  /** @brief Transform a UTC date to a TT date.
   *
   * The TT date is returned in two parts:
   * 1. The MJD part (integral), and
   * 2. The seconds of day part (fractional)
   * Note that the fractional seconds part may be over 86400.
   */
  int utc2tt(FDOUBLE &taisec) const noexcept {
    constexpr const FDOUBLE dtat = TT_MINUS_TAI;
    taisec = _fsec + (dat(modified_julian_day(_mjd))+dtat);
    return _mjd;
  }
}; /* class TwoPartDateUTC */

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
class TwoPartDate {
private:
  using FDOUBLE = /*long*/ double;
  int _mjd;  /** Mjd */
  FDOUBLE _fsec; /** fractional seconds of day */

public:
  /** Constructor from datetime<T> */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  TwoPartDate(const datetime<T> &d) noexcept
      : _mjd(d.imjd().as_underlying_type()),
        _fsec(to_fractional_seconds<T,FDOUBLE>(d.sec())) 
  {
    this->normalize();
  }

  /** Constructor from a pair of doubles, such that MJD = a + b */
  explicit TwoPartDate(int b = 0, FDOUBLE s = 0) noexcept
      : _mjd(b), _fsec(s) {
    this->normalize();
  }

  /** Get the MJD as an intgral number, i.e. no fractional part */
  int imjd() const noexcept { return _mjd; }

  /** Get the fractional seconds of the MJD */
  FDOUBLE seconds() const noexcept { return _fsec; }
  
  /** @brief Get the fractional seconds of day as some multiple of seconds.
   *
   * 'Some multiple of seconds' can be any type defined with a member 
   * is_of_sec_type set to true. In practical terms, this means that you can
   * use seconds, milliseconds, microseconds or nanoseconds.
   * So, in essence this function will give you the seconds of day, or the 
   * milliseconds of day, or ...
   * The function will return the [whatever]seconds of day as a floating point 
   * number. 
   *
   * @return Fractional day as fractional T-type second multiples. I.e. 
   *         fractional seconds of day, or milliseconds of day, or 
   *         microseconds of day, etc ...
   */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  FDOUBLE sec_of_day() const noexcept {
    return seconds() * T:: template sec_factor<FDOUBLE>();
  }

  /** @brief Transform the (integral part of the) date to Year Month Day */
  ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

  /** Add seconds to instance.
   * @warning Does not take into account leap seconds.
   */
  void add_seconds(FDOUBLE sec) noexcept {
    _fsec += sec;
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
  TwoPartDate operator-(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_mjd - d._mjd, _fsec - d._fsec);
  }

  /** Add two instances.
   *
   * In this case, the second instance can be though of a datetime interval
   * (rather than an actual datetime instance). The right operand can be a
   * negative interval, which means that we are going backwards in time.
   */
  TwoPartDate operator+(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_mjd + d._mjd, _fsec + d._fsec);
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
  FDOUBLE diff(const TwoPartDate &d) const noexcept {
    if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
      /* difference as fractional days */
      return (_mjd - d._mjd) + (_fsec - d._fsec) / SEC_PER_DAY;
    } else {
      /* difference as fractional seconds */
      return (_fsec - d._fsec) + (_mjd - d._mjd) * SEC_PER_DAY;
    }
  }

  /** Get the date as (fractional) Julian Date */
  FDOUBLE julian_date() const noexcept {
    return _fsec / SEC_PER_DAY + (_mjd + dso::MJD0_JD);
  }

  /** @brief Transform instance to TT, assuming it is in TAI
   *
   * The two time scales are connected by the formula:
   * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
   */
  TwoPartDate tai2tt() const noexcept {
    constexpr const FDOUBLE dtat = TT_MINUS_TAI;
    return TwoPartDate(_mjd, _fsec + dtat);
  }
  
  /** @brief Transform an instance to TAI assuming it is in TT 
   *
   * The two time scales are connected by the formula:
   * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
   */
  TwoPartDate tt2tai() const noexcept {
    constexpr const FDOUBLE dtat = TT_MINUS_TAI;
    return TwoPartDate(_mjd, _fsec - dtat);
  }

  /** Transform an instance to TAI, assuming it is in UTC
   *
   * The two time scales are connected by the formula:
   * \f$ UTC = TAI + ΔAT \$ where ΔAT are the leap seconds.
   */
  //TwoPartDate utc2tai() const noexcept {
  //  constexpr const FDOUBLE SPD = SEC_PER_DAY;
  //  auto utc = *this;
  //  /* Get TAI-UTC at 0h today and extra seconds in day (if any) */
  //  int extra;
  //  const int leap = dat(modified_julian_day((int)utc._mjd), extra);
  //  /* Remove any scaling applied to spread leap into preceding day */
  //  utc._fday *= (SPD + extra) / SPD;
  //  /* Assemble the TAI result, preserving the UTC split and order */
  //  return TwoPartDate(utc._mjd, utc._fday + leap / SPD);
  //}

  /** Transform an instance to TT assuming it is in UTC */
  //TwoPartDate utc2tt() const noexcept {
  //  const auto tai = this->utc2tai();
  //  return tai.tai2tt();
  //}

  ///**  Transform an instance to UTC assuming it is in TAI */
  //TwoPartDate tai2utc() const noexcept {
  //  // do it the SOFA way ...
  //  auto utc1(*this);
  //  FDOUBLE small = utc1._fday;
  //  FDOUBLE big = utc1._mjd;
  //  for (int i = 0; i < 3; i++) {
  //    TwoPartDate guess = TwoPartDate(big, small).utc2tai();
  //    small += utc1._mjd - guess._mjd;
  //    small += utc1._fday - guess._fday;
  //  }
  //  return TwoPartDate(utc1._mjd, small);
  //}

  ///**  Transform an instance to UTC assuming it is in TT */
  //TwoPartDate tt2utc() const noexcept {
  //  const auto tai = this->tt2tai();
  //  return tai.tai2utc();
  //}

  /* @brief TT to UT1 MJD
   *
   * Note that because the Earth’s rotation is slowing due to tidal friction,
   * and the rotation rate decreases approximately linearly with time, ∆T
   * increases quadratically.
   * @param[in] dut1 ΔUT1 in [sec]. This value should be looked up using e.g.
   *                 IERS products
   * @return The corresponding UT1 MJD, computed using:
   *         ∆T = TT − UT1 = 32.184[sec] + ∆AT − ∆UT1
   */
  //TwoPartDate tt2ut1(FDOUBLE dut1) const noexcept {
  //  /* note that ΔUT1 = UT1 − UTC hence UT1 = ΔUT1 + UTC */
  //  return tt2utc() + TwoPartDate(0e0, dut1 / SEC_PER_DAY);
  //}

  FDOUBLE as_mjd() const noexcept { return _fsec/SEC_PER_DAY + _mjd; }

  FDOUBLE jcenturies_sinceJ2000() const noexcept {
    return (_mjd - J2000_MJD) / DAYS_IN_JULIAN_CENT +
           _fsec/ SEC_PER_DAY / DAYS_IN_JULIAN_CENT;
  }

  bool operator>(const TwoPartDate &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec > d._fsec));
  }
  bool operator>=(const TwoPartDate &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec >= d._fsec));
  }
  bool operator<(const TwoPartDate &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec < d._fsec));
  }
  bool operator<=(const TwoPartDate &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec <= d._fsec));
  }

  /** @brief Normalize an instance. 
   *
   * Normalize here is meant in the sense that the fractional seconds of day 
   * (\p _fsec) are always in the range [0,86400), while the integral part 
   * (\p _mjd) is always a valid integer.
   * The only case where negative seconds are allowed, is when the MJD part is 
   * zero. In this case, the seconds of day are allowed to be negative, so 
   * that they can hold the sign.
   */
  void normalize() noexcept {
    /* remove whole days from seconds and compute signed remainder */
    auto srem = std::fmod(_fsec, seconds::max_in_day);
    int extradays = _fsec / seconds::max_in_day;
    /* only allow negative seconds if whole days are zero */
    if ((srem < 0e0) && (_mjd + extradays)) {
      --extradays;
      srem += seconds::max_in_day;
    }
    _fsec = srem;
    _mjd += extradays;
#ifdef DEBUG
    if (_mjd) assert(_fsec >= 0e0 && _fday < 86400e0);
    else assert(_fsec < 0e0 && _fday > -86400e0);
#endif
    /* all done */
    return;
  }

  TwoPartDate normalized() const noexcept {
    TwoPartDate d(*this);
    d.normalize();
    return d;
  }

  /** @brief Equality operator */
  bool operator==(const TwoPartDate &d) const noexcept {
    return (_mjd == d._mjd) && (_fsec == d._fsec);
  }

  /** @brief In-equality operator */
  bool operator!=(const TwoPartDate &d) const noexcept {
    return !(this->operator==(d));
  }

}; /* class TwoPartDate */

//TwoPartDate utc2tai(const TwoPartDate &d) noexcept;

} /* namespace dso */

#endif
