///
/// \file  dtcalendar.hpp
///
/// \brief A fundamental, template datetime class.
///
/// This file contains the definition (and implementation) of a datetime class
/// to be used for GNSS applications.
///
/// \author xanthos
///
/// \bug No known bugs.
///

#ifndef __DTCALENDAR_NGPT__HPP__
#define __DTCALENDAR_NGPT__HPP__

#include "dtfund.hpp"

#ifdef DEBUG
#include <iostream>
#include <iomanip>  // std::setprecision
#include <sstream>  // std::ostringstream
#endif

namespace ngpt
{

/// \brief A generic, templatized class to hold a datetime period/interval.
///
/// A datetime_interval represents a time (datetime) interval or period, i.e.
/// 5 days, 12 hours and 49 seconds. We assume a continuous time scale (no leap
/// seconds are taken into consideration --this is only an interval not an
/// actual datetime instance--).
///
/// A datetime_interval instance has two fundamental parts (members):
/// - a day part (i.e. holding the days), and 
/// - a time part (i.e. holding any type S i.e. any second type)
///
/// The purpose of this class is to work together with the datetime class.
///
/// \tparam S Any class of 'second type', i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true. This can be
///           ngpt::seconds, ngpt::milliseconds, ngpt::microseconds.
///
template<class S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    class datetime_interval {
public:
    explicit
    constexpr datetime_interval(modified_julian_day d, S s) noexcept
    : m_days{d},
      m_secs{s}
    {};

    explicit
    constexpr datetime_interval(std::tuple<modified_julian_day, S>&& t) noexcept
    : m_days{std::get<0>(t)},
      m_secs{std::get<1>(t)}
    {};

private:
    modified_julian_day m_days;
    S                   m_secs;
}; // end class datetime_interval

/// \brief A generic, templatized Date/Time class.
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
/// \tparam S Any class of 'second type', i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true. This can be
///           ngpt::seconds, ngpt::milliseconds, ngpt::microseconds.
///
/// \note Constructors can be called with the time part being more than one day;
///       (e.g. datetime<ngpt::seconds> d {year(2016), month(12), day_of_month(15), seconds(86401};)
///       If you think that this is a posibility, then call datetime::normalize
///       imidiately after the construction. An automatic call of the function
///       datetime::normalize could be added in the body of (nearly all)
///       constructors, but that would be time consuming. So, the option chosen
///       is **not**  to include a call to datetime::normalize and let the user
///       call the function if needed.
template<class S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    class datetime {
public:

    /// Expose the underlying sec type S
    using sec_type = S;

    /// Default (zero) constructor.
    explicit constexpr datetime() noexcept
    : m_mjd(ngpt::j2000_mjd), m_sec(0)
    {};
    
    /// Constructor from year, month, day of month and sec type.
    explicit constexpr datetime(year y, month m, day_of_month d, S s)
    : m_mjd{cal2mjd(y, m, d)}, m_sec{s}
    {}

    /// Constructor from year, month, day of month and any sec type T,
    /// convertible to S.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value,
                bool
                >
            >
        explicit datetime(year y, month m, day_of_month d, T t)
        : m_mjd{cal2mjd(y, m, d)}, m_sec{S(t)}
    {}
    
    /// Constructor from year, day of year and any sec type T,
    /// convertible to S.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value,
                bool
                >
            >
        explicit datetime(year y, day_of_year d, T t)
        : m_mjd{ydoy2mjd(y, d)}, m_sec{S(t)}
    {}
    
    /// Constructor from year, month, day of month, hours, minutes and
    /// any second type T convertible to S (i.e. T can be cast to S).
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value,
                bool
                >
            >
        explicit
        datetime(year y, month m, day_of_month d, hours hr, minutes mn, T sec)
        : m_mjd{cal2mjd(y, m, d)}, m_sec{hr, mn, S(sec)}
    {}
    
    /// Constructor from year, day of year, hours, minutes and
    /// any second type T convertible to S (i.e. T can be cast to S).
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value,
                bool
                >
            >
        explicit
        datetime(year y, day_of_year d, hours hr, minutes mn, T sec)
        : m_mjd{ydoy2mjd(y, d)}, m_sec{hr, mn, S(sec)}
    {}

    /// Constructor from year, month, day of month and fractional seconds.
    explicit
    datetime(year y, month m, day_of_month d, hours hr, minutes mn,
        double fsecs)
    : m_mjd{cal2mjd(y, m, d)}, m_sec{hr, mn, fsecs}
    {}
    
    /// Constructor from year, day of year and fractional seconds.
    explicit
    datetime(year y, day_of_year d, hours hr, minutes mn,
        double fsecs)
    : m_mjd{ydoy2mjd(y, d)}, m_sec{hr, mn, fsecs}
    {}
    
    /// Constructor from year, month, day of month, hours, minutes and
    /// second type S.
    explicit
    datetime(year y, month m, day_of_month d, hours hr=hours(),
        minutes mn=minutes(), S sec=S())
    : m_mjd{cal2mjd(y, m, d)}, m_sec{hr, mn, sec}
    {}
    
    /// Constructor from year, day of year, hours, minutes and
    /// second type S.
    explicit
    datetime(year y, day_of_year d, hours hr=hours(),
        minutes mn=minutes(), S sec=S())
    : m_mjd{ydoy2mjd(y, d)}, m_sec{hr, mn, sec}
    {}
    
    /// Constructor from modified julian day, hours, minutes and 
    /// second type S.
    explicit
    datetime(modified_julian_day mjd, hours hr=hours(), minutes mn=minutes(),
        S sec=S())
    : m_mjd{mjd}, m_sec{hr, mn, sec}
    {}
    
    /// Constructor from modified julian day, and second type S.
    explicit
    datetime(modified_julian_day mjd, S sec=S())
    : m_mjd{mjd}, m_sec{sec}
    {}

    /// Get the Modified Julian Day (const).
    constexpr modified_julian_day  mjd() const noexcept { return m_mjd; }
    
    /// Get the Modified Julian Day.
    constexpr modified_julian_day& mjd() noexcept { return m_mjd; }

    #ifdef DEBUG
    /// Get the sec type:
    constexpr typename S::underlying_type secs() const noexcept
    { return m_sec.as_underlying_type(); }
    #endif

    /// \brief Add any second type T, convertible to S.
    /// 
    /// Add to the datetime instance an amount of t 'second type'. If the total
    /// amount of the 'second type' adds up to more than a day, the instance is
    /// normalized.
    ///
    /// \tparam    T A 'second type' that is or can be converted to S
    /// \param[in] t The amount of T to add to the datetime instance.
    /// \throw       Does not throw
    /// 
    /// \note
    ///     - If the total amount of the 'second type' adds up to more than a
    ///       day, the instance is normalized.
    ///     - The parameter t should not be negative! If the total time of day
    ///       adds up to a negative number, the normalization my fail. If you
    ///       need to add a negative amount of seconds, use
    ///       datetime::remove_seconds
    ///
    /// \see datetime::remove_seconds
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr void add_seconds(T t) noexcept
    { 
        m_sec += static_cast<S>(t);
        if (m_sec.more_than_day()) this->normalize();
        return;
    }

    /// \brief Add a Modified Julian Day and any second type T to a datetime.
    ///
    /// This is actually like adding two datetime instances, only the second
    /// instance is given in parts (date and time).
    ///
    /// \tparam    T    A 'second type' that is or can be converted to S
    /// \param[in] days The (MJ)Days to add
    /// \param[in] secs The amount of T to add to the datetime instance.
    /// \throw          Does not throw
    /// 
    /// \note
    ///     - If the total amount of the 'second type' adds up to more than a
    ///       day, the instance is normalized.
    ///     - The parameter t should not be negative! If the total time of day
    ///       adds up to a negative number, the normalization my fail. If you
    ///       need to add a negative amount of seconds, use
    ///       datetime::remove_seconds
    ///
    /// \see datetime::remove_seconds.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr datetime<S> add(modified_julian_day days, T secs)
        const noexcept
    { 
        datetime<S> new_dt { days+m_mjd, static_cast<S>(secs)+m_sec };
        new_dt.normalize();
        return new_dt;
    }
    
    /// Subtract any second type T, convertible to S.
    ///
    /// \tparam    T    A 'second type' that is or can be converted to S
    /// \param[in] t    The amount of T to subtract from the datetime instance.
    /// \throw          Does not throw
    ///
    /// Given a normalized intstance, after the operation, the instance will 
    /// still be normalized.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr void remove_seconds(T t) noexcept
    { 
        m_sec -= (S)t;
        while ( m_sec < (S)0 ) {
            --m_mjd;
            m_sec += (S)S::max_in_day;
        }
        return;
    }

    /// Return the difference of two datetimes as second type S. The difference
    /// is: calling_instance - passed_instance.
    constexpr S delta_sec(const datetime& d) const noexcept
    {
        return (m_sec - d.m_sec) + mjd_sec_diff<S>(m_mjd, d.m_mjd);
    }

    /// Overload equality operator.
    /// \warning Expects normalized datetimes.
    constexpr bool operator==(const datetime& d) const noexcept
    { return m_mjd == d.m_mjd && m_sec == d.m_sec; }

    /// Overload ">" operator.
    /// \warning Expects normalized datetimes.
    constexpr bool operator>(const datetime& d) const noexcept
    { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec); }
    
