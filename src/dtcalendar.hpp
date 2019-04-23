///
/// @file  dtcalendar.hpp
///
/// @brief A fundamental, template datetime class.
///
/// This file contains the definition (and implementation) of a datetime class
/// to be used for GNSS applications.
///
/// @author xanthos
///
/// @bug No known bugs.
///

#ifndef __DTCALENDAR_NGPT__HPP__
#define __DTCALENDAR_NGPT__HPP__

#include <cassert>
#include "dtfund.hpp"

#ifdef DEBUG
# include <iostream>
# include <iomanip>  // std::setprecision
# include <sstream>  // std::ostringstream
#endif

namespace ngpt
{

/// @brief A generic, templatized class to hold a datetime period/interval.
///
/// A datetime_interval represents a time (datetime) interval or period, i.e.
/// 5 days, 12 hours and 49 seconds. We assume a continuous time scale (no leap
/// seconds are taken into consideration --this is only an interval not an
/// actual datetime instance--).
/// A datetime_interval instance can only have positive (or zero) values (for
/// both its members).
///
/// A datetime_interval instance has two fundamental parts (members):
/// - a day part (i.e. holding the days), and 
/// - a time part (i.e. holding any type S i.e. any second type)
///
/// The purpose of this class is to work together with the datetime class.
///
/// @tparam S Any class of 'second type', i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true. This can be
///           ngpt::seconds, ngpt::milliseconds, ngpt::microseconds.
///
///
/// @note  - Any instance of the class has two members, m_days an integer
///          representing the (MJ) days and m_secs, an instance of type S, 
///          representing the fractional day part. However, the parts may be
///          mixed (!), that is if e.g. S is ngpt::seconds, the m_secs part may
///          actualy have any value, including ones larger than 86400. That is
///          to say that an instance can be constructed as:
///        \code{.cpp}
///          datetime_interval<seconds> d {modified_julian_day(1), seconds(90000)};
///        \endcode
///          Should the user want the "normalization" of the instance (that is
///          remove whole days from the m_secs part and add them to the m_days
///          part), call the function datetime_interval::normalize().
///          
template<class S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
  class datetime_interval {
public:

  /// Null constructor (everything set to 0).
  explicit constexpr
  datetime_interval() noexcept
    : m_days{0},
      m_secs{0}
  {};

  /// Constructor given a Modified Julian Day and *seconds (type S).
  ///
  /// param[in] d  Number of days; a ngpt::modified_julian_day instance.
  /// param[in] s  Number of *seconds; an instance of type S
  ///
  /// @note  A datetime_interval can only have positive values for its member
  ///        variables. However, it can happen that the constructor gets a
  ///        negative value for the seconds part, if the resulting time_interval
  ///        is positive after normalization. E.g. datetime_interval(2, -123)
  ///        is a valid time_interval; the constructor will first call the
  ///        normalize function, which will transform the negative seconds to
  ///        positive and remove whole days.
  explicit constexpr
  datetime_interval(modified_julian_day d, S s) noexcept
    : m_days{d},
      m_secs{s}
  {
    this->normalize();
#ifdef USE_DATETIME_CHECKS
    assert( m_days.as_underlying_type() >= 0
         && m_secs.as_underlying_type() >= 0 );
#endif
  };

  /// Get the number of days of the instance.
  /// @return The number of days in the instance, as ngpt::modified_julian_day
  modified_julian_day
  days() const noexcept
  { return m_days; }
    
  /// Get the number of *seconds (as type S) of the instance.
  /// @return The number of *seconds (as type S) of the instance.
  S
  secs() const noexcept
  { return m_secs; }
    
  /// Cast to double (i.e. fractional) Modified Julian Date.
  constexpr double
  as_mjd() const noexcept
  {
    return static_cast<double>(m_days.as_underlying_type())
                             + m_secs.fractional_days();
  }
    
  /// @brief Normalize a datetime_interval instance.
  ///
  /// Split the date and time parts such that the time part is always less
  /// than one day (i.e. make it time-of-day) and positive (i.e.>=0).
  /// Remove whole days of from the time part and add them to the date part.
  constexpr void
  normalize() noexcept
  {
    typename S::underlying_type secs { m_secs.as_underlying_type() };
    m_days += secs / S::max_in_day;
    m_secs  = secs / S::max_in_day;
    return;
  }

