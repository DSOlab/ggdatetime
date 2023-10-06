/** @file
 * A fundamental, template datetime class. This file contains the definition 
 * (and implementation) of a datetime class to be used for Space Geodesy 
 * applications.
 */

#ifndef __DTCALENDAR_NGPT__HPP__
#define __DTCALENDAR_NGPT__HPP__

#include "dtfund.hpp"
#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace dso {

namespace core {
/** Return +1 or -1 depending on the sign of the input (arithmetic) parameter
 *
 * @param[in] value Any arithmetic value
 * @return +1 if the passed in value is >=0; -1 otherwise
 */
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
int sgn(T val) noexcept {
  return (T(0) <= val) - (val < T(0));
}

  /** A utility struct to hold possibly negative datetime intervals
   *
   * This struct is meant to act as an intermediate/buffer state between 
   * a datetime difference and a datetime interval. A difference could be 
   * negative, but an interval can only be positive. Hence, this class only 
   * has the purpose of acting as an intermediate state, so that it can 
   * 'normalize' the behaviour between a difference and an interval.
   */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
struct RawDatetimeDifference {
  typedef typename S::underlying_type SecIntType;
  typedef modified_julian_day::underlying_type DaysIntType;

  /** seconds of day (in interval), always positive */
  SecIntType msecs;
  /** days (in interval),can be negative */
  DaysIntType mdays;

  explicit RawDatetimeDifference(DaysIntType days, SecIntType sec) noexcept
      : mdays(days), msecs(sec) {
    normalize();
  }

  void normalize() noexcept {
    /* number of whole days in seconds (always positive) */
    DaysIntType more = std::copysign(1, msecs) / S::max_in_day();
    /* add to current days, with the right sign */
    mdays += std::copysign(msecs, more);
    /* leftover seconds (positive) */
    SecIntType s = std::copysign(1, msecs) - more * S::max_in_day();
    /* if initial seconds were negative, adjust */
    mdays -= 1 * (msecs < 0);
    msecs = (S::max_in_day() - s) * (msecs < 0) + s * (msecs >= 0);
#ifdef DEBUG
    assert(mdays >= 0 && msecs >= 0);
#endif
  }
}; /* RawDatetimeInterval */

}/* namespace core*/

/** @brief A generic, templatized class to hold a datetime period/interval.
 *
 * A datetime_interval represents a time (datetime) interval or period, i.e.
 * 5 days, 12 hours and 49 seconds. We assume a continuous time scale, no leap
 * seconds are taken into consideration --this is only an interval not an
 * actual datetime instance--).
 * A datetime_interval instance can only have positive (or zero) values (for
 * both its members).
 *
 * A datetime_interval instance has two fundamental parts (members):
 * - a day part (i.e. holding the days), and
 * - a time part (i.e. holding any type S i.e. any second type)
 *
 * The purpose of this class is to work together with the datetime class.
 *
 * @tparam S Any class of 'second type', i.e. any class S that has a (static)
 *           member variable S::is_of_sec_type set to true. This can be
 *           dso::seconds, dso::milliseconds, dso::microseconds.
 *
 *
 * @note  - Any instance of the class has two members, m_days an integer
 *          representing the (MJ) days and m_secs, an instance of type S,
 *          representing the fractional day part. However, the parts may be
 *          mixed (!), that is if e.g. S is dso::seconds, the m_secs part may
 *          actualy have any value, including ones larger than 86400. That is
 *          to say that an instance can be constructed as:
 *        \code{.cpp}
 *          datetime_interval<seconds> d {modified_julian_day(1),
 *          seconds(90000)};
 *        \endcode
 *          Should the user want the "normalization" of the instance (that is
 *          remove whole days from the m_secs part and add them to the m_days
 *          part), call the function datetime_interval::normalize().
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class datetime_interval {
  typedef typename S::underlying_type SecIntType;
  typedef modified_julian_day::underlying_type DaysIntType;

public:
  /** Default constructor (everything set to 0). */
  explicit constexpr datetime_interval() noexcept
      : m_days(0), m_secs(0), m_sign(1){};

  void normalize() noexcept {
    /* number of whole days in seconds (always positive) */
    const DaysIntType more = std::copysign(m_secs, 1) / S::max_in_day();
    /* add to current days, with the right sign */
    const DaysIntType days =
        std::copysign(m_days, m_sign) + std::copysign(more, m_secs);
    /* leftover seconds (positive) */
    const SecIntType s = std::copysign(m_secs, 1) - more * S::max_in_day();
    /* if initial seconds were negative, adjust */
    const DaysIntType mdays = days - 1 * (m_secs < 0);
    m_secs = (S::max_in_day() - s) * (m_secs < 0) + s * (m_secs >= 0);
    m_sign = core::sgn(mdays);
    m_days = std::copysign(mdays, 1);
#ifdef DEBUG
    assert(mdays >= 0 && msecs >= 0);
#endif
  }

  /*
  /// Operator >
  constexpr bool operator>(const datetime_interval &d) const noexcept {
    return m_days > d.m_days || (m_days == d.m_days && m_secs > d.m_secs);
  }

  /// Operator >=
  constexpr bool operator>=(const datetime_interval &d) const noexcept {
    return m_days > d.m_days || (m_days == d.m_days && m_secs >= d.m_secs);
  }

  /// Operator <
  constexpr bool operator<(const datetime_interval &d) const noexcept {
    return m_days < d.m_days || (m_days == d.m_days && m_secs < d.m_secs);
  }

  /// Operator <=
  constexpr bool operator<=(const datetime_interval &d) const noexcept {
    return m_days < d.m_days || (m_days == d.m_days && m_secs <= d.m_secs);
  }

  /// Operator ==
  constexpr bool operator==(const datetime_interval &d) const noexcept {
    return (m_days == d.m_days) && (m_secs == d.m_secs);
  }

  /// Operator !=
  constexpr bool operator!=(const datetime_interval &d) const noexcept {
    return !(this->operator==(d));
  }

  /// @todo wtf is this?? it needs checking
  datetime_interval operator/(int div) const noexcept {
    double d = static_cast<double>(m_days.as_underlying_type());
    double i, f;
    f = std::modf(d / div, &i);
    auto new_mjd = static_cast<modified_julian_day::underlying_type>(i);

    d = static_cast<double>(m_secs.as_underlying_type());
    d = d / div + f * S::max_in_day;
    auto new_sec = static_cast<typename S::underlying_type>(d);

    return datetime_interval{modified_julian_day{new_mjd}, S{new_sec}};
  }
  */

  /** return number of days in interval, always positive */
  DaysIntType days() const noexcept {return m_days;}

  /** return number of *secs in interval, always positive */
  SecIntType sec() const noexcept {return m_secs;}

  /** return the sign of the interval */
  int sign() const noexcept {return m_sign;}
  
  SecIntType unsigned_total_sec() const noexcept {
    return m_secs + S::max_in_day() * m_days;
  }

  SecIntType signed_total_sec() const noexcept {
    return std::copysign(unsigned_total_sec(), m_sign);
  }

