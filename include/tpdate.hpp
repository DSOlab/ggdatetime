/** @file
 * A utility class to hold datetime instances, in a continuous time-scale
 * (e.g. TT, TAI, etc). In construst to datetime<S>, this is not a template
 * class and uses a storage method of two numeric values (one for MJD and one
 * for seconds of day) to represent datetime epochs.
 */

#ifndef __DSO_DATETIME_TWOPARTDATES2_HPP__
#define __DSO_DATETIME_TWOPARTDATES2_HPP__

#include "dtdatetime.hpp"
#include "dtfund.hpp"
#include <random>

namespace dso {

/** forward decleration */
class TwoPartDate;

class TwoPartDateUTC {
private:
  using FDOUBLE = /*long*/ double;
  int _mjd;      /** Mjd */
  FDOUBLE _fsec; /** fractional seconds of day */

  /** Check the integral (MJD) part of the date; if we are on a leap
   * insertation day, return 1, otherwise return 0
   */
  int extra_seconds_in_day() const noexcept {
    int extra;
    dat(_mjd, extra);
    return extra;
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
    taisec = _fsec + (dat(modified_julian_day(_mjd)) + dtat);
    return _mjd;
  }

  explicit TwoPartDateUTC(int mjd, FDOUBLE secday) noexcept
      : _mjd(mjd), _fsec(secday) {
    normalize();
  }

  /** Add seconds to instance, taking into account leap seconds.
   */
  void add_seconds(FDOUBLE sec) noexcept {
    _fsec += sec;
    this->normalize();
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
        _fsec(to_fractional_seconds<T, FDOUBLE>(d.sec())) {
    this->normalize();
  }

  /** Constructor from a pair of doubles, such that MJD = a + b */
  explicit TwoPartDateUTC(int b = 0,
                          FractionalSeconds s = FractionalSeconds{0}) noexcept
      : _mjd(b), _fsec(s.seconds()) {
    if (b != 0 &&
        s.seconds() != 0e0) /* do not normalize for default constructor! */
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
    return seconds() * T::template sec_factor<FDOUBLE>();
  }

  /** @brief Transform the (integral part of the) date to Year Month Day */
  ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

  /** Add seconds to instance, taking into account leap seconds.
   */
  void add_seconds(FractionalSeconds fsec) noexcept {
    this->add_seconds(fsec.seconds());
  }

  /** Add seconds to instance and return the "Kahan summation" error.
   *
   * This function implements a "Kahan summation" scheme to iteratively add
   * seconds (to the instance). Especially in the case of adding multiple
   * times, small multiples of second (i.e. nanoseconds and/or microseconds),
   * it gives better precision than the simple add_seconds method.
   * Example usage:
   * d = TwoPartDateUTC(...);
   * double err = 0;
   * for (long i = 0; i < 1'000'000'000; i++) {
   *    d.add_seconds(1e-9,err);
   * }
   * This version will give better results that using
   * d = TwoPartDateUTC(...);
   * double err = 0;
   * for (long i = 0; i < 1'000'000'000; i++) {
   *    d.add_seconds(1e-9);
   * }
   * For accuracy/precision results, see test/precision/tpadd.cpp
   *
   * @param[in] sec  Floating-point seconds to add to instance
   * @param[out] err Previous summation error at input; updated at output to
   *                 be used at next iteration. If this is the first call,
   *                 set err to 0e0.
   */
  void add_seconds(FDOUBLE sec, FDOUBLE &err) noexcept {
    FDOUBLE a = _fsec;
    FDOUBLE b = sec;
    FDOUBLE y = b - err;
    b = a + y;
    err = (b - a) - y;
    _fsec = b;
    this->normalize();
  }

  /** @brief Differencce betweeen two UTC dates
   *
   * The difference is returned in two parts,
   * 1. An integral part, holding whole days of 86400[sec], and
   * 2. remaining seconds
   * This function will take into consideration the leap seconds between the
   * two days.
   * Note that the whole days returned are days of 86400[sec] aka **NOT**
   * always UTC days
   * Example:
   * a leap insertion date
   * d1 = TwoPartDateUTC(ymd_date{year(2016), month(12), day_of_month(31))
   * d2 = TwoPartDateUTC(ymd_date{year(2017), month(1), day_of_month(1))
   * auto diff = d1 - d2
   * now: diff.imjd() == 1 and diff.seconds() == 1
   */
  TwoPartDate operator-(const TwoPartDateUTC &d) const noexcept;

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
    while (_fsec >= 86400e0 + extra_sec_in_day) {
      _fsec -= (86400e0 + extra_sec_in_day);
      ++_mjd;
      dat(_mjd, extra_sec_in_day);
    }
#ifdef DEBUG
    if (_mjd)
      assert(_fsec >= 0e0 && _fsec < 86400e0 + extra_sec_in_day);
    else
      assert(_fsec < 0e0 && _fsec > -86400e0);
#endif
    /* all done */
    return;
  }