    /// Overload ">=" operator.
    /// \warning Expects normalized datetimes.
    constexpr bool operator>=(const datetime& d) const noexcept
    { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec); }
    
    /// Overload "<" operator.
    /// \warning Expects normalized datetimes.
    constexpr bool operator<(const datetime& d) const noexcept
    { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec); }
    
    /// Overload "<=" operator.
    /// \warning Expects normalized datetimes.
    constexpr bool operator<=(const datetime& d) const noexcept
    { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec <= d.m_sec); }

    /// \brief Normalize a datetime instance.
    ///
    /// Split the date and time parts such that the time part is always less
    /// than one day (i.e. make it time-of-day).
    /// Remove whole days of from the time part and add them to the date part.
    ///
    constexpr void normalize() noexcept
    {
        typename S::underlying_type secs { m_sec.as_underlying_type() };

        if (secs > 0 && secs < S::max_in_day) {
            return;
        } else if ( secs >= S::max_in_day ) {
            modified_julian_day _add {static_cast<long>(m_sec.remove_days())};
            m_mjd += _add;
            return;
        } else {
            modified_julian_day one_day{1};
            while (secs < 0) {
                secs += S::max_in_day;
                m_mjd -= one_day;
            }
            m_sec = static_cast<S>(secs);
        }
    }

    /// Difference between two dates in MJdays and S.
    /// \bug needsmore documentation
    constexpr std::tuple<modified_julian_day, S>
    delta_date(const datetime& d) const noexcept
    {
        modified_julian_day::underlying_type t_mjd{0}, sign{1};
        typename S::underlying_type t_secs{0};
        datetime<S> d1{*this}, d2{d};

        if (d2 > d1) {
            d1 = d;
            d2 = *this;
            sign = -1;
        }

        t_mjd = d1.m_mjd.as_underlying_type() - d2.m_mjd.as_underlying_type();
        t_secs= d1.m_sec.as_underlying_type() - d2.m_sec.as_underlying_type();
        
        while (t_secs < 0) {
            t_secs += S::max_in_day;
            t_mjd  -= 1;
        }
        t_mjd *= sign;
        
        return std::make_tuple(modified_julian_day{t_mjd}, S{t_secs});
    }

    /// Cast to double (i.e. fractional) Modified Julian Date.
    constexpr double as_mjd() const noexcept
    {
        return static_cast<double>(m_mjd.as_underlying_type())
            + m_sec.fractional_days();
    }

    /// Cast to year, month, day of month
    /// \bug should i normalize the (this) date before calling to_ymd()
    /// \warning Expects normalized instance.
    constexpr std::tuple<year, month, day_of_month>
    as_ymd() const noexcept
    {
        year y;
        month m;
        day_of_month d;
        std::tie(y, m, d) = m_mjd.to_ymd();
        return std::make_tuple(y, m, d);
    }

    /// Cast to year, day_of_year
    /// \bug should i normalize the (this) date before calling to_ymd()
    /// \warning Expects normalized instance.
    constexpr std::tuple<year, day_of_year>
    as_ydoy() const noexcept
    {
        year y;
        day_of_year d;
        std::tie(y, d) = m_mjd.to_ydoy();
        return std::make_tuple(y, d);
    }

    /// Convert the time of day to hours, minutes, seconds and S
    constexpr std::tuple<hours, minutes, seconds, long>
    as_hmsf() const noexcept { return m_sec.to_hmsf(); }

