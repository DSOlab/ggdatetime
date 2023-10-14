#ifndef __DSO_DATETIME_TWOPARTDATES_HPP__
#define __DSO_DATETIME_TWOPARTDATES_HPP__

#include "dtcalendar.hpp"

namespace dso {

class TwoPartDate {
private:
  double _mjd;  /** Mjd */
  double _fday; /** fractional days */

public:
  /** Constructor from datetime<T> */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  TwoPartDate(const datetime<T> &d) noexcept
      : _mjd((double)d.imjd().as_underlying_type()),
        _fday(d.sec().fractional_days()) {
    this->normalize();
  }

  /** Constructor from a pair of doubles, such that MJD = a + b */
  explicit constexpr TwoPartDate(double b = 0, double s = 0) noexcept
      : _mjd(b), _fday(s) {
    this->normalize();
  }

  /** Get the MJD as an intgral number, i.e. no fractional part */
  double imjd() const noexcept { return _mjd; }

  /** Get the fractional part of the MJD*/
  double fday() const noexcept { return _fday; }

  /** Add seconds to instance.
   * @warning Does not take into account leap seconds.
   */
  void add_seconds(double sec) noexcept {
    _fday += sec / SEC_PER_DAY;
    this->normalize();
  }

  /** Difference between two dates as integral number of days and seconds of
   * day
   *
   * This is not an 'actual date' but rather a datetime interval, but can be
   * represented a TwoPartDate instance. If the calling instance is prior to
   * the operand (i.e. d1-d2 with d2>d1) the interval is signed as 'negative'.
   * This means that the number of days can be negative, but the fractional
   * day will always be positive
   */
  TwoPartDate operator-(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_mjd - d._mjd, _fday - d._fday);
  }