private:
  /** number of whole days in interval */
  DaysIntType m_days;
  /** number of *sec in interval */ 
  SecIntType m_secs; 
  /** sign of interval (only care for the sign of m_sign) */
  int m_sign; 
}; /* end class datetime_interval */

/// @brief A generic, templatized Date/Time class.
///
/// A datetime instance has two fundamental parts (members):
/// - a date part (i.e. holding the year/month/day), and
/// - a time part (i.e. holding hours/minutes/*seconds)
///
/// A datetime holds both a date and a time (i.e. of day). The date is recorded
/// as a Modified Julian Day (i.e. dso::modified_julian_day). The time of day
/// can be expressed via any class of 'second type', i.e. dso::seconds,
/// dso::milliseconds, or dso::microseconds). Actually, there is no
/// restriction for the time to be 'time of day'; in-fact, it can hold even days
/// (e.g. 2 or 3 days in dso::seconds), but usually this is not good practice
/// and should be avoided. Especially when the second type is e.g.
/// dso::milliseconds, then holding multiple days in the time part could result
/// in overflow. Additionaly, some member functions (e.g. the equality
/// operators), expect the datetime to be split as date and time (of day), so
/// make sure you conform to this. To remove whole days from the time part and
/// add them to the date part, use the datetime::normalize method.
///
/// Never use negative times; they actually have no physical meaning. Besides
/// that, they can cause UB.
///
/// The datetime class represents a continuous time-scale (i.e. something like
/// TAI); thus, representing non-continuous time-scales (e.g. UTC) can be
/// ambiguous cause of leap seconds. (Modified) Julian Day cannot unambiguously
/// represent UTC during a leap second unless special measures are taken.  The
/// internal convention is that he quasi-JD day represents UTC days whether the
/// length is 86399, 86400 or 86401 SI seconds.
///
/// @tparam S Any class of 'second type', i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true. This can be
///           dso::seconds, dso::milliseconds, dso::microseconds.
///
/// @note Constructors can be called with the time part being more than one day;
///       (e.g.
///       \code{.cpp}
///         datetime<dso::seconds> d {year(2016), month(12), day_of_month(15),
///         seconds(86401};)
///       \endcode
///       If you think that this is a posibility, then call datetime::normalize
///       imidiately after the construction. An automatic call of the function
///       datetime::normalize could be added in the body of (nearly all)
///       constructors, but that would be time consuming. So, the option chosen
///       is **not**  to include a call to datetime::normalize and let the user
///       call the function if needed.
///
/// @bug All operator expect normalized dates (i.e. the fraction_of_day part
/// should
///      not be over 1 day). We have to make sure that this is always the case.
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class datetime {
public:
  /** Expose the underlying sec type S */
  using SecIntType = S;
  /** Expose the underlying modified_julian_day int */
  typedef modified_julian_day::underlying_type DaysIntType;

  /** Maximum possible date (seconds are 0, modified_julian_day is max
   * possible.
   */
  constexpr static datetime max() noexcept {
    return datetime(modified_julian_day::max(), S(0));
  }

  /** Minimum possible date (seconds are 0, modified_julian_day is min
    * possible.
    */
  constexpr static datetime min() noexcept {
    return datetime(modified_julian_day::min(), S(0));
  }

  /** Default constructor. */
  explicit constexpr datetime() noexcept : m_mjd(dso::J2000_MJD), m_sec(0){};

  /** Constructor from year, month, day of month and sec type.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, S s)
      : m_mjd(y, m, d), m_sec(s) {
    this->normalize();
  };

  /** Constructor from year, month, day of month and any sec type T 
   *  If an invalid date is passed-in, the constructor will throw.
   *
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>>
  constexpr datetime(year y, month m, day_of_month d, T t)
      : m_mjd(y, m, d), m_sec(S(t)) {
    this->normalize();
  }
  */

  /** Constructor from year, day of year and any sec type T.
   *  If an invalid date is passed-in, the constructor will throw.
   *
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>>
  constexpr datetime(year y, day_of_year d, T t) : m_mjd(y, d), m_sec(S(t)) {
    this->normalize();
  }
  */

  /** Constructor from year, month, day of month, hours, minutes and
   *  any second type T convertible to S (i.e. T can be cast to S).
   *  If an invalid date is passed-in, the constructor will throw.
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>>
  constexpr datetime(year y, month m, day_of_month d, hours hr, minutes mn,
                     T sec)
      : m_mjd(y, m, d), m_sec(hr, mn, S(sec)) {
    this->normalize();
  }
  */

  /** Constructor from year, month, day of month and fractional seconds.
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, hours hr, minutes mn,
                     double fsecs)
      : m_mjd(y, m, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** Constructor from year, day of year and fractional seconds.
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, day_of_year d, hours hr, minutes mn, double fsecs)
      : m_mjd(y, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** Constructor from year, month, day of month, hours, minutes and second 
   * type S.
   *
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, hours hr = hours(),
                     minutes mn = minutes(), S sec = S())
      : m_mjd(y, m, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** Constructor from year, day of year, hours, minutes and second type S.
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, day_of_year d, hours hr = hours(),
                     minutes mn = minutes(), S sec = S())
      : m_mjd(y, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** Constructor from modified julian day, hours, minutes and second type S.
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(modified_julian_day mjd, hours hr = hours(),
                     minutes mn = minutes(), S sec = S()) noexcept
      : m_mjd(mjd), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** Constructor from modified julian day, and second type S. */
  constexpr datetime(modified_julian_day mjd, S sec = S()) noexcept
      : m_mjd(mjd), m_sec(sec) {
    this->normalize();
  }

  /** Constructor from GPS Week and Seconds of Week */
  constexpr datetime(gps_week w, S sow) noexcept
      : m_mjd(w.as_underlying_type() * 7 +
              sow.as_underlying_type() / S::max_in_day + JAN61980),
        m_sec(sow) {
    m_sec.remove_days();
  }

  /** Get the Modified Julian Day (const). */
  constexpr modified_julian_day imjd() const noexcept { return m_mjd; }

  /** Get the number of *seconds (as type S) of the instance.
    * @return The number of *seconds (as type S) of the instance.
    */
  constexpr S sec() const noexcept { return m_sec; }

/*
  /// @brief Add any second type T to an instance of type S
  ///
  /// This function will add to the instance a number of seconds (of type T).
  /// This works with tag-dispatch, thus this function is actually only a shell.
  ///
  /// @tparam    T   Any seconds type
  /// @param[in] sec Seconds of T-type
  /// @return nothing
  ///
  /// @warning If the input seconds type is of higher resolution than the
  ///          instance, then loss of accuracy may happen.
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr void add_seconds(T sec) noexcept {
    typedef std::integral_constant<bool, (S::max_in_day > T::max_in_day)> cmp__;
    cmp__ btype{};
    __add_seconds_impl(sec, btype);
  }

  /// @brief Subtract any second type T to an instance of type S
  ///
  /// This function will remove to the instance a number of seconds (of type T).
  /// This works with tag-dispatch, thus this function is actually only a shell.
  ///
  /// @tparam    T   Any seconds type
  /// @param[in] sec Seconds of T-type
  /// @return nothing
  ///
  /// @warning If the input seconds type is of higher resolution than the
  ///          instance, then loss of accuracy may happen.
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr void remove_seconds(T sec) noexcept {
    typedef std::integral_constant<bool, (S::max_in_day > T::max_in_day)> cmp__;
    cmp__ btype{};
    __remove_seconds_impl(sec, btype);
  }
*/

  /** Operator '+' where the right-hand-side is an interval */
  constexpr datetime<S>
  operator+(const datetime_interval<S> &dt) const noexcept {
    const auto mjd =
        m_mjd + modified_julian_day(std::copysign(dt.days(), dt.sign()));
    const auto sec = m_sec + dt.sec();
    return datetime<S>(mjd, sec);
  }
  
  /** Add a time interval to a datetime instance. */
  constexpr void operator+=(const datetime_interval<S> &dt) noexcept {
    m_mjd += modified_julian_day(std::copysign(dt.days(), dt.sign()));
    m_sec += dt.sec();
    this->normalize();
  }
  
  /** Operator '-' between two instances, produces a (signed) interval */
  constexpr datetime_interval<S>
  operator-(const datetime<S> &dt) const noexcept {
    return datetime_interval<S>(m_mjd - dt.m_mjd, m_sec - dt.m_sec);
  }

  /** Cast to any datetime<T> instance, regardless of what T is
   * 
   * @tparam T    A 'second type'
   * @return The calling object as an instance of type datetime<T>
   */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr datetime<T> cast_to() const noexcept {
    return datetime<T>(m_mjd, dso::cast_to<S, T>(m_sec));
  }

  /** Overload equality operator. */
  constexpr bool operator==(const datetime &d) const noexcept {
    return m_mjd == d.m_mjd && m_sec == d.m_sec;
  }

  /** Overload in-equality operator. */
  constexpr bool operator!=(const datetime &d) const noexcept {
    return !(this->operator==(d));
  }

  /** Overload ">" operator. */
  constexpr bool operator>(const datetime &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec);
  }

  /** Overload ">=" operator. */
  constexpr bool operator>=(const datetime &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec);
  }

  /** Overload "<" operator. */
  constexpr bool operator<(const datetime &d) const noexcept {
    return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec);
  }

  /** Overload "<=" operator. */
  constexpr bool operator<=(const datetime &d) const noexcept {
    return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec <= d.m_sec);
  }

  /** @brief Normalize a datetime instance.
   * 
   * Split the date and time parts such that the time part is always less
   * than one day (i.e. make it time-of-day) and positive (i.e.>=0).
   * Remove whole days of from the time part and add them to the date part.
   */
  constexpr void normalize() noexcept {
    /* number of whole days in seconds (always positive) */
    const DaysIntType more = std::copysign(m_sec, 1) / S::max_in_day();
    /* add to current days, with the right sign */
    const DaysIntType days =
        m_mjd.as_underlying_type() + std::copysign(more, sgn(m_sec));
    /* leftover seconds (positive) */
    const SecIntType s =
        std::copysign(m_sec.as_underlying_type(), 1) - more * S::max_in_day();
    /* if initial seconds were negative, adjust */
    m_mjd = days - 1 * (m_sec < 0);
    m_sec = (S::max_in_day() - s) * (m_sec < 0) + s * (m_sec >= 0);
#ifdef DEBUG
    assert(m_sec >= S(0) && m_sec <);
#endif
  }

  /** Difference (interval) between two datetimes. d1.delta_date(d2) is d1-d2 */
  constexpr datetime_interval<S> delta_date(const datetime &d) const noexcept {
    return datetime_interval<S>((m_mjd - d.m_mjd).as_underlying_type(),
                                (m_sec - d.m_sec).as_underlying_type());
  }

  /** @brief Cast to double (i.e. fractional) Modified Julian Date. */
  constexpr double fmjd() const noexcept {
    return static_cast<double>(m_mjd.as_underlying_type()) +
           m_sec.fractional_days();
  }

  /** @brief Cast to double (i.e. fractional) Julian Date. */
  constexpr double as_jd() const noexcept {
    const double jd = m_mjd.to_julian_day();
    return m_sec.fractional_days()+jd;
  }

  /** @brief compute Julian centuries since J2000 */
  constexpr double jcenturies_sinceJ2000() const noexcept {
    const double jd = m_mjd.to_julian_day();
    const double jc =
        (jd - J2000_JD) / 36525e0 + m_sec.fractional_days() / 36525e0;
    return jc;
  }

  /** @brief Cast to year, month, day of month */
  constexpr ymd_date as_ymd() const noexcept { return m_mjd.to_ymd(); }

  /** @brief Cast to year, day_of_year */
  constexpr ydoy_date as_ydoy() const noexcept { return m_mjd.to_ydoy(); }

  /** @brief Convert to Julian Epoch */
  constexpr double as_julian_epoch() const noexcept {
    return epj(this->fmjd());
  }

  /** @brief Convert to fractional years. 
   * The function will assuming a year of of 365.25 days, i.e. a Julian year
   */
  constexpr double fractional_jyears() const noexcept {
    const ydoy_date ydoy(as_ydoy());
    const double year = ydoy.__year.as_underlying_type();
    const double doy = ydoy.__doy.as_underlying_type();
    const double fday = sec().fractional_days();
    return year + (doy / DAYS_IN_JULIAN_YEAR + fday / DAYS_IN_JULIAN_YEAR);
  }
  
  /** @brief Convert to fractional years. 
   * The function will assuming a year of of 365 or 366 days, depending on the 
   * year (i.e. if it is leap or not).
   */
  constexpr double fractional_cyears() const noexcept {
    const ydoy_date ydoy(as_ydoy());
    const double year = ydoy.__year.as_underlying_type();
    const double doy = ydoy.__doy.as_underlying_type();
    const double fday = sec().fractional_days();
    const double days_in_year = 365e0 + (double)ydoy.__year.is_leap();
    return year + (doy / days_in_year + fday / days_in_year);
  }

  /** @brief Cast to gps_week and S-Of-Week */
  constexpr gps_week gps_wsow(S &sow) const noexcept {
    const DaysIntType mjd = m_mjd.as_underlying_type();
    gps_week w((mjd - JAN61980) / 7);
    sow = S(((mjd - JAN61980) - w.as_underlying_type() * 7) * S::max_in_day);
    sow += m_sec;
    return w;
  }