#ifdef DEBUG
    std::string stringify() const
    {
        auto ymd { this->as_ymd() };
        auto hms { this->as_hmsf() };
        S st { std::get<3>(hms) };
        double fsec { st.as_underlying_type() / S::template sec_factor<double>() };
        std::ostringstream out;
        out << std::fixed << std::setprecision(9) << fsec;

        return std::string {
                     std::to_string( std::get<0>(ymd).as_underlying_type() )
             + "/" + std::to_string( std::get<1>(ymd).as_underlying_type() )
             + "/" + std::to_string( std::get<2>(ymd).as_underlying_type() )
             + " " + std::to_string( std::get<0>(hms).as_underlying_type() )
             + ":" + std::to_string( std::get<1>(hms).as_underlying_type() )
             + ":" + std::to_string( std::get<2>(hms).as_underlying_type() )
             + "+" + out.str()
        };
    }
#endif

private:
    modified_julian_day m_mjd;  ///< Modified Julian Day
    S                   m_sec;  ///< Time of day in S precision.

}; // end class datetime


/// Difference between two dates in MJdays and T.
template<typename T, 
        typename = std::enable_if_t<T::is_of_sec_type>
        >
    constexpr std::tuple<modified_julian_day, T> delta_date(const datetime<T>& dt1,
        const datetime<T>& dt2) noexcept
{
    return dt1.delta_date( dt2 );
}

} // end namespace

#endif