  /// Operator >
  constexpr bool
  operator>(const datetime_interval& d) const noexcept
  { return m_days > d.m_days || (m_days == d.m_days && m_secs > d.m_secs); }
    
  /// Operator >=
  constexpr bool
  operator>=(const datetime_interval& d) const noexcept
  { return m_days > d.m_days || (m_days == d.m_days && m_secs >= d.m_secs); }
    
  /// Operator <
  constexpr bool
  operator<(const datetime_interval& d) const noexcept
  { return m_days < d.m_days || (m_days == d.m_days && m_secs < d.m_secs); }
    
  /// Operator <=
  constexpr bool
  operator<=(const datetime_interval& d) const noexcept
  { return m_days < d.m_days || (m_days == d.m_days && m_secs <= d.m_secs); }

  /// Operator ==
  constexpr bool
  operator==(const datetime_interval& d) const noexcept
  { return (m_days == d.m_days) && (m_secs == d.m_secs); }
    
  /// Operator !=
  constexpr bool
  operator!=(const datetime_interval& d) const noexcept
  { return !(this->operator==(d)); }

  /// @todo wtf is this?? it needs checking
  datetime_interval
  operator/(int div) const noexcept
  {
    double d = static_cast<double>(m_days.as_underlying_type());
    double i, f;
    f = std::modf(d/div, &i);
    auto new_mjd = static_cast<modified_julian_day::underlying_type>(i);

    d = static_cast<double>(m_secs.as_underlying_type());
    d = d/div + f*S::max_in_day;
    auto new_sec = static_cast<typename S::underlying_type>(d);

    return datetime_interval{modified_julian_day{new_mjd}, S{new_sec}};
  }

private:
  modified_julian_day m_days;
  S                   m_secs;
}; // end class datetime_interval

/// @brief A generic, templatized Date/Time class.
///
/// A datetime instance has two fundamental parts (members):
/// - a date part (i.e. holding the year/month/day), and 
/// - a time part (i.e. holding hours/minutes/*seconds)
///
/// A datetime holds both a date and a time (i.e. of day). The date is recorded
/// as a Modified Julian Day (i.e. ngpt::modified_julian_day). The time of day
/// can be expressed via any class of 'second type', i.e. ngpt::seconds, 
/// ngpt::milliseconds, or ngpt::microseconds). Actually, there is no restriction
/// for the time to be 'time of day'; in-fact, it can hold even days (e.g. 2 or
/// 3 days in ngpt::seconds), but usually this is not good practice and should be
/// avoided. Especially when the second type is e.g. ngpt::milliseconds, then
/// holding multiple days in the time part could result in overflow. Additionaly,
/// some member functions (e.g. the equality operators), expect the datetime
/// to be split as date and time (of day), so make sure you conform to this.
/// To remove whole days from the time part and add them to the date part, use
/// the datetime::normalize method.
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
///           ngpt::seconds, ngpt::milliseconds, ngpt::microseconds.
///
/// @note Constructors can be called with the time part being more than one day;
///       (e.g.
///       \code{.cpp}
///         datetime<ngpt::seconds> d {year(2016), month(12), day_of_month(15), seconds(86401};)
///       \endcode
///       If you think that this is a posibility, then call datetime::normalize
///       imidiately after the construction. An automatic call of the function
///       datetime::normalize could be added in the body of (nearly all)
///       constructors, but that would be time consuming. So, the option chosen
///       is **not**  to include a call to datetime::normalize and let the user
///       call the function if needed.
///
/// @bug All operator expect normalized dates (i.e. the fraction_of_day part should
///      not be over 1 day). We have to make sure that this is always the case.
template<class S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
  class datetime {
public:

  /// Expose the underlying sec type S
  using sec_type = S;

  /// Maximum possible date (seconds are 0, modified_julian_day is max
  /// possible).
  constexpr static datetime
  max() noexcept
  { return datetime{modified_julian_day::plus_infinity(), S{0}}; }
    
  /// Minimum possible date (seconds are 0, modified_julian_day is min
  /// possible).
  constexpr static datetime
  min() noexcept
  { return datetime{modified_julian_day::minus_infinity(), S{0}}; }

  /// Default (zero) constructor.
  explicit constexpr
  datetime() noexcept
    : m_mjd(ngpt::j2000_mjd),
      m_sec(0)
  {};
    
