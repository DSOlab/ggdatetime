#ifndef __DTCALENDAR_NGPT__HPP__
#define __DTCALENDAR_NGPT__HPP__

#include "dtfund.hpp"
#ifdef DEBUG
    #include <iostream>
    #include <iomanip>  // std::setprecision
    #include <sstream>  // std::ostringstream
#endif

namespace ngpt {

/*
 * A datetime class. Holds (integral) days as MJD and fraction of day as any
 * of the is_of_sec_type class (i.e. seconds/milli/micro).
 */
template<class S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    class datetime {
public:

    /// Default (zero) constructor.
    explicit constexpr datetime() noexcept : m_mjd(0), m_sec(0) {};
    
    /// Constructor from year, month, day of month and sec type.
    explicit constexpr datetime(year y, month m, day_of_month d, S s)
    : m_mjd{cal2mjd(y, m, d)}, m_sec{s}
    {}

    /// Constructor from year, month, day of month and any sec type (T),
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
    
    /// Constructor from year, day of year and any sec type (T),
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
    /// micro- or milli- or seconds (if T can be cast to S).
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
    /// micro- or milli- or seconds (if T can be cast to S).
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

    /// Constructor from modified julian day, hours, minutes and 
    /// micro- or milli- or seconds.
    explicit
    datetime(modified_julian_day mjd, hours hr=hours(), minutes mn=minutes(),
        S sec=S())
    : m_mjd{mjd}, m_sec{hr, mn, sec}
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

    /// Get the MJDay.
    constexpr modified_julian_day  mjd() const noexcept { return m_mjd; }
    constexpr modified_julian_day& mjd() noexcept { return m_mjd; }

    #ifdef DEBUG
    /// Get the sec type:
    constexpr typename S::underlying_type secs() const noexcept
    { return m_sec.as_underlying_type(); }
    #endif

    /// Add any second type T, convertible to S.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr void add_seconds(T t) noexcept
    { 
        m_sec += (S)t;
        if ( m_sec.more_than_day() ) {
            this->normalize();
        }
        return;
    }

    /// Add an MJDay and any second type T, convertible to S to a datetime.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr datetime<S> add(modified_julian_day days, T secs)
        const noexcept
    { 
        datetime<S> new_dt { days+m_mjd, static_cast<S>(secs) + m_sec };
        new_dt.normalize();
        return new_dt;
    }
    
    /// Subtract any second type T, convertible to S.
    template<class T,
            typename = std::enable_if_t<T::is_of_sec_type>,
            typename = std::enable_if_t<
                std::is_same<S, decltype(static_cast<S>(T{}))>::value, bool
                >
            >
        constexpr void remove_seconds(T t) noexcept
    { 
        m_sec -= (S)t;
        if ( m_sec < (S)0 ) {
            while ( m_sec < (S)0 ) {
                --m_mjd;
                m_sec += (S)S::max_in_day;
            }
        }
        return;
    }

    /// Return the difference of two datetimes as second type S.
    constexpr S delta_sec(const datetime& d) const noexcept
    {
        return (m_sec - d.m_sec) + mjd_sec_diff<S>(m_mjd, d.m_mjd);
    }

    /// Overload equality operator.
    constexpr bool operator==(const datetime& d) const noexcept
    { return m_mjd == d.m_mjd && m_sec == d.m_sec; }

    /// Overload ">" operator.
    constexpr bool operator>(const datetime& d) const noexcept
    { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec > d.m_sec); }
    
    /// Overload ">=" operator.
    constexpr bool operator>=(const datetime& d) const noexcept
    { return m_mjd > d.m_mjd || (m_mjd == d.m_mjd && m_sec >= d.m_sec); }
    
    /// Overload "<" operator.
    constexpr bool operator<(const datetime& d) const noexcept
    { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec < d.m_sec); }
    
    /// Overload "<=" operator.
    constexpr bool operator<=(const datetime& d) const noexcept
    { return m_mjd < d.m_mjd || (m_mjd == d.m_mjd && m_sec <= d.m_sec); }

    /// Reset the seconds/milli/nano after removing whole days.
    constexpr void normalize() noexcept
    {
        // TODO what happens if m_sec < 0 ??
        modified_julian_day _add {static_cast<long>(m_sec.remove_days())};
        m_mjd += _add;
        return;
    }

    /// Difference between two dates in MJdays and *seconds.
    constexpr std::tuple<modified_julian_day, S>
    delta_date(const datetime& d) const noexcept
    {
        modified_julian_day::underlying_type t_mjd, sign{1};
        typename S::underlying_type t_secs;
        datetime<S> d1{*this}, d2{d};

        if (d2 > d1) {
            d1 = d;
            d2 = *this;
            sign = -1;
        }

        t_mjd = d1.m_mjd.as_underlying_type() - d2.m_mjd.as_underlying_type();
        t_secs= d1.m_sec.as_underlying_type() - d2.m_sec.as_underlying_type();
        
        if (t_secs < 0) {
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
    constexpr std::tuple<year, month, day_of_month>
    as_ymd() const noexcept
    {
        // TODO should i normalize the (this) date before calling to_ymd()
        year y;
        month m;
        day_of_month d;
        std::tie(y, m, d) = m_mjd.to_ymd();
        return std::make_tuple(y, m, d);
    }

    /// Cast to year, day_of_year
    constexpr std::tuple<year, day_of_year>
    as_ydoy() const noexcept
    {
        // TODO should i normalize the (this) date before calling to_ydoy()
        year y;
        day_of_year d;
        std::tie(y, d) = m_mjd.to_ydoy();
        return std::make_tuple(y, d);
    }

    /// Convert the *seconds to hours, minutes, seconds and S
    constexpr std::tuple<hours, minutes, seconds, long>
    as_hmsf() const noexcept { return m_sec.to_hmsf(); }

#ifdef DEBUG
    std::string stringify() const
    {
        auto ymd { this->as_ymd() };
        auto hms { this->as_hmsf() };
        S st { std::get<3>(hms) };
        double fsec { st.as_underlying_type() * S::template sec_ifactor<double>() };
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
    S                   m_sec;  ///< Fraction of day in milli/nano/seconds

}; // end class datetime


/// Difference between two dates in MJdays and *seconds.
template<typename T, 
        typename = std::enable_if_t<T::is_of_sec_type>
        >
    constexpr std::tuple<modified_julian_day, T> delta_date(const datetime<T>& dt1,
        const datetime<T>& dt2) noexcept
{
    return dt1.delta_date( dt2 );
}

} // end namespace

#endif // define DATETIME
