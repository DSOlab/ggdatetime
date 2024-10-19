/** @file
 *
 * A fundamental, generic datetime class. This file contains the definition
 * (and implementation) of a datetime class to be used for Space Geodesy
 * applications.
 */

#ifndef __DTCALENDAR_NGPT__HPP__
#define __DTCALENDAR_NGPT__HPP__

#include "date_integral_types.hpp"
#include "time_integral_types.hpp"
#include "hms_time.hpp"
#include "datetime_interval.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <type_traits>

namespace dso {

/** @brief A generic, templatized Date/Time class.
 *
 * A datetime instance has two fundamental parts (members):
 * - a date part (i.e. holding the year/month/day), and
 * - a time part (i.e. holding hours/minutes/ *seconds)
 *
 * A datetime holds both a date and a time (i.e. of day). The date is stored
 * as a Modified Julian Day (i.e. dso::modified_julian_day). The time of day
 * can be stored via any class of 'second type', i.e. dso::seconds,
 * dso::milliseconds, or dso::microseconds). Every method in the class will
 * (including constructors) will take provisions such that the *seconds are
 * in fact *seconds of day (i.e. do not surpass one day).
 * 
 * Never use negative times; they actually have no physical meaning. Besides
 * that, they can cause UB.
 *
 * The datetime class represents a continuous time-scale (i.e. something like
 * TAI); thus, representing non-continuous time-scales (e.g. UTC) can be
 * ambiguous cause of leap seconds. (Modified) Julian Day cannot unambiguously
 * represent UTC during a leap second unless special measures are taken.  The
 * internal convention is that he quasi-JD day represents UTC days whether the
 * length is 86399, 86400 or 86401 SI seconds.
 *
 * @tparam S Any class of 'second type', i.e. any class S that has a (static)
 *           member variable S::is_of_sec_type set to true. This can be
 *           dso::seconds, dso::milliseconds, dso::microseconds.
 *
 * @note Constructors can be called with the time part being more than one day;
 *       (e.g.
 *       \code{.cpp}
 *         datetime<dso::seconds> d {year(2016), month(12), day_of_month(15),
 *         seconds(86401};)
 *       \endcode
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class datetime {
private:
  /** A constructor that will NOT call normalize! 
   *
   * Use with extra care. The char parameter is actually usseless, but is 
   * there to make sure that the user intents to use this function.
   */
  constexpr datetime(modified_julian_day mjd, S sec,
                     [[maybe_unused]] char c) noexcept
      : m_mjd(mjd), m_sec(sec) {};

public:
  /** Expose the underlying sec type S */
  using SecIntType = typename S::underlying_type;
  /** Expose the underlying modified_julian_day int */
  typedef modified_julian_day::underlying_type DaysIntType;

  /** Maximum possible date (seconds are 0, modified_julian_day is max
   * possible.
   */
  constexpr static datetime max() noexcept {
    return datetime(modified_julian_day::max(), S(0), 'c');
  }

  /** Minimum possible date (seconds are 0, modified_julian_day is min
   * possible.
   */
  constexpr static datetime min() noexcept {
    return datetime(modified_julian_day::min(), S(0), 'c');
  }

  /** Reference epoch (J2000.0), as a Modified Julian Date. */
  constexpr static datetime j2000_mjd() noexcept {
    return datetime(modified_julian_day(51544), S(S::max_in_day / 2L), 'c');
  }

  /** Default constructor. */
  explicit constexpr datetime() noexcept : m_mjd(dso::J2000_MJD), m_sec(0) {};

  /** @brief Julian centuries since J2000.
   *
   * Note that a Julian Century has 365.25 days.
   */
  double jcenturies_sinceJ2000() const noexcept {
    const double d_mjd = (double)(m_mjd.as_underlying_type());
    const double fdays = fractional_days(m_sec);
    return ((d_mjd - J2000_MJD) + fdays) / DAYS_IN_JULIAN_CENT;
  }