  /** @brief Transform a UTC date to a TAI date */
  TwoPartDate utc2tai() const noexcept;

  /** @brief Transform a UTC date to a TT date */
  TwoPartDate utc2tt() const noexcept;

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
  int _mjd;      /** Mjd */
  FDOUBLE _fsec; /** fractional seconds of day */

  /** Construct from MJD and fractional seconds **of day**.
   *
   * A constexpr constructor that will not check arguments, and will NOT
   * normalize the date. Be very carefull with this one!
   */
  constexpr explicit TwoPartDate(int mjd, FDOUBLE secday,
                                 [[maybe_unused]] char c) noexcept
      : _mjd(mjd), _fsec(secday) {}

  /** Construct from MJD and fractional seconds.
   *
   * This is only private and should be used in rare cases. Normal users,
   * should explicitely cast the second argument to FractionalSeconds to
   * avoid misconceptions (i.e. is the parameters fractional seconds or
   * fractional days?).
   */
  explicit TwoPartDate(int mjd, FDOUBLE secday) noexcept
      : _mjd(mjd), _fsec(secday) {
    normalize();
  }

  /** Add seconds to instance.
   * @warning Does not take into account leap seconds.
   */
  void add_seconds(FDOUBLE sec) noexcept {
    _fsec += sec;
    this->normalize();
  }

public:
  /** Constructor from datetime<T>
   * Note that we are not (explicitly) normalizing the instance here, because
   * the parameter \p d is already considered to hold a 'normalized' datetime.
   * */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr TwoPartDate(const datetime<T> &d) noexcept
      : _mjd(d.imjd().as_underlying_type()),
        _fsec(to_fractional_seconds<T, FDOUBLE>(d.sec())) {
  }

  /** Reference epoch (J2000.0), as a Modified Julian Date. */
  static constexpr TwoPartDate j2000_mjd() noexcept {
    return TwoPartDate(51544, 86400e0 / 2e0, 'y');
  }

  /** Random Date within some MJD limits
   * @todo transfer this into a .cpp file
   */
  static TwoPartDate
  random(modified_julian_day from = modified_julian_day::min(),
         modified_julian_day to = modified_julian_day::max()) noexcept {
    int istart = (int)from.as_underlying_type();
    int istop = (int)to.as_underlying_type();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(istart, istop);
    std::uniform_real_distribution<double> unif(0, 86400e0);
    return TwoPartDate(distr(gen), unif(gen), 'y');
  }

  /** Min date */
  static constexpr TwoPartDate min() noexcept {
    return TwoPartDate(datetime<nanoseconds>::min());
  }

  /** Max date */
  static constexpr TwoPartDate max() noexcept {
    return TwoPartDate(datetime<nanoseconds>::max());
  }

  /** Constructor from a pair of doubles, such that TODO */
  explicit TwoPartDate(int b = 0,
                       FractionalSeconds s = FractionalSeconds{0}) noexcept
      : _mjd(b), _fsec(s.seconds()) {
    this->normalize();
  }