private:
  /// @brief Add any second type T where S is of higher resolution than T
  ///
  /// This is the implementation for adding any type of seconds (T), where T is
  /// of lower resolution than S, to the instanece. The input seconds sec will
  /// be cast to S-type and then added to the instanece.
  /// This is the implementation function meant to work via tag dispatch, so it
  /// needs a dummy parameter of type std::true_type
  ///
  /// @tparam T Any seconds type where S::max_id_day > T::max_in_day
  /// @param[in] sec Seconds of T-type where S::max_id_day > T::max_in_day
  /// @return nothing
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::true_type) noexcept {
    S ssec = dso::cast_to<T, S>(sec);
    m_sec += ssec;
    this->normalize();
    return;
  }

  /// @brief Add any second type T where T is of higher resolution than S
  ///
  /// This is the implementation for adding any type of seconds (T), where T is
  /// of higher resolution than S, to the instance. The instance will first be
  /// cast into T-type, the input seconds are added to the instance and then
  /// the instance will be re-casted to S-type.
  /// This is the implementation function meant to work via tag dispatch, so it
  /// needs a dummy parameter of type std::false_type
  ///
  /// @tparam T Any seconds type where T::max_id_day > S::max_in_day
  /// @param[in] sec Seconds of T-type where T::max_id_day > S::max_in_day
  /// @return nothing
  ///
  /// @warning The input seconds (parameter) is of higher resolution than the
  ///          instance, thus loss of accuracy may happen.
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::false_type) noexcept {
    T sect = dso::cast_to<S, T>(m_sec);
    sect += sec;
    m_sec = dso::cast_to<T, S>(sect);
    this->normalize();
    return;
  }

  /// @brief Subtract any second type T where S is of higher resolution than T
  ///
  /// This is the implementation for removing any type of seconds (T), where T
  /// is of lower resolution than S, to the instanece. The input seconds sec
  /// will be cast to S-type and then subtracted from the instanece. This is the
  /// implementation function meant to work via tag dispatch, so it needs a
  /// dummy parameter of type std::true_type
  ///
  /// @tparam T Any seconds type where S::max_id_day > T::max_in_day
  /// @param[in] sec Seconds of T-type where S::max_id_day > T::max_in_day
  /// @return nothing
  template <typename T>
  constexpr void __remove_seconds_impl(T sec, std::true_type) noexcept {
    S ssec = dso::cast_to<T, S>(sec);
    m_sec -= ssec;
    this->normalize();
    return;
  }

  /// @brief Subtract any second type T where T is of higher resolution than S
  ///
  /// This is the implementation for removing any type of seconds (T), where T
  /// is of higher resolution than S, to the instance. The instance will first
  /// be cast into T-type, the input seconds are subtracted from the instance
  /// and then the instance will be re-casted to S-type. This is the
  /// implementation function meant to work via tag dispatch, so it needs a
  /// dummy parameter of type std::false_type
  ///
  /// @tparam T Any seconds type where T::max_id_day > S::max_in_day
  /// @param[in] sec Seconds of T-type where T::max_id_day > S::max_in_day
  /// @return nothing
  ///
  /// @warning The input seconds (parameter) is of higher resolution than the
  ///          instance, thus loss of accuracy may happen.
  template <class T>
  constexpr void __remove_seconds_impl(T sec, std::false_type) noexcept {
    T sect = dso::cast_to<S, T>(m_sec);
    sect -= sec;
    m_sec = dso::cast_to<T, S>(sect);
    this->normalize();
    return;
  }

  modified_julian_day m_mjd; /** Modified Julian Day */
  S m_sec;                   /** Time of day in S precision. */

}; /* datetime<S> */