  /// Constructor from year, month, day of month and sec type.
  explicit constexpr
  datetime(year y, month m, day_of_month d, S s) noexcept
    : m_mjd{cal2mjd(y, m, d)},
      m_sec{s}
  {
#ifdef USE_DATETIME_CHECKS
    assert( y.as_underlying_type() > 0
        && (m.as_underlying_type() >=1 && m.as_underlying_type() <= 12)
        && d.is_valid(y, m) );
    assert(s>=S{0});
#endif
    this->normalize();
  };

  /// Constructor from year, month, day of month and any sec type T,
  /// convertible to S.
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  explicit
  datetime(year y, month m, day_of_month d, T t) noexcept
    : m_mjd{cal2mjd(y, m, d)},
      m_sec{S(t)}
  {
    this->normalize();
  }
    
  /// Constructor from year, day of year and any sec type T,
  /// convertible to S.
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  explicit
  datetime(year y, day_of_year d, T t) noexcept
    : m_mjd{ydoy2mjd(y, d)},
      m_sec{S(t)}
  {
    this->normalize();
  }
    
  /// Constructor from year, month, day of month, hours, minutes and
  /// any second type T convertible to S (i.e. T can be cast to S).
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  explicit
  datetime(year y, month m, day_of_month d, hours hr, minutes mn, T sec)
  noexcept
    : m_mjd{cal2mjd(y, m, d)},
      m_sec{hr, mn, S(sec)}
  {
    this->normalize();
  }
    
  /// Constructor from year, day of year, hours, minutes and
  /// any second type T convertible to S (i.e. T can be cast to S).
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
              >
  explicit
  datetime(year y, day_of_year d, hours hr, minutes mn, T sec) noexcept
    : m_mjd{ydoy2mjd(y, d)},
      m_sec{hr, mn, S(sec)}
  {
    this->normalize();
  }

  /// Constructor from year, month, day of month and fractional seconds.
  explicit
  datetime(year y, month m, day_of_month d, hours hr, minutes mn, double fsecs)
  noexcept
    : m_mjd{cal2mjd(y, m, d)},
      m_sec{hr, mn, fsecs}
  {
    this->normalize();
  }
    
  /// Constructor from year, day of year and fractional seconds.
  explicit
  datetime(year y, day_of_year d, hours hr, minutes mn, double fsecs) noexcept
    : m_mjd{ydoy2mjd(y, d)},
      m_sec{hr, mn, fsecs}
  {
    this->normalize();
  }
    
  /// Constructor from year, month, day of month, hours, minutes and
  /// second type S.
  explicit
  datetime(year y, month m, day_of_month d, hours hr=hours(),
           minutes mn=minutes(), S sec=S()) noexcept
    : m_mjd{cal2mjd(y, m, d)},
      m_sec{hr, mn, sec}
  {
    this->normalize();
  }
    
  /// Constructor from year, day of year, hours, minutes and
  /// second type S.
  explicit
  datetime(year y, day_of_year d, hours hr=hours(), 
           minutes mn=minutes(), S sec=S()) noexcept
    : m_mjd{ydoy2mjd(y, d)},
      m_sec{hr, mn, sec}
  {
    this->normalize();
  }
    
  /// Constructor from modified julian day, hours, minutes and 
  /// second type S.
  explicit
  datetime(modified_julian_day mjd, hours hr=hours(), minutes mn=minutes(),
           S sec=S()) noexcept
    : m_mjd{mjd},
      m_sec{hr, mn, sec}
  {
    this->normalize();
  }
    
  /// Constructor from modified julian day, and second type S.
  explicit
  datetime(modified_julian_day mjd, S sec=S()) noexcept
    : m_mjd{mjd},
      m_sec{sec}
  {
    this->normalize();
  }

  /// Constructor from GPS Week and Seconds of Week
  explicit
  datetime(gps_week w, S sow) noexcept
    : m_mjd{w.as_underlying_type()*7
           +sow.as_underlying_type()/S::max_in_day
           +jan61980},
    m_sec{sow}
  {
    m_sec.remove_days();
  }

  /// Get the Modified Julian Day (const).
  constexpr modified_julian_day
  mjd() const noexcept
  { return m_mjd; }
    