  /** Add two instances.
   *
   * In this case, the second instance can be though of a datetime interval
   * (rather than an actual datetime instance). The right operand can be a
   * negative interval, which means that we are going backwards in time.
   */
  TwoPartDate operator+(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_mjd + d._mjd, _fday + d._fday);
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
  double diff(const TwoPartDate &d) const noexcept {
    if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
      /* difference as fractional days */
      return (_mjd - d._mjd) + (_fday - d._fday);
    } else {
      /* difference as fractional seconds */
      return (_mjd - d._mjd) * SEC_PER_DAY + (_fday - d._fday) * SEC_PER_DAY;
    }
  }

  /** Get the date as (fractional) Julian Date */
  double julian_date() const noexcept { return _fday + (_mjd + dso::MJD0_JD); }

  /** Transform instance to TT, assuming it is in TAI
   *
   * The two time scales are connected by the formula:
   * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
   */
  TwoPartDate tai2tt() const noexcept {
    constexpr const double dtat = TT_MINUS_TAI / SEC_PER_DAY;
    return TwoPartDate(_mjd, _fday + dtat);
  }

  /** Transform an instance to TAI, assuming it is in UTC
   *
   * The two time scales are connected by the formula:
   * \f$ UTC = TAI + ΔAT \$ where ΔAT are the leap seconds.
   */
  TwoPartDate utc2tai() const noexcept {
    auto utc = *this;
    /* Get TAI-UTC at 0h today and extra seconds in day (if any) */
    int extra;
    const int leap = dat(modified_julian_day((int)utc._mjd), extra);
    /* Remove any scaling applied to spread leap into preceding day */
    utc._fday *= (SEC_PER_DAY + extra) / SEC_PER_DAY;
    /* Assemble the TAI result, preserving the UTC split and order */
    return TwoPartDate(utc._mjd, utc._fday + leap / SEC_PER_DAY);
  }

  /** Transform an instance to TT assuming it is in UTC */
  TwoPartDate utc2tt() const noexcept {
    const auto tai = this->utc2tai();
    return tai.tai2tt();
  }

  /**  Transform an instance to UTC assuming it is in TAI */
  TwoPartDate tai2utc() const noexcept {
    // do it the SOFA way ...
    auto utc1(*this);
    double small = utc1._fday;
    double big = utc1._mjd;
    for (int i = 0; i < 3; i++) {
      TwoPartDate guess = TwoPartDate(big, small).utc2tai();
      small += utc1._mjd - guess._mjd;
      small += utc1._fday - guess._fday;
    }
    return TwoPartDate(utc1._mjd, small);
  }

  /**  Transform an instance to UTC assuming it is in TT */
  TwoPartDate tt2utc() const noexcept {
    const auto tai = this->tt2tai();
    return tai.tai2utc();
  }

  /**  Transform an instance to TAI assuming it is in TT */
  TwoPartDate tt2tai() const noexcept {
    constexpr const double dtat = TT_MINUS_TAI / SEC_PER_DAY;
    return TwoPartDate(_mjd, _fday - dtat);
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
  TwoPartDate tt2ut1(double dut1) const noexcept {
    /* note that ΔUT1 = UT1 − UTC hence UT1 = ΔUT1 + UTC */
    return tt2utc() + TwoPartDate(0e0, dut1 / SEC_PER_DAY);
  }

  double as_mjd() const noexcept { return _fday + _mjd; }

  double jcenturies_sinceJ2000() const noexcept {
    return (_mjd - J2000_MJD) / DAYS_IN_JULIAN_CENT +
           _fday / DAYS_IN_JULIAN_CENT;
  }

  bool operator>(const TwoPartDate &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fday > d._fday));
  }
  bool operator>=(const TwoPartDate &d) const noexcept {
    return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fday >= d._fday));
  }
  bool operator<(const TwoPartDate &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fday < d._fday));
  }
  bool operator<=(const TwoPartDate &d) const noexcept {
    return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fday <= d._fday));
  }

  /// @brief Keep _fday < 1e0 and _mjd integral only
  constexpr void normalize() noexcept {
    // fractional part should NOT be negative
    while (_fday < 0e0) {
      _fday = 1 - _fday;
      _mjd -= 1e0;
    }
    double fmore(0e0), extra(0e0);
    // check if _mjd part has a fractional part
    if ((fmore = std::modf(_mjd, &extra)) != 0e0) {
      // assign fractional part to _fday and keep integral part to _mjd
      _fday += fmore;
      _mjd = extra;
    }
    // check if fractional part is >= 1e0
    if (_fday >= 1e0) {
      _fday = std::modf(_fday, &extra);
      _mjd += extra;
    }
    // all done
    return;
  }

  TwoPartDate normalized() const noexcept {
    TwoPartDate d(*this);
    d.normalize();
    return d;
  }

  bool operator==(const TwoPartDate &d) const noexcept {
    const auto d1(this->normalized());
    const auto d2(d.normalized());
    return (d1._mjd == d2._mjd) && (d1._fday == d2._fday);
  }
  bool operator!=(const TwoPartDate &d) const noexcept {
    return !(this->operator==(d));
  }

  /// A Julian Date can be partioned in any of the following ways:
  /// -----------------------------------------------------------
  ///            dj1            dj2
  ///
  ///        2450123.7           0.0       (JD method)
  ///        2451545.0       -1421.3       (J2000 method)
  ///        2400000.5       50123.2       (MJD method)
  ///        2450123.5           0.2       (date & time method)
  enum class JdSplitMethod { JD, J2000, MJD, DT };
  template <JdSplitMethod S = JdSplitMethod::J2000>
  TwoPartDate jd_split() const noexcept {
    if constexpr (S == JdSplitMethod::JD)
      return TwoPartDate(_mjd + dso::MJD0_JD + _fday, 0e0);
    else if constexpr (S == JdSplitMethod::J2000)
      return TwoPartDate(J2000_JD, _mjd - J2000_MJD + _fday);
    else if constexpr (S == JdSplitMethod::MJD)
      return TwoPartDate(MJD0_JD, _mjd + _fday);
    else
      return TwoPartDate(_mjd + dso::MJD0_JD, _fday);
  }
}; /* class TwoPartDate */

TwoPartDate utc2tai(const TwoPartDate &d) noexcept;

} /* namespace dso */

#endif