  constexpr explicit TwoPartDate(modified_julian_day mjd) noexcept
      : _mjd(mjd.as_underlying_type()), _fsec(0) {};

  explicit TwoPartDate(year y, month m, day_of_month d, double sec_of_day = 0e0)
      : _mjd(modified_julian_day(y, m, d).as_underlying_type()),
        _fsec(sec_of_day) {
    this->normalize();
  }

  explicit TwoPartDate(year y, day_of_year d, double sec_of_day = 0e0)
      : _mjd(modified_julian_day(y, d).as_underlying_type()),
        _fsec(sec_of_day) {
    this->normalize();
  }

  /** Get the MJD as an intgral number, i.e. no fractional part */
  int imjd() const noexcept { return _mjd; }

  /** Get the (fractional) seconds of the MJD */
  FDOUBLE seconds() const noexcept { return _fsec; }

  /** Get the seconds of the MJD as fractional day. Always in range [0,1) */
  FDOUBLE fractional_days() const noexcept { return _fsec / 86400e0; }

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
    return seconds() * T::template sec_factor<FDOUBLE>();
  }

  /** @brief Transform the (integral part of the) date to Year Month Day */
  ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

  /** @brief Transform the (integral part of the) date to Year Day-Of-Year */
  ydoy_date to_ydoy() const noexcept {
    const modified_julian_day mjd(_mjd);
    return mjd.to_ydoy();
    // return modified_julian_day(_mjd).to_ydoy();
  }

  /** Add seconds to instance.
   * @warning Does not take into account leap seconds.
   */
  void add_seconds(FractionalSeconds fsec) noexcept {
    this->add_seconds(fsec.seconds());
  }