/// Difference between two dates in MJdays and T.
/// Diff is dt1 - dt2
/*#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
constexpr datetime_interval<T> delta_date(const datetime<T> &dt1,
                                          const datetime<T> &dt2) noexcept {
  return dt1.delta_date(dt2);
}*/

/** @brief (Signed) interval between two datetimes in *seconds.
 * Difference of two datetime instances in seconds, when they are of different
 * second type (i.e. datetime<seconds> and datetime<milliseconds>). This
 * function will cast the most imprecise instance to the accuracy of the most
 * precise one and then perform the computation. E.g. if given:
 * a=datetime<seconds>{...} and b=datetime<milliseconds>{...}, then a will
 * be cast to a datetime<milliseconds> instance and then the Δseconds will be
 * computed. The return type, will be that of the most precise type (aka in
 * the case above the difference will be returned in milliseconds).
 * This implementation is used when S1 is more precise than S2.
 *
 * @tparam S1  any second type that has a static member S1::is_of_sec_type set
 *             to True and has an Integral static variable S1::max_in_day
 * @tparam S2  any second type that has a static member S2::is_of_sec_type set
 *             to True and has an Integral static variable S2::max_in_day
 * @param  d1  datetime<S1> instance (difference is d1-d2)
 * @param  d2  datetime<S2> instance (difference is d1-d2)
 * @return     Difference d1-d2 in S1
 */