  /** @brief Constructor from calendar date.
   *
   * Constructor from year, month, day of month and sec type.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, S s)
      : m_mjd(y, m, d), m_sec(s) {
    this->normalize();
  };

  /** @brief Constructor from calendar date and time.
   *
   * Constructor from year, month, day of month and fractional seconds.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, hours hr, minutes mn,
                     double fsecs)
      : m_mjd(y, m, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year and time.
   *
   * Constructor from year, day of year and fractional seconds.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, day_of_year d, hours hr, minutes mn, double fsecs)
      : m_mjd(y, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** @brief Constructor from calendar date and time.
   *
   * Constructor from year, month, day of month, hours, minutes and second
   * type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetime(year y, month m, day_of_month d, hours hr = hours(0),
                     minutes mn = minutes(0), S sec = S(0))
      : m_mjd(y, m, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from calendar date and time.
   *
   * Constructor from ymd_date and hms_time<S>. No validation performed. 
   */
  datetime(const ymd_date &ymd, const hms_time<S> &hms) noexcept
      : m_mjd(ymd.yr(), ymd.mn(), ymd.dm()),
        m_sec(hms.hr(), hms.mn(), hms.nsec()) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year and time.
   *
   * Constructor from year, day of year, hours, minutes and second type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  datetime(year y, day_of_year d, hours hr = hours(0), minutes mn = minutes(0),
           S sec = S(0))
      : m_mjd(y, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year and time.
   *
   * Constructor from year, day of year, and second type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  datetime(year y, day_of_year d, S sec) : m_mjd(y, d), m_sec(sec) {
    this->normalize();
  }

  /** @brief Constructor from MJD and time. */
  constexpr datetime(modified_julian_day mjd, hours hr, minutes mn,
                     S sec) noexcept
      : m_mjd(mjd), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from modified julian day, and second type S. */
  constexpr datetime(modified_julian_day mjd, S sec = S(0)) noexcept
      : m_mjd(mjd), m_sec(sec) {
    this->normalize();
  }

  /** @brief Non-normalizing constrcutor; be careful!
   *
   * Constructor from modified julian day, and second type S. This version
   * will **NOT** call normalize, hence be very very carefull when using it.
   * The sec part, should be positive and less than *sec of day (for type S).
   */
  static constexpr datetime non_normalize_construct(modified_julian_day mjd,
                                                    S sec) noexcept {
    return datetime(mjd, sec, 'y');
  }

  /** @brief Constructor from GPS Week and Seconds of Week */
  constexpr datetime(gps_week w, S sow) noexcept
      : m_mjd(w.as_underlying_type() * 7 +
              sow.as_underlying_type() / S::max_in_day + JAN61980),
        m_sec(sow) {
    m_sec.remove_days();
  }

  /** @brief Get the Modified Julian Day (const). */
  constexpr modified_julian_day imjd() const noexcept { return m_mjd; }

  /** @brief Get the number of *seconds (as type S) of the instance. */
  constexpr S sec() const noexcept { return m_sec; }

  /** @brief Seconds in day as fractional days */
  constexpr FractionalDays fractional_days() const noexcept {
    return dso::to_fractional_days<S>(m_sec);
  }

  /** @brief Add an interval to a datetime.
   *
   * Operator '+' where the right-hand-side is an interval. Note that the 
   * addition here is algebraic, i.e. the interval is added to or subtracted 
   * from the instance, depending on its sign.
   * Hence, an interval of e.g. (- 2 days, 30 sec) will actually be subtracted
   * from the instance, not added to it.
   */
  constexpr datetime<S>
  operator+(const datetime_interval<S> &dt) const noexcept {
    const auto mjd =
        m_mjd + modified_julian_day(std::copysign(dt.days(), dt.sign()));
    const auto sec = m_sec + dt.signed_sec();
    return datetime<S>(mjd, sec);
  }

  /** @brief Add an interval to a datetime.
   *
   * Note that the aidition here is algebraic, i.e. the interval is added to
   * or subtracted from the instance, depending on its sign.
   * Hence, an interval of e.g. (- 2 days, 30 sec) will actually be subtracted
   * from the instance, not added to it.
   */
  constexpr void operator+=(const datetime_interval<S> &dt) noexcept {
    m_mjd += modified_julian_day(std::copysign(dt.days(), dt.sign()));
    m_sec += dt.signed_sec();
    this->normalize();
  }

  /** @brief Operator '-' between two instances, produces a (signed) interval */
  constexpr datetime_interval<S>
  operator-(const datetime<S> &other) const noexcept {
    /* big date at d1, small at d2 */
    auto d1 = *this;
    auto d2 = other;
    int sgn = 1;
    if (*this < other) {
      d1 = d2;
      d2 = *this;
      sgn = -1;
    }
    /* seconds, could be negative */
    SecIntType secs =
        d1.sec().as_underlying_type() - d2.sec().as_underlying_type();
    /* branchless TODO which is faster ? */
    d1.m_mjd = d1.m_mjd - modified_julian_day(1 * (secs < 0));
    secs = secs + S::max_in_day * (secs < 0);
    DaysIntType days =
        d1.imjd().as_underlying_type() - d2.imjd().as_underlying_type();
    /* note that if days are 0 and the sign is negative, it must be applied to
     * the seconds part */
    return datetime_interval<S>(days * sgn,
                                S(std::copysign(secs, (days == 0) * sgn)));
  }

  /** @brief Cast to any datetime<T> instance, regardless of what T is.
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

  /** @brief Overload equality operator. */
  constexpr bool operator==(const datetime &d) const noexcept {
    return m_mjd == d.m_mjd && m_sec == d.m_sec;
  }

  /** @brief Overload in-equality operator. */
  constexpr bool operator!=(const datetime &d) const noexcept {
    return !(this->operator==(d));
  }

  /** @brief Overload ">" operator. */
  constexpr bool operator>(const datetime &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec);
  }