  /// Get the *seconds as underlying_type
  constexpr typename S::underlying_type
  secs() const noexcept
  { return m_sec.as_underlying_type(); }

  /// @brief Add any second type T, convertible to S.
  /// 
  /// Add to the datetime instance an amount of t 'second type'. If the total
  /// amount of the 'second type' adds up to more than a day, the instance is
  /// normalized.
  ///
  /// @tparam    T A 'second type' that is or can be converted to S
  /// @param[in] t The amount of T to add to the datetime instance.
  /// @throw       Does not throw
  /// 
  /// @note
  ///     - If the total amount of the 'second type' adds up to more than a
  ///       day, the instance is normalized.
  ///     - The parameter t should not be negative! If the total time of day
  ///       adds up to a negative number, the normalization my fail. If you
  ///       need to add a negative amount of seconds, use
  ///       datetime::remove_seconds
  ///
  /// @see datetime::remove_seconds
  /// @todo make sure the input parameter cannot be negative
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  constexpr void
  add_seconds(T t) noexcept
  {
    m_sec += static_cast<S>(t);
    this->normalize();
    return;
  }

  /// @brief Add a Modified Julian Day and any second type T to a datetime.
  ///
  /// This is actually like adding two datetime instances, only the second
  /// instance is given in parts (date and time).
  ///
  /// @tparam    T    A 'second type' that is or can be converted to S
  /// @param[in] days The (MJ)Days to add
  /// @param[in] secs The amount of T to add to the datetime instance.
  /// @throw          Does not throw
  /// 
  /// @note
  ///     - If the total amount of the 'second type' adds up to more than a
  ///       day, the instance is normalized.
  ///     - The parameter t should not be negative! If the total time of day
  ///       adds up to a negative number, the normalization my fail. If you
  ///       need to add a negative amount of seconds, use
  ///       datetime::remove_seconds
  ///
  /// @see datetime::remove_seconds.
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  constexpr datetime<S>
  add(modified_julian_day days, T secs) const noexcept
  { 
    datetime<S> new_dt { days+m_mjd, static_cast<S>(secs)+m_sec };
    new_dt.normalize();
    return new_dt;
  }
    
  /// Subtract any second type T, convertible to S.
  ///
  /// @tparam    T    A 'second type' that is or can be converted to S
  /// @param[in] t    The amount of T to subtract from the datetime instance.
  /// @throw          Does not throw
  ///
  /// Given a normalized intstance, after the operation, the instance will 
  /// still be normalized.
  template<class T,
    typename = std::enable_if_t<T::is_of_sec_type>,
    typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool>
               >
  constexpr void
  remove_seconds(T t) noexcept
  { 
    m_sec -= (S)t;
    this->normalize();
#ifdef USE_DATETIME_CHECKS
    assert(m_mjd >= modified_julian_day{0} && m_sec >= S{0});
#endif
    return;
  }

  /// Return the difference of two datetimes as second type S.
  ///
  /// The difference computed is: calling_instance - passed_instance, aka
  /// this - d
  constexpr S
  delta_sec(const datetime& d) const noexcept
  {
    return (m_sec - d.m_sec) + mjd_sec_diff<S>(m_mjd, d.m_mjd);
  }

  /// Overload equality operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator==(const datetime& d) const noexcept
  { return m_mjd == d.m_mjd && m_sec == d.m_sec; }
    
  /// Overload in-equality operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator!=(const datetime& d) const noexcept
  { return !(this->operator==(d)); }

  /// Overload ">" operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator>(const datetime& d) const noexcept
  { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec); }
    