//template <typename S1, typename S2,
//          typename = std::enable_if_t<S1::is_of_sec_type>,
//          typename = std::enable_if_t<S2::is_of_sec_type>,
//          typename = std::enable_if_t<(S1::max_in_day > S2::max_in_day)>>
//inline S1 delta_sec(const datetime<S1> &d1, const datetime<S2> &d2) noexcept {
//  /* cast d2 from datetime<S2> to datetime<S1> */
//  const datetime<S1> d2s1 = d2. template cast_to<S1>(d2);
//  /* return the interval in (signed) *seconds S1 */
//  return (d1-d2s1).signed_total_sec();
//}

/** @brief (Signed) interval between two datetimes in *seconds.
 *
 * Difference of two datetime instances in seconds, when they are of different
 * second type (aka datetime<seconds> and datetime<milliseconds>). This
 * function will cast the most imprecise instance to the accuracy of the most
 * precise one and then perform the computation. E.g. if given:
 * a=datetime<seconds>{...} and b=datetime<milliseconds>{...}, then a will
 * be cast to a datetime<milliseconds> instance and then the Δseconds will be
 * computed. The return type, will be that of the most precise type (aka in
 * the case above the difference will be returned in milliseconds).
 * This implementation is used when S2 is more precise than S1.
 *
 * @tparam S1  any second type that has a static member S1::is_of_sec_type set
 *             to True and has an Integral static variable S1::max_in_day
 * @tparam S2  any second type that has a static member S2::is_of_sec_type set
 *             to True and has an Integral static variable S2::max_in_day
 * @param  d1  datetime<S1> instance (difference is d1-d2)
 * @param  d2  datetime<S2> instance (difference is d1-d2)
 * @return     Difference d1-d2 in S2
 */