  /** Add seconds to instance and return the "Kahan summation" error.
   *
   * This function implements a "Kahan summation" scheme to iteratively add
   * seconds (to the instance). Especially in the case of adding multiple
   * times, small multiples of second (i.e. nanoseconds and/or microseconds),
   * it gives better precision than the simple add_seconds method.
   * Example usage:
   * d = TwoPartDate(...);
   * double err = 0;
   * for (long i = 0; i < 1'000'000'000; i++) {
   *    d.add_seconds(1e-9,err);
   * }
   * This version will give better results that using
   * d = TwoPartDate(...);
   * double err = 0;
   * for (long i = 0; i < 1'000'000'000; i++) {
   *    d.add_seconds(1e-9);
   * }
   * For accuracy/precision results, see test/precision/tpadd.cpp
   *
   * @param[in] sec  Floating-point seconds to add to instance
   * @param[out] err Previous summation error at input; updated at output to
   *                 be used at next iteration. If this is the first call,
   *                 set err to 0e0.
   */
  void add_seconds(FDOUBLE sec, FDOUBLE &err) noexcept {
    FDOUBLE a = _fsec;
    FDOUBLE b = sec;
    FDOUBLE y = b - err;
    b = a + y;
    err = (b - a) - y;
    _fsec = b;
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

  /** Remove integral days */
  TwoPartDate operator-(const modified_julian_day days) const noexcept {
    return TwoPartDate(_mjd - days.as_underlying_type(), _fsec);
  }

  /** Add integral days */
  TwoPartDate operator+(const modified_julian_day days) const noexcept {
    return TwoPartDate(_mjd + days.as_underlying_type(), _fsec);
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
    } else if constexpr (DT == DateTimeDifferenceType::FractionalSeconds) {
      /* difference as fractional seconds */
      return (_fsec - d._fsec) + (_mjd - d._mjd) * SEC_PER_DAY;
    } else {
      /* difference as fractional (julian) years */
      return this->diff<DateTimeDifferenceType::FractionalDays>(d) /
             DAYS_IN_JULIAN_YEAR;
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

  /** @brief Transform an instance to GPS Time assuming it is in TAI
   *
   * The two time scales are connected by the formula:
   * \f$ TAI = GPSTime + 19 [sec] \f$
   */
  TwoPartDate tai2gps() const noexcept {
    return TwoPartDate(_mjd, _fsec - 19e0);
  }

  TwoPartDate gps2tai() const noexcept {
    return TwoPartDate(_mjd, _fsec + 19e0);
  }

  /**  Transform an instance to UTC assuming it is in TAI */
  /* taisec = _fsec + dat(modified_julian_day(_mjd)); */
  TwoPartDateUTC tai2utc() const noexcept {
    FDOUBLE utcsec = _fsec - dat(modified_julian_day(_mjd));
    int utcmjd = _mjd;
    /* in case we are actually on the previous day, check for leap insertion
     * day and 'normalize' integral and fractional parts
     */
    if (utcsec < 0e0) {
      --utcmjd;
      int extrasec;
      FDOUBLE secinday = SEC_PER_DAY + dat(utcmjd, extrasec);
      utcsec = secinday + utcsec;
    }
    return TwoPartDateUTC(utcmjd, FractionalSeconds{utcsec});
  }

  /**  Transform an instance to UTC assuming it is in TT */
  TwoPartDateUTC tt2utc() const noexcept {
    const auto tai = this->tai2tt();
    return tai.tai2utc();
  }

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
  TwoPartDate tt2ut1(FDOUBLE dut1) const noexcept {
    /* note that ΔUT1 = UT1 − UTC hence UT1 = ΔUT1 + UTC
     * UT1 = TT - 32.184[sec] - ΔAT + ΔUT1
     *     = TAI - ΔAT + ΔUT1
     */
    const auto utc = this->tt2utc();
    TwoPartDate ut1(utc.imjd(), utc.seconds());
    ut1.add_seconds(dut1);
    return ut1;
  }

  TwoPartDate tai2ut1(FDOUBLE dut1) const noexcept {
    /* UT1 = TAI + ΔUT1 - ΔAT
     *     = UTC + ΔUT1
     */
    const auto utc = this->tai2utc();
    TwoPartDate ut1(utc.imjd(), utc.seconds());
    ut1.add_seconds(dut1);
    return ut1;
  }

  FDOUBLE as_mjd() const noexcept { return _fsec / SEC_PER_DAY + _mjd; }

  FDOUBLE jcenturies_sinceJ2000() const noexcept {
    return ((static_cast<FDOUBLE>(_mjd) - J2000_MJD) + _fsec / SEC_PER_DAY) /
           DAYS_IN_JULIAN_CENT;
  }

  /** @brief Convert to Julian Epoch, assuming the TT time-scale. */
  FDOUBLE epj() const noexcept {
    return core::mjd2epj((double)imjd(), seconds() / SEC_PER_DAY);
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
    if (_mjd) {
      assert(_fsec >= 0e0 && _fsec < 86400e0);
    }
    if (_fsec < 0e0) {
      assert(_mjd == 0);
      assert(_fsec > -86400e0);
    } else {
      assert(_fsec >= 0e0 && _fsec < 86400e0);
    }
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

/** @brief Julian Epoch to two-part Modified Julian Date (TT).
 *
 * The function assume the TT time-scale.
 *
 * @param[in] epj The Julian Epoch to convert
 * @return The corresponding MJD as a TwoPartDate
 */
inline TwoPartDate epj2tpd(double epj) noexcept {
  double fday;
  const double mjd = core::epj2mjd(epj, fday);
  return TwoPartDate(mjd, FractionalSeconds{fday * SEC_PER_DAY});
}

/** Cast a TwoPartDate instance to an instance of type datetime<T>
 * TODO needs testing!
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline datetime<T> from_mjdepoch(const TwoPartDate &t) noexcept {
  dso::nanoseconds nsec(static_cast<nanoseconds::underlying_type>(
      t.seconds() * nanoseconds::sec_factor<double>()));
  return datetime<T>(t.imjd(), cast_to<nanoseconds, T>(nsec));
}

} /* namespace dso */

#endif