  /// Overload ">=" operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator>=(const datetime& d) const noexcept
  { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec); }
    
  /// Overload "<" operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator<(const datetime& d) const noexcept
  { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec); }
  
  /// Overload "<=" operator.
  /// @warning Expects normalized datetimes.
  constexpr bool
  operator<=(const datetime& d) const noexcept
  { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec <= d.m_sec); }

  /// Add a time interval to a datetime instance.
  constexpr void
  operator+=(const datetime_interval<S>& dt) noexcept
  {
    m_mjd += dt.days();
    m_sec += dt.secs();
    this->normalize();
  }

  /// @brief Normalize a datetime instance.
  ///
  /// Split the date and time parts such that the time part is always less
  /// than one day (i.e. make it time-of-day) and positive (i.e.>=0).
  /// Remove whole days of from the time part and add them to the date part.
  ///
  inline constexpr void
  normalize() noexcept
  {
    typename S::underlying_type secs { m_sec.as_underlying_type() };
    m_mjd += secs / S::max_in_day;
    m_sec  = secs % S::max_in_day;
    return;
  }

  /// Difference between two dates in MJdays and S.
  /// d1.delta_date(d2) is d1-d2
  ///
  /// @bug needsmore documentation
  constexpr datetime_interval<S>
  delta_date(const datetime& d) const noexcept
  {
    datetime_interval<S> dt { m_mjd - d.m_mjd, m_sec - d.m_sec };
    dt.normalize();
    return dt;
  }

  /// Cast to double (i.e. fractional) Modified Julian Date.
  constexpr double
  as_mjd() const noexcept
  {
    return static_cast<double>(m_mjd.as_underlying_type())
            + m_sec.fractional_days();
  }

  /// Cast to year, month, day of month
  /// @warning Expects normalized instance.
  constexpr ymd_date
  as_ymd() const noexcept
  {return m_mjd.to_ymd();}

  /// Cast to year, day_of_year
  /// @warning Expects normalized instance.
  constexpr ydoy_date
  as_ydoy() const noexcept
  {return m_mjd.to_ydoy();}

  /// Cast to gps_week and Sec-Of-Week
  constexpr gps_week
  as_gps_wsow(long& sow) const noexcept
  {
    auto mjd = m_mjd.as_underlying_type();
    gps_week w {(mjd - jan61980)/7};
    seconds sec {m_sec}; 
    sow = ((mjd - jan61980) 
         - w.as_underlying_type()*7) * seconds::max_in_day
         + sec.as_underlying_type();
    return w;
  }
    
  /// Cast to gps_week and S-Of-Week
  constexpr gps_week
  as_gps_wsow(S& sow) const noexcept
  {
    auto mjd = m_mjd.as_underlying_type();
    gps_week w {(mjd - jan61980)/7};
    sow = S{((mjd-jan61980) 
        - w.as_underlying_type()*7) * S::max_in_day};
    sow += m_sec;
    return w;
  }

  /// Convert the time of day to hours, minutes, seconds and S
  /// @bug needs more documentation
  /// @todo change return type
  constexpr std::tuple<hours, minutes, seconds, long>
  as_hmsf() const noexcept
  { return m_sec.to_hmsf(); }

  std::string
  stringify() const
  {
    auto ymd { this->as_ymd() };
    auto hms { this->as_hmsf() };
    S st { std::get<3>(hms) };
    double fsec { st.as_underlying_type() / S::template sec_factor<double>() };
    std::ostringstream out;
    out << std::fixed << std::setprecision(9) 
        << (fsec + static_cast<double>(st.as_underlying_type()));

    return std::string {
                 std::to_string( ymd.__year.as_underlying_type() )
         + "/" + std::to_string( ymd.__month.as_underlying_type() )
         + "/" + std::to_string( ymd.__dom.as_underlying_type() )
         + " " + std::to_string( std::get<0>(hms).as_underlying_type() )
         + ":" + std::to_string( std::get<1>(hms).as_underlying_type() )
         //+ ":" + std::to_string( std::get<2>(hms).as_underlying_type() )
         + ":" + out.str()
    };
  }

private:
  modified_julian_day m_mjd;  ///< Modified Julian Day
  S                   m_sec;  ///< Time of day in S precision.

}; // end class datetime


/// Difference between two dates in MJdays and T.
/// Diff is dt1 - dt2
template<typename T, 
        typename = std::enable_if_t<T::is_of_sec_type>
        >
  constexpr datetime_interval<T>
  delta_date(const datetime<T>& dt1, const datetime<T>& dt2) noexcept
{
  return dt1.delta_date( dt2 );
}

/// Sec-Millisec-MicroSec of Week to Day of week
template<typename T,
        typename = std::enable_if_t<T::is_of_sec_type>
      >
  constexpr int
  day_of_week(T sow) noexcept
{
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
/// @see ngpt::dat
template<typename T,
        typename = std::enable_if_t<T::is_of_sec_type>
      >
  inline int
  dat(datetime<T> t) noexcept
{
  return ngpt::dat(t.mjd());
}

} // end namespace

#endif