  /** @brief Overload ">=" operator. */
  constexpr bool operator>=(const datetime &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec);
  }

  /** @brief Overload "<" operator. */
  constexpr bool operator<(const datetime &d) const noexcept {
    return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec);
  }

  /** @brief Overload "<=" operator. */
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
    const DaysIntType more =
        std::copysign(m_sec.as_underlying_type(), 1) / S::max_in_day;
    /* add to current days, with the right sign */
    const DaysIntType days =
        m_mjd.as_underlying_type() + std::copysign(more, core::sgn(m_sec));
    /* leftover seconds (positive) */
    const SecIntType s =
        std::copysign(m_sec.as_underlying_type(), 1) - more * S::max_in_day;
    /* if initial seconds were negative, adjust */
    m_mjd = modified_julian_day(days - 1 * (m_sec < S(0)));
    m_sec = S((S::max_in_day - s) * (m_sec < S(0)) + s * (m_sec >= S(0)));
#ifdef DEBUG
    assert(m_sec >= S(0) && m_sec < S(S::max_in_day));
#endif
  }

  /** @brief Get the difference between two datetime instances as some kind of
   *  floating type time.
   *
   * The difference can be obtained as a fractional days or fractional seconds,
   * depending on the template parameter \p DT.
   * If called as diff(d1,d2), the computation is d1-d2; the difference can be
   * negative if d2>d1.
   *
   * @warning Does not take into account leap seconds.
   */
  template <DateTimeDifferenceType DT>
  typename DateTimeDifferenceTypeTraits<DT>::dif_type
  diff(const datetime<S> &d) const noexcept {
    return (this->operator-(d)).template to_fraction<DT>();
  }

  /** @brief Cast to double (i.e. fractional) Modified Julian Date. */
  constexpr double fmjd() const noexcept {
    return static_cast<double>(m_mjd.as_underlying_type()) +
           fractional_days().days();
  }

  /** @brief Cast to double (i.e. fractional) Julian Date. */
  constexpr double as_jd() const noexcept {
    const double jd = m_mjd.to_julian_day();
    return m_sec.fractional_days() + jd;
  }

  /** @brief Cast to year, month, day of month
   *
   * Only the date part is considered. Time (of day) is ignored.
   */
  constexpr ymd_date as_ymd() const noexcept { return m_mjd.to_ymd(); }

  /** @brief Cast to year, day_of_year.
   *
   * Only the date part is considered. Time (of day) is ignored.
   */
  constexpr ydoy_date as_ydoy() const noexcept { return m_mjd.to_ydoy(); }

  /** @brief Convert to Julian Epoch */
  constexpr double as_julian_epoch() const noexcept {
    return epj(this->fmjd());
  }

  /** @brief Cast to gps_week and S-Of-Week */
  constexpr gps_week gps_wsow(S &sow) const noexcept {
    const DaysIntType mjd = m_mjd.as_underlying_type();
    gps_week w((mjd - JAN61980) / 7);
    sow = S(((mjd - JAN61980) - w.as_underlying_type() * 7) * S::max_in_day);
    sow += m_sec;
    return w;
  }

  /** @brief Add seconds of any type (T). */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr void add_seconds(T nsec) noexcept {
    constexpr const auto TT = (S::template sec_factor<unsigned long>() >=
                               T::template sec_factor<unsigned long>());
    return __add_seconds_impl<T>(nsec, std::integral_constant<bool, TT>{});
  }

  /** @brief Transform instance to TT, assuming it is in TAI
   *
   * The two time scales are connected by the formula:
   * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
   */
  [[nodiscard]] constexpr datetime<S> tai2tt() const noexcept {
    constexpr const S dtat =
        dso::cast_to<nanoseconds, S>(nanoseconds(TT_MINUS_TAI_IN_NANOSEC));
    return datetime(m_mjd, m_sec + dtat);
  }

  /** @brief Transform an instance to TAI assuming it is in TT
   *
   * The two time scales are connected by the formula:
   * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
   */
  [[nodiscard]] constexpr datetime<S> tt2tai() const noexcept {
    constexpr const SecIntType dtat = static_cast<SecIntType>(
        TT_MINUS_TAI * S::template sec_factor<double>());
    return datetime(m_mjd, m_sec - dtat);
  }

  /** @brief Transform an instance to GPS Time assuming it is in TAI
   *
   * The two time scales are connected by the formula:
   * \f$ TAI = GPSTime + 19 [sec] \f$
   */
  [[nodiscard]] constexpr datetime<S> tai2gps() const noexcept {
    return datetime(m_mjd, m_sec - dso::cast_to<seconds, S>(seconds(19)));
  }

  [[nodiscard]] constexpr datetime<S> tt2gps() const noexcept {
    return datetime(m_mjd, m_sec - dso::cast_to<milliseconds, S>(
                                       milliseconds(19000 + 32184)));
  }

  [[nodiscard]] constexpr datetime<S> gps2tai() const noexcept {
    return datetime(m_mjd, m_sec + dso::cast_to<seconds, S>(seconds(19)));
  }

  [[nodiscard]] constexpr datetime<S> gps2tt() const noexcept {
    return gps2tai().tai2tt();
  }