//template <typename S1, typename S2,
//          typename = std::enable_if_t<S1::is_of_sec_type>,
//          typename = std::enable_if_t<S2::is_of_sec_type>,
//          typename = std::enable_if_t<(S2::max_in_day > S1::max_in_day)>>
//inline S2 delta_sec(const datetime<S1> &d1, const datetime<S2> &d2) noexcept {
//  /* cast d1 from datetime<S1> to datetime<S2> */
//  const datetime<S2> d1s2 = d1. template cast_to<S2>(d1);
//  /* return the interval in (signed) *seconds S2 */
//  return (d1s2-d2).signed_total_sec();
//}

template <typename S1, typename S2,
          typename = std::enable_if_t<S1::is_of_sec_type>,
          typename = std::enable_if_t<S2::is_of_sec_type>,
          typename RetSecType = typename std::conditional<
              (S1::max_in_day >= S2::max_in_day), S1, S2>::type>
inline RetSecType delta_sec(const datetime<S1> &d1,
                            const datetime<S2> &d2) noexcept {
  if constexpr (S1::max_in_day > S2::max_in_day) {
    /* cast d2 from datetime<S2> to datetime<S1> */
    const datetime<S1> d2s1 = d2.template cast_to<S1>(d2);
    /* return the interval in (signed) *seconds S1 */
    return (d1 - d2s1).signed_total_sec();
  } else if constexpr (S2::max_in_day > S1::max_in_day) {
    /* cast d1 from datetime<S1> to datetime<S2> */
    const datetime<S2> d1s2 = d1.template cast_to<S2>(d1);
    /* return the interval in (signed) *seconds S2 */
    return (d1s2 - d2).signed_total_sec();
  } else /* S1::max_in_day == S2::max_in_day */ {
    return (d1 - d2).signed_total_sec();
  }
}

/// Because we have a function of type:
/// delta_sec(datetime<S1> d1, datetime<S2> d2) for any S1, S2 of sec type, we
/// should also include an implementation for when S1=S2. In that case, the
/// implementation is just d1.delta_sec(d2) but here we are declaring a
/// non-member function for ease of use.
///
/// @tparam S   any second type that has a static member S1::is_of_sec_type set
///             to True and has an Integral static variable S1::max_in_day
/// @param  d1  datetime<S> instance (deifference is d1-d2)
/// @param  d2  datetime<S> instance (deifference is d1-d2)
/// @return     Difference d1-d2 in S
//#if __cplusplus >= 202002L
//template <gconcepts::is_sec_dt S>
//#else
//template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
//#endif
//inline S delta_sec(const datetime<S> &d1, const datetime<S> &d2) noexcept {
//  return d1.delta_sec(d2);
//}

enum class DateTimeDifferenceType { FractionalDays, FractionalSeconds };

/// @brief Return the difference d1 - d2 in the Date/Time units specified by
///        the template parameter D
template <DateTimeDifferenceType D, typename S,
          typename = std::enable_if_t<S::is_of_sec_type>>
inline double date_diff(const datetime<S> &d1, const datetime<S> &d2) noexcept {
  if constexpr (D == DateTimeDifferenceType::FractionalSeconds) {
    const auto sdiff = delta_sec(d1, d2);
    return sdiff.to_fractional_seconds();
  } else {
    const auto ddiff = d1.delta_date(d2);
    // TODO how to handle many days without overflow ?
    // First effort ... (same as first)
    // return ddiff.as_mjd();
    // Second effort ... (same as first)
    // const double fsec_part2 = ddiff.sec().fractional_days();
    // const double fsec_part1 =
    //    static_cast<double>(ddiff.days().as_underlying_type());
    // return fsec_part2 + fsec_part1;
    // Third effort ... (best approach, but cannot handle many days days due
    // to possible overflow)
    S __s = ddiff.template to_sec_type<S>();
    return __s.fractional_days();
  }
}

/// Sec-Millisec-MicroSec of Week to Day of week
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
constexpr int day_of_week(T sow) noexcept {
  int day_of_week = sow.as_underlying_type() / T::max_in_day;
  return day_of_week;
}

/// @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
///
/// The day of month is actually not needed, since all leap second insertions
/// happen at the begining, i.e. the first day of a month.
///
/// @warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// @see IAU SOFA (iau-dat.c)
/// @see dso::dat
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline int dat(const datetime<T> &t) noexcept {
  return dso::dat(t.mjd());
}

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline t_hmsf as_hmsf(T secday) noexcept {
  return t_hmsf(secday);
}

class TwoPartDate {
private:
  double _big;   ///< Mjd
  double _small; ///< fractional days

public:
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  TwoPartDate(const datetime<T> &d) noexcept
      : _big((double)d.mjd().as_underlying_type()),
        _small(d.sec().fractional_days()) {
    this->normalize();
  }

  explicit constexpr TwoPartDate(double b = 0, double s = 0) noexcept
      : _big(b), _small(s) {
    this->normalize();
  }

  double big() const noexcept { return _big; }
  double small() const noexcept { return _small; }

  void add_seconds(double sec) noexcept {
    _small += sec / sec_per_day;
    this->normalize();
  }