private:
  /** @brief Add any second type T where S is of higher resolution than T
   *
   * This is the implementation for adding any type of seconds (T), where T is
   * of lower resolution than S, to the instanece. The input seconds sec will
   * be cast to S-type and then added to the instance.
   * This is the implementation function meant to work via tag dispatch, so it
   * needs a dummy parameter of type std::true_type
   *
   * @tparam T Any seconds type where S::max_id_day > T::max_in_day
   * @param[in] sec Seconds of T-type where S::max_id_day > T::max_in_day
   * @return nothing
   */
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::true_type) noexcept {
    S ssec = dso::cast_to<T, S>(sec);
    m_sec += ssec;
    this->normalize();
    return;
  }

  /** @brief Add any second type T where T is of higher resolution than S
   *
   * This is the implementation for adding any type of seconds (T), where T is
   * of higher resolution than S, to the instance. The instance will first be
   * cast into T-type, the input seconds are added to the instance and then
   * the instance will be re-casted to S-type.
   * This is the implementation function meant to work via tag dispatch, so it
   * needs a dummy parameter of type std::false_type
   *
   * @tparam T Any seconds type where T::max_id_day > S::max_in_day
   * @param[in] sec Seconds of T-type where T::max_id_day > S::max_in_day
   * @return nothing
   *
   * @warning The input seconds (parameter) is of higher resolution than the
   *          instance, thus loss of accuracy may happen.
   */
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::false_type) noexcept {
    m_sec += dso::cast_to<T, S>(sec);
    this->normalize();
    return;
  }

  modified_julian_day m_mjd; /** Modified Julian Day */
  S m_sec;                   /** Time of day in S precision. */
}; /* datetime<S> */

/** @brief A generic, templatized Date/Time class to represent UTC dates.
 *
 * A datetimeUTC instance has two fundamental parts (members):
 * - a date part (i.e. holding the year/month/day), and
 * - a time part (i.e. holding hours/minutes/ *seconds)
 *
 * A datetime holds both a date and a time (i.e. of day). The date is stored
 * as a Modified Julian Day (i.e. dso::modified_julian_day). The time of day
 * can be stored via any class of 'second type', i.e. dso::seconds,
 * dso::milliseconds, or dso::microseconds). Every method in the class will
 * (including constructors) will take provisions such that the *seconds are
 * in fact *seconds of day (i.e. do not surpass one day).
 * 
 * Never use negative times; they actually have no physical meaning. Besides
 * that, they can cause UB.
 *
 * @tparam S Any class of 'second type', i.e. any class S that has a (static)
 *           member variable S::is_of_sec_type set to true. This can be
 *           dso::seconds, dso::milliseconds, dso::microseconds.
 *
 * @note Constructors can be called with the time part being more than one day;
 *       (e.g.
 *       \code{.cpp}
 *         datetime<dso::seconds> d {year(2016), month(12), day_of_month(15),
 *         seconds(86401};)
 *       \endcode
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <class S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
class datetimeUtc {
public:
  /** Expose the underlying sec type S */
  using SecIntType = typename S::underlying_type;
  /** Expose the underlying modified_julian_day int */
  typedef modified_julian_day::underlying_type DaysIntType;

  /** @brief Maximum possible date (seconds are 0, modified_julian_day is max
   * possible.
   */
  constexpr static datetimeUtc max() noexcept {
    return datetimeUtc(modified_julian_day::max(), S(0));
  }

  /** @brief Minimum possible date (seconds are 0, modified_julian_day is min
   * possible.
   */
  constexpr static datetimeUtc min() noexcept {
    return datetimeUtc(modified_julian_day::min(), S(0));
  }

  /** @brief Default constructor. */
  explicit constexpr datetimeUtc() noexcept
      : m_mjd(dso::J2000_MJD), m_sec(0) {};

  /** @brief Constructor from calendar date.
   *
   * Constructor from year, month, day of month and sec type.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetimeUtc(year y, month m, day_of_month d, S s)
      : m_mjd(y, m, d), m_sec(s) {
    this->normalize();
  };

  /** @brief Constructor from calendar date.
   *
   * Constructor from year, month, day of month and fractional seconds.
   *  If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetimeUtc(year y, month m, day_of_month d, hours hr, minutes mn,
                        double fsecs)
      : m_mjd(y, m, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year.
   *
   * Constructor from year, day of year and fractional seconds.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetimeUtc(year y, day_of_year d, hours hr, minutes mn,
                        double fsecs)
      : m_mjd(y, d), m_sec(hr, mn, fsecs) {
    this->normalize();
  }

  /** @brief Constructor from calndar date.
   *
   * Constructor from year, month, day of month, hours, minutes and second
   * type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  constexpr datetimeUtc(year y, month m, day_of_month d, hours hr = hours(0),
                        minutes mn = minutes(0), S sec = S(0))
      : m_mjd(y, m, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from calendar date.
   *
   * Constructor from ymd_date and hms_time<S>. No validation performed. 
   */
  datetimeUtc(const ymd_date &ymd, const hms_time<S> &hms) noexcept
      : m_mjd(ymd.yr(), ymd.mn(), ymd.dm()),
        m_sec(hms.hr(), hms.mn(), hms.nsec()) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year.
   *
   * Constructor from year, day of year, hours, minutes and second type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  datetimeUtc(year y, day_of_year d, hours hr = hours(0),
              minutes mn = minutes(0), S sec = S(0))
      : m_mjd(y, d), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from year, day of year.
   * 
   * Constructor from year, day of year, and second type S.
   * If an invalid date is passed-in, the constructor will throw.
   */
  datetimeUtc(year y, day_of_year d, S sec) : m_mjd(y, d), m_sec(sec) {
    this->normalize();
  }

  /** @brief Constructor from MJD and time of day. 
   *
   * Constructor from modified julian day, hours, minutes and second type S.
   */
  constexpr datetimeUtc(modified_julian_day mjd, hours hr, minutes mn,
                        S sec) noexcept
      : m_mjd(mjd), m_sec(hr, mn, sec) {
    this->normalize();
  }

  /** @brief Constructor from MJD and time of day. 
   * 
   * Constructor from modified julian day, and second type S. 
   */
  constexpr datetimeUtc(modified_julian_day mjd, S sec = S(0)) noexcept
      : m_mjd(mjd), m_sec(sec) {
    this->normalize();
  }

  /** @brief Get the Modified Julian Day (const). */
  constexpr modified_julian_day imjd() const noexcept { return m_mjd; }

  /** @brief Get the number of *seconds (as type S) of the instance. */
  constexpr S sec() const noexcept { return m_sec; }

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
  constexpr datetimeUtc<T> cast_to() const noexcept {
    return datetimeUtc<T>(m_mjd, dso::cast_to<S, T>(m_sec));
  }

  /** @brief Overload equality operator. */
  constexpr bool operator==(const datetimeUtc &d) const noexcept {
    return m_mjd == d.m_mjd && m_sec == d.m_sec;
  }

  /** @brief Overload in-equality operator. */
  constexpr bool operator!=(const datetimeUtc &d) const noexcept {
    return !(this->operator==(d));
  }

  /** @brief Overload ">" operator. */
  constexpr bool operator>(const datetimeUtc &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec);
  }

  /** @brief Overload ">=" operator. */
  constexpr bool operator>=(const datetimeUtc &d) const noexcept {
    return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec);
  }

  /** @brief Overload "<" operator. */
  constexpr bool operator<(const datetimeUtc &d) const noexcept {
    return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec);
  }

  /** @brief Overload "<=" operator. */
  constexpr bool operator<=(const datetimeUtc &d) const noexcept {
    return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec <= d.m_sec);
  }

  /** @brief Cast to year, month, day of month */
  constexpr ymd_date as_ymd() const noexcept { return m_mjd.to_ymd(); }

  /** @brief Cast to year, day_of_year */
  constexpr ydoy_date as_ydoy() const noexcept { return m_mjd.to_ydoy(); }

  /** @brief Overload '-' operator taking into account leap seconds. 
   *
   * Operator '-' between two instances, produces a (signed) interval, i.e. 
   * the time lapsed between the two dates. Note that since the two epochs are 
   * in UTC, leap seconds must be taken into account.
   */
  constexpr datetime_interval<S>
  operator-(const datetimeUtc<S> &dt) const noexcept {
    /* big date at d1, small at d2 */
    auto d1 = *this;
    auto d2 = dt;
    int sgn = 1;
    if (*this < dt) {
      d1 = dt;
      d2 = *this;
      sgn = -1;
    }
    /* let's see what happens with leap seconds */
    int dat1 = dso::dat(d1.imjd());
    int dat2 = dso::dat(d2.imjd());
    /* seconds, could be negative */
    SecIntType secs =
        d1.sec().as_underlying_type() - d2.sec().as_underlying_type();
    /* branchless TODO which is faster ? */
    d1.m_mjd = d1.m_mjd - modified_julian_day(1 * (secs < 0));
    secs = secs + S::max_in_day * (secs < 0);
    DaysIntType days =
        d1.imjd().as_underlying_type() - d2.imjd().as_underlying_type();
    SecIntType ddat((dat1 - dat2) * S::template sec_factor<SecIntType>());
    /* note that id days are 0 and the sign is negative, it must be applied to
     * the seconds part */
    return datetime_interval<S>(
        days * sgn, S(std::copysign(secs + ddat, (days == 0) * sgn)));
  }

  /** @brief Normalize a datetimeUTC instance.
   *
   * Split the date and time parts such that the time part is always less
   * than one day (i.e. make it time-of-day) and positive (i.e.>=0).
   * Remove whole days of from the time part and add them to the date part.
   */
  constexpr void normalize() noexcept {
    int extra_sec_in_day = 0;
    dat(m_mjd, extra_sec_in_day);
    /* for each MJD, remove integral days. Each MJD may have a different
     * number of seconds, since we are in UTC time scale. Hence, iteratively
     * remove whole days using the number of seconds for each day
     */
    S daysec;
    while (m_sec >= (daysec = S(S::max_in_day +
                                extra_sec_in_day *
                                    S::template sec_factor<SecIntType>()))) {
      m_sec -= daysec;
      ++m_mjd;
      dat(m_mjd, extra_sec_in_day);
    }
  }

  /** @brief Add seconds of any type (T). */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
  template <class T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
  constexpr void add_seconds(T nsec) noexcept {
    constexpr const auto TT = (S::template sec_factor<unsigned long>() >=
                               T::template sec_factor<unsigned long>());
    return __add_seconds_impl<T>(nsec, std::integral_constant<bool, TT>{});
  }