  // difference
  TwoPartDate operator-(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_big - d._big, _small - d._small);
  }
  TwoPartDate operator+(const TwoPartDate &d) const noexcept {
    return TwoPartDate(_big + d._big, _small + d._small);
  }

  template <DateTimeDifferenceType DT>
  double diff(const TwoPartDate &d) const noexcept {
    if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
      return (_big - d._big) + (_small - d._small);
    } else {
      return (_big - d._big) * sec_per_day + (_small - d._small) * sec_per_day;
    }
  }

  // As Julian date, implementing the SOFA Date & Time idiom
  // TODO no, this is plit as integral part and fractional par, only the
  // integral part is JD (not MJD)
  TwoPartDate jd_sofa1() const noexcept {
    return TwoPartDate(_big + dso::mjd0_jd, _small);
  }

  double jd() const noexcept { return (_small + _big) + dso::mjd0_jd; }

  TwoPartDate tai2tt() const noexcept {
    constexpr const double dtat = tt_minus_tai / sec_per_day;
    return TwoPartDate(_big, _small + dtat);
  }

  TwoPartDate utc2tai() const noexcept {
    // normalize so that the big part is MJD & small is fraction of day
    auto utc = *this;

    // Get TAI-UTC at 0h today and extra seconds in day (if any)
    int extra;
    const int leap = dat(modified_julian_day((int)utc._big), extra);

    // Remove any scaling applied to spread leap into preceding day
    utc._small *= (sec_per_day + extra) / sec_per_day;

    // Assemble the TAI result, preserving the UTC split and order
    return TwoPartDate(utc._big, utc._small + leap / sec_per_day);
  }

  TwoPartDate utc2tt() const noexcept {
    const auto tai = this->utc2tai();
    return tai.tai2tt();
  }

  TwoPartDate tai2utc() const noexcept {
    // do it the SOFA way ...
    auto utc1(*this);
    double small = utc1._small;
    double big = utc1._big;
    for (int i = 0; i < 3; i++) {
      TwoPartDate guess = TwoPartDate(big, small).utc2tai();
      small += utc1._big - guess._big;
      small += utc1._small - guess._small;
    }
    return TwoPartDate(utc1._big, small);
  }

  TwoPartDate tt2utc() const noexcept {
    const auto tai = this->tt2tai();
    return tai.tai2utc();
  }

  TwoPartDate tt2tai() const noexcept {
    constexpr const double dtat = tt_minus_tai / sec_per_day;
    return TwoPartDate(_big, _small - dtat);
  }

  /* @brief TT to UT1 MJD
   * Note that because the Earth’s rotation is slowing due to tidal friction,
   * and the rotation rate decreases approximately linearly with time, ∆T
   * increases quadratically.
   * @param[in] dut1 ΔUT1 in [sec]. Thi value should be looked up using e.g.
   *                 IERS products
   * @return The corresponding UT1 MJD, computed using:
   *         ∆T = TT − UT1 = 32.184[sec] + ∆AT − ∆UT1
   */
  TwoPartDate tt2ut1(double dut1) const noexcept {
    /* note that ΔUT1 = UT1 − UTC hence UT1 = ΔUT1 + UTC */
    return tt2utc() + TwoPartDate(0e0, dut1 / sec_per_day);
  }

  double as_mjd() const noexcept { return _small + _big; }

  double jcenturies_sinceJ2000() const noexcept {
    return (_big - j2000_mjd) / days_in_julian_cent +
           _small / days_in_julian_cent;
  }

  double as_fractional_years() const noexcept {
    auto cp(*this);
    cp.normalize();
    const datetime<dso::seconds> d(modified_julian_day((long)_big),
                                   dso::seconds(0));
    const ydoy_date ydoy(d.as_ydoy());
    const double daysinyear = days_in_julian_year + ydoy.__year.is_leap();
    return _small / daysinyear + d.as_fractional_years();
  }

  bool operator>(const TwoPartDate &d) const noexcept {
    return (_big > d._big) || ((_big == d._big) && (_small > d._small));
  }
  bool operator>=(const TwoPartDate &d) const noexcept {
    return (_big > d._big) || ((_big == d._big) && (_small >= d._small));
  }
  bool operator<(const TwoPartDate &d) const noexcept {
    return (_big < d._big) || ((_big == d._big) && (_small < d._small));
  }
  bool operator<=(const TwoPartDate &d) const noexcept {
    return (_big < d._big) || ((_big == d._big) && (_small <= d._small));
  }

  /// @brief Keep _small < 1e0 and _big integral only
  constexpr void normalize() noexcept {
    // fractional part should NOT be negative
    while (_small < 0e0) {
      _small = 1 - _small;
      _big -= 1e0;
    }
    double fmore(0e0), extra(0e0);
    // check if _big part has a fractional part
    if ((fmore = std::modf(_big, &extra)) != 0e0) {
      // assign fractional part to _small and keep integral part to _big
      _small += fmore;
      _big = extra;
    }
    // check if fractional part is >= 1e0
    if (_small >= 1e0) {
      _small = std::modf(_small, &extra);
      _big += extra;
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
    return (d1._big == d2._big) && (d1._small == d2._small);
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
      return TwoPartDate(_big + dso::mjd0_jd + _small, 0e0);
    else if constexpr (S == JdSplitMethod::J2000)
      return TwoPartDate(j2000_jd, _big - j2000_mjd + _small);
    else if constexpr (S == JdSplitMethod::MJD)
      return TwoPartDate(mjd0_jd, _big + _small);
    else
      return TwoPartDate(_big + dso::mjd0_jd, _small);
  }
};

TwoPartDate utc2tai(const TwoPartDate &d) noexcept;

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline TwoPartDate as_two_part_date(const datetime<T> &date) noexcept {
  return TwoPartDate(date);
}

namespace datetime_ranges {
enum class OverlapComparissonType { Strict, AllowEdgesOverlap };
} // namespace datetime_ranges

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T, datetime_ranges::OverlapComparissonType O>
#else
template <typename T, datetime_ranges::OverlapComparissonType O,
          typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline bool intervals_overlap(const datetime<T> &r1_start,
                              const datetime<T> &r1_end,
                              const datetime<T> &r2_start,
                              const datetime<T> &r2_end) noexcept {
  if constexpr (O == datetime_ranges::OverlapComparissonType::Strict) {
    // (StartA <= EndB) and (EndA >= StartB)
    // see
    // https://stackoverflow.com/questions/325933/determine-whether-two-date-ranges-overlap/325964#325964
    return (r1_start <= r2_end) && (r1_end >= r2_start);
  } else {
    // (StartA <= EndB) and (EndA >= StartB)
    return (r1_start < r2_end) && (r1_end > r2_start);
  }
}

} // namespace dso

#endif