private:
  /** @brief Add any second type T where S is of higher resolution than T
   *
   * This is the implementation for adding any type of seconds (T), where T is
   * of lower resolution than S, to the instanece. The input seconds sec will
   * be cast to S-type and then added to the instance.
   * This is the implementation function meant to work via tag dispatch, so it
   * needs a dummy parameter of type std::true_type
   *
   * @tparam T Any seconds type where S::max_id_day > T::max_in_day
   * @param[in] sec Seconds of T-type where S::max_id_day > T::max_in_day
   * @return nothing
   */
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::true_type) noexcept {
    S ssec = dso::cast_to<T, S>(sec);
    m_sec += ssec;
    this->normalize();
    return;
  }

  /** @brief Add any second type T where T is of higher resolution than S
   *
   * This is the implementation for adding any type of seconds (T), where T is
   * of higher resolution than S, to the instance. The instance will first be
   * cast into T-type, the input seconds are added to the instance and then
   * the instance will be re-casted to S-type.
   * This is the implementation function meant to work via tag dispatch, so it
   * needs a dummy parameter of type std::false_type
   *
   * @tparam T Any seconds type where T::max_id_day > S::max_in_day
   * @param[in] sec Seconds of T-type where T::max_id_day > S::max_in_day
   * @return nothing
   *
   * @warning The input seconds (parameter) is of higher resolution than the
   *          instance, thus loss of accuracy may happen.
   */
  template <class T>
  constexpr void __add_seconds_impl(T sec, std::false_type) noexcept {
    m_sec += dso::cast_to<T, S>(sec);
    this->normalize();
    return;
  }

  modified_julian_day m_mjd; /** Modified Julian Day */
  S m_sec;                   /** Time of day in S precision. */
}; /* datetimeUtc<S> */

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

/** @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
 *
 * TODO add documentation
 *
 * The day of month is actually not needed, since all leap second insertions
 * happen at the begining, i.e. the first day of a month.
 *
 * @see dso::dat
 */
#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt T>
#else
template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
inline int dat(const datetime<T> &t) noexcept {
  return dso::dat(t.imjd());
}

namespace datetime_ranges {
enum class OverlapComparissonType { Strict, AllowEdgesOverlap };
} /* namespace datetime_ranges */

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

} /* namespace dso */

#endif
