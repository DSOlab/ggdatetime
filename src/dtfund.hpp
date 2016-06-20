///
/// \file  dtfund.hpp
///
/// \brief Fundamental (core) datetime constants and algorithms.
///
/// This file contains fundamental constants and algorithms for manipulating
/// date and time, targeting GNSS applications.
///
/// \author xanthos
///
/// \bug No known bugs.
///

#ifndef __DTFUND_NGPT__HPP__
#define __DTFUND_NGPT__HPP__

#include <ostream>
#include <iomanip>
#include <cstdio>
#include <limits>
#include <cassert>
#include <cmath>
#include <tuple>
#include <cstring>
#include <string>

#ifdef DEBUG
#include <iostream>
#endif


namespace ngpt
{

/// Check if long is big enough to hold two days in microseconds.
static_assert( 86400L  * 1000000L * 2 < std::numeric_limits<long>::max(),
    "FUCK! Long is not big enough to hold two days in microseconds" );

/// Jan 1st 1980
constexpr long jan61980 { 44244L };

constexpr long jan11901 { 15385L };

/// Seconds per day.
constexpr double sec_per_day { 86400.0e0 };

/// Days per Julian year.
constexpr double days_in_julian_year { 365.25e0 };

/// Days per Julian century.
constexpr double days_in_julian_cent { 36525.0e0 };

/// Reference epoch (J2000.0), Julian Date.
constexpr double j2000_jd { 2451545.0e0 };

/// Reference epoch (J2000.0), Modified Julian Date.
constexpr double j2000_mjd { 51544.5e0 };

/// Julian Date of Modified Julian Date zero.
constexpr double mjd0_jd { 2400000.5e0 };

/// TT minus TAI in seconds.
constexpr double tt_minus_tai { 32.184e0 };

/// \brief Calendar date to Modified Julian Day.
///
/// \param[in] iy The year.
/// \param[in] im The month.
/// \param[in] id The day of month.
/// \return    The Modified Julian Date (as \c long).
///
/// \throw A runtime_error if the month and/or day is invalid.
///
/// Reference: iauCal2jd
///
long cal2mjd(int iy, int im, int id);

/// Convert hours, minutes, seconds into fractional days.
/// double hms2fd(int, int, double) noexcept;

/// Decompose fractional days to hours, minutes, seconds and fractional seconds
/// with a given precision.
/// void fd2hms(double, int, int ihmsf[4]);

/// \brief Check if year is leap.
///
/// \param[in] iy The year to check.
/// \return true if year is leap, false otherwise.
///
/// \throw Does not throw.
///
inline bool is_leap(int iy) noexcept
{ return !(iy%4) && (iy%100 || !(iy%400)); }

/// Forward declerations
class year;
class month;
class day_of_month;
class day_of_year;
class modified_julian_day;
class hours;
class minutes;
class seconds;
class milliseconds;
class microseconds;

/// \brief Transform a calendar date to modified_julian_day.
///
/// \param[in] y The year.
/// \param[in] m The month.
/// \param[in] d The day of month.
/// \return      The corresponding Modified Julian Day
/// 
/// \throw       This function will throw if ngpt::cal2mjd throws, i.e. if
///              the input date is invalid.
///
modified_julian_day cal2mjd(year, month, day_of_month);

/// Convert a pair of Year, Day of year toMJDay.
modified_julian_day ydoy2mjd(year, day_of_year) noexcept;

/// \brief A wrapper class for years.
///
/// A year is represented by just an integer number. There are no limits
/// (excpept from intger overflow) to the range of the year (integer).
///
class year
{
public:
    /// Years are represented as integers.
    typedef int underlying_type;

    /// Constructor; default year is 0.
    explicit constexpr year (underlying_type i=0) noexcept : m_year(i) {};

    /// Get the year as year::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_year; }

private:
    /// The year as underlying type.
    underlying_type m_year;

}; // class year

/// \brief A wrapper class for months.
///
/// A month is represented by just an integer number. There are no limits
/// (excpept from intger overflow) to the range of the month (integer), i.e. 
/// the month is not check (by default) to be in the range [1,12]. So, a user
/// can construct a month from whatever integer, e.g. :
/// month m (200);
/// will work fine! To check if the month is within the range [1,12], use the
/// month::is_valid method.
/// Most functions (within ngpt) accept months in the range [1,12]; do not
/// use the range [0,11], except if you realy know what you're doing.
///
class month
{
public:
    /// Months are represented as int.
    typedef int underlying_type;

    /// Constructor; default month is 0.
    explicit constexpr month(underlying_type i=1) noexcept : m_month(i) {};

    /// \brief Constructor from c-string.
    ///
    /// \param[in] str The month's name; The 3 first characters should match
    ///                a month in the month::short_names array.
    /// \throw         An std::invalid_argument exception is thrown if a) no
    ///                match is found, or b) the input string is too short.
    ///
    explicit month(const char* str);

    /// Get the month as month::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_month; }

    /// Return the corresponding short name (i.e. 3-char name) e.g. "Jan".
    const char* short_name() const noexcept { return short_names[m_month-1]; }

    /// Return the corresponding long name (i.e. normal month name) e.g. "January".
    const char* long_name() const noexcept { return long_names[m_month-1]; }

    /// Check if the month is within the interval [1,12].
    bool is_valid() const noexcept { return m_month > 0 && m_month <= 12; }

private:
    /// \brief Decleration of short month names. 
    /// \note  Note that we do need a definition in the .cpp file.
    constexpr static const char* short_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    /// \brief  Decleration of long month names.
    /// \note   Note that we do need a definition in the .cpp file.
    constexpr static const char* long_names[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    /// The month as underlying_type.
    underlying_type m_month;

}; // class month

/// \brief A wrapper class for day of month.
/// 
/// A day_of_month is just an integer, representing the day of month. No limits
/// are set though, so the user can construct a day_of_month from whatever 
/// integer. inluding negative numbers!
///
/// \todo Provide a day_of_month::validate method.
///
class day_of_month
{
public:
    /// Days are represented as ints.
    typedef int underlying_type;
    
    /// Constructor; default day of month is 1.
    explicit constexpr day_of_month(underlying_type i=1) noexcept : m_dom(i) {};
    
    /// Get the day_of_month as day_of_month::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_dom; }

private:
    /// The day of month as underlying_type.
    underlying_type m_dom;

}; // class day_of_month

/// \brief A wrapper class for Modified Julian Day.
///
/// A Modified Julian Day is represented by a long integer (there is no
/// fractional part). Thus, a modified_julian_day only represents a date *not*
/// a datetime.
///
class modified_julian_day
{
public:
    /// MJDs are represented as long ints.
    typedef long underlying_type;
    
    /// Constructor; default Modified Julian Day is 0.
    explicit constexpr modified_julian_day(underlying_type i=0) noexcept
    : m_mjd(i) 
    {};
    
    /// \brief Constructor from Year and DayOfYear.
    /// 
    /// \param[in] iy The year.
    /// \param[in] id The day of year.
    /// \throw Does not throw.
    ///
    /// \see "Remondi Date/Time Algorithms", http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    ///
    explicit constexpr modified_julian_day(year iy, day_of_year id) noexcept;
    
    /// Get the modified_julian_day as modified_julian_day::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_mjd; }
    
    /// Define addition (between MJDs).
    constexpr void operator+=(const modified_julian_day& d) noexcept
    { m_mjd += d.m_mjd; }
    
    /// Operator - (subtraction).
    constexpr modified_julian_day
    operator-(const modified_julian_day& mjd) const noexcept
    { return modified_julian_day{m_mjd-mjd.m_mjd}; }
    
    /// Operator + (addition).
    constexpr modified_julian_day
    operator+(const modified_julian_day& mjd) const noexcept
    { return modified_julian_day{m_mjd+mjd.m_mjd}; }

    /// Operator == (equality).
    constexpr bool operator==(const modified_julian_day& d) const noexcept
    { return m_mjd == d.m_mjd; }

    /// Operator > (greater than).
    constexpr bool operator>(const modified_julian_day& d) const noexcept
    { return m_mjd > d.m_mjd; }

    /// Operator >= (greater or equal to).
    constexpr bool operator>=(const modified_julian_day& d) const noexcept
    { return m_mjd >= d.m_mjd; }

    /// Operator < (less than).
    constexpr bool operator<(const modified_julian_day& d) const noexcept
    { return m_mjd < d.m_mjd; }

    /// Operator <= (less or equal to).
    constexpr bool operator<=(const modified_julian_day& d) const noexcept
    { return m_mjd <= d.m_mjd; }

    /// Operator -- (minus 1 mjd).
    constexpr modified_julian_day& operator--() noexcept
    {
        --m_mjd;
        return *this;
    }

    /// \brief Convert a Modified Julian Day to Year and Day of year.
    ///
    /// \return A tuple with two elements: (year, day_of_year)
    /// \throw  Does not throw.
    ///
    /// \see    "Remondi Date/Time Algorithms", http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    ///
    std::tuple<year, day_of_year> to_ydoy() const noexcept;
    
    /// \brief Convert a Modified Julian Day to Calendar Date.
    ///
    /// \return A tuple with three elements: (year, month, day_of_month)
    /// \throw  Does not throw.
    ///
    /// \see    "Remondi Date/Time Algorithms", http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    ///
    std::tuple<year, month, day_of_month> to_ymd() const noexcept;
    
private:
    /// The modified julian day as underlying type.
    underlying_type m_mjd;

}; // class modified_julian_day

/// \brief A wrapper class for day of year.
///
/// A day of year (doy) is represented by an integer; any integer will do, no
/// automatic check is performed to see if the doy is within the range 
/// [1,365/6]. In fact, one can actually use a negative integer to represent
/// a doy. It the user wishes to check wether the given doy is indeed valid,
/// then the method day_of_year::validate() can be used.
///
/// \todo Provide a day_of_year::validate method.
///
class day_of_year
{
public:
    /// Day of year represented as int.
    typedef int underlying_type;
    
    /// Constructor; default day of year is 1.
    explicit constexpr day_of_year(underlying_type i=1) noexcept : m_doy(i) {};
    
    /// Cast to underlying type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_doy; }

private:
    /// The day_of_year as day_of_year::underlying_type.
    underlying_type m_doy;   
};

/// \brief A wrapper class for hours.
///
/// An hour is just an integer number representing hours; there is no
/// 'valid range' for hours, meaning that any integer value will do!
/// No checks are provided for hours cause, well, i don't know what to check.
/// Note however, that hours are always integer; there is *no fractional part*.
/// hours are designed to be part of a (date)time system of type 
/// hours/minutes/(milli|nano|...)seconds, so they only represent integeral hours
/// -- normally hour of day, although no such restriction exists --.
/// In case a subdivision is needed (e.g. minutes, seconds, etc), then use
/// the corresponsing classes (nngpt::minutes, ngpt::seconds, etc...).
class hours
{
public:
    /// Hours are represented by ints.
    typedef int underlying_type;
    
    /// Constructor; default hours is 0.
    explicit constexpr hours(underlying_type i=0) noexcept : m_hours(i) {};
    
    /// Get the hours as hours::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_hours; }

private:
    /// The hours as hours::underlying_type
    underlying_type m_hours;

}; // class hours

/// \brief A wrapper class for minutes.
///
/// A minutes is just an integer number representing minutes; there is no
/// 'valid range', meaning that any integer value will do!
/// No checks are provided (for the minute range) cause, well, i don't know 
/// what to check.
/// Note however, that minutes are always integer; there is *no fractional part*.
/// They are designed to be part of a (date)time system of type 
/// hours/minutes/(milli|nano|...)seconds, so they only represent integeral
/// minutes -- normally min of hours, although no such restriction exists --.
/// In case a subdivision is needed (e.g. seconds, milliseconds etc), then use
/// the corresponsing classes (ngpt::seconds, ngpt::milliseconds, etc...).
class minutes
{
public:
    /// Minutes are represented by ints
    typedef int underlying_type;
    
    /// Constructor
    explicit constexpr minutes(underlying_type i=0) noexcept : m_min(i) {};

    /// Get the minutes as minutes::underlying_type
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_min; }

private:
    /// The minutes as underlying type.
    underlying_type m_min;

}; // class minutes

/// \brief A wrapper class for seconds.
///
/// seconds is just a wrapper class around long integer numbers, i.e. a second
/// is just a long int and can be either positive or negative. Users are however
/// restricted by integer overflow. The maximum number of days that can be
/// expressed in seconds without fear of overflow is given by the template 
/// function ngpt::max_days_allowed.
/// Negative seconds are allowed (so that a user can perform basic operations
/// like e.g. addition), but some functions expect only positive seconds
/// (seconds::remove_days, seconds::to_days).
/// seconds is a class which represents a second subdivision (as is 
/// ngpt::milliseconds, ngpt::microseconds, etc); quite a few methods should be
/// common to all of these classes, all of which have a member variable 
/// seconds::is_of_sec_type which is set to true.
///
/// \see ngpt::milliseconds
/// \see ngpt::microseconds
class seconds {
public:
    /// Seconds are represented as long ints.
    typedef long underlying_type;
    
    /// Seconds is a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    
    /// Seconds in day.
    static constexpr underlying_type max_in_day { 86400L };

    /// The scale factor to transform from seconds to seconds.
    template<typename T>
        static constexpr T sec_factor() noexcept
    { return static_cast<T>(1); }

    /// Constructor; default seconds is 0.
    explicit constexpr seconds(underlying_type i=0L) noexcept : m_sec(i) {};

    /// Constructor from hours, minutes, seconds.
    explicit constexpr seconds(hours h, minutes m, seconds c) noexcept
    : m_sec {  c.as_underlying_type()
             + m.as_underlying_type()*60L
             + h.as_underlying_type()*3600L}
    {}

    /// Constructor from hours, minutes, fractional seconds.
    ///
    /// \note Only the integral part of the input seconds (fs) is considered;
    ///       the fractional part is ommited.
    explicit constexpr seconds(hours h, minutes m, double fs) noexcept
    : m_sec{  static_cast<long>(fs)
            + m.as_underlying_type()*60L
            + h.as_underlying_type()*3600L}
    {}
    
    /// Addition operator between seconds.
    constexpr void operator+=(const seconds& sc) noexcept { m_sec+=sc.m_sec; }

    /// Subtraction operator between seconds.
    constexpr void operator-=(const seconds& sc) noexcept { m_sec-=sc.m_sec; }
    
    /// Overload - operator (subtraction).
    constexpr seconds operator-(const seconds& n) const noexcept
    { return seconds{m_sec - n.m_sec}; }

    /// Overload + operator (addition).
    constexpr seconds operator+(const seconds& sec) const noexcept
    { return seconds{m_sec+sec.m_sec}; }

    /// Overload operator '/'
    /// \note This is integer division; no fractional part considered.
    constexpr seconds operator/(const seconds& n) const noexcept
    { return seconds{m_sec/n.m_sec}; }
  
    /// Equality operator.
    constexpr bool operator==(const seconds& d) const noexcept
    { return m_sec == d.m_sec; }

    /// Greater than operator.
    constexpr bool operator>(const seconds& d) const noexcept
    { return m_sec > d.m_sec; }

    /// Greater or equal to operator.
    constexpr bool operator>=(const seconds& d) const noexcept
    { return m_sec >= d.m_sec; }
    
    /// Less than operator.
    constexpr bool operator<(const seconds& d) const noexcept
    { return m_sec < d.m_sec; }

    /// Less or equal to operator.
    constexpr bool operator<=(const seconds& d) const noexcept
    { return m_sec <= d.m_sec; }
    
    /// Do the secods sum up to more than one day?
    constexpr bool more_than_day() const noexcept { return m_sec>max_in_day; }
    
    /// Get the seconds as seconds::underlying_type .
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_sec; }
    
    /// \brief Normalize seconds and return the integeral days.
    ///
    /// If the seconds sum up to more (or equal to) one day, remove the integer
    /// days and return them as integer; reset the seconds to seconds of the
    /// new day.
    ///
    /// \return The integer number of days (if the seconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative seconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int remove_days() noexcept
    {
        assert( m_sec >= 0 );
        int d { static_cast<int>(m_sec/max_in_day) };
        m_sec%=max_in_day;
        return d;
    }
    
    /// \brief Cast to days.
    ///
    /// If the seconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// \return The integer number of days (if the seconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative seconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int to_days() const noexcept
    {
        assert( m_sec >= 0 );
        return static_cast<int>(m_sec/max_in_day);
    }
    
    /// \brief Seconds as fractional days.
    ///
    /// Interpret (cast) the seconds as fractional days; returns a double.
    constexpr double fractional_days() const noexcept
    {
        return static_cast<double>(m_sec)/static_cast<double>(max_in_day);
    }
    
    /// Cast to double (i.e. fractional seconds).
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(m_sec); }

    /// Translate to hours, minutes, seconds and fractional seconds
    /// \bug needs mor documentation
    constexpr std::tuple<hours, minutes, seconds, long>
    to_hmsf() const noexcept
    {
        return std::make_tuple(hours  {static_cast<int>(m_sec/3600L)},
                               minutes{static_cast<int>((m_sec%3600L)/60L)},
                               seconds{(m_sec%3600L)%60L},
                               0L);
    }

private:
    /// Cast to any arithmetic type.
    template<typename T,
             typename=std::enable_if_t<std::is_arithmetic<T>::value>
             >
        constexpr T cast_to() const noexcept
    { return static_cast<T>(m_sec); }

    /// The seconds as underlying type.
    underlying_type m_sec;

}; // class seconds

/// \brief A wrapper class for milliseconds (i.e. 10**-3 sec).
/// 
/// milliseconds is just a wrapper class around long integer numbers, i.e. a
/// millisecond is just a long int and can be either positive or negative. 
/// Users are however restricted by integer overflow. The maximum number of days
/// that can be expressed in milliseconds without fear of overflow is given by
/// the template function ngpt::max_days_allowed.
/// Negative milliseconds are allowed (so that a user can perform basic 
/// operations like e.g. addition), but some functions expect only positive 
/// milliseconds (milliseconds::remove_days, milliseconds::to_days).
/// milliseconds is a class which represents a second subdivision (as is 
/// ngpt::seconds, ngpt::microseconds, etc); quite a few methods should be 
/// common to all of these classes, all of which have a member variable
/// milliseconds::is_of_sec_type which is set to true.
/// 
/// \note milliseconds can be cast to ngpt::seconds (via a static_cast or
/// a C-type cast) but the opposite is not true; i.e. ngpt::seconds cannot be
/// cast to milliseconds. This is still an open question!
///
/// \see ngpt::seconds
/// \see ngpt::microseconds
///
class milliseconds
{
public:
    /// MilliSeconds are represented as long ints.
    typedef long underlying_type;
    
    /// MilliSeconds are a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    
    /// Max milliseconds in one day.
    static constexpr long max_in_day { 86400L * 1000L };

    /// The scale factor to transform from seconds to milliseconds.
    template<typename T>
        static constexpr T sec_factor() noexcept
    { return static_cast<T>(1000); }

    /// Constructor; default milliseconds is 0.
    explicit constexpr milliseconds(underlying_type i=0L) noexcept
    : m_msec(i)
    {};
 
    /// Constructor from hours, minutes, milliseconds.   
    explicit constexpr milliseconds(hours h, minutes m, milliseconds c) noexcept
    : m_msec { c.as_underlying_type()
        + m.as_underlying_type()*60L  * sec_factor<long>()
        + h.as_underlying_type()*3600L* sec_factor<long>()}
    {}
    
    /// \brief Constructor from hours, minutes, fractional seconds.
    ///
    /// Note that the fractional seconds are only considered with millisecond
    /// precision; i.e. anything smaller than 10e-3 will not be considered (for
    /// the fs input argument).
    explicit constexpr milliseconds(hours h, minutes m, double fs) noexcept
    : m_msec{ static_cast<long>(fs * sec_factor<double>())
        + (m.as_underlying_type()*60L
        + h.as_underlying_type()*3600L) * sec_factor<long>()}
    {}
    
    /// \brief Cast to ngpt::seconds.
    /// Milliseconds can be cast to seconds (with a loss of precission).
    constexpr explicit operator seconds() const
    { return seconds{m_msec / sec_factor<long>()}; }
    
    /// Addition operator.
    constexpr milliseconds operator+(const milliseconds& sec) const noexcept
    { return milliseconds{m_msec+sec.m_msec}; }
    
    /// Addition operator.
    constexpr void operator+=(const milliseconds& ms) noexcept
    { m_msec+=ms.m_msec; }

    /// Subtraction operator.
    constexpr void operator-=(const milliseconds& ms) noexcept
    { m_msec-=ms.m_msec; }
    
    /// Subtraction operator.
    constexpr milliseconds operator-(const milliseconds& n) const noexcept
    { return milliseconds{m_msec - n.m_msec}; }
    
    /// Division operator between milliseconds.
    constexpr milliseconds operator/(const milliseconds& sc) noexcept
    { return milliseconds{m_msec/sc.m_msec}; }
    
    /// Equality operator.
    constexpr bool operator==(const milliseconds& d) const noexcept
    { return m_msec == d.m_msec; }

    /// Greater than operator.
    constexpr bool operator>(const milliseconds& d) const noexcept
    { return m_msec > d.m_msec; }

    /// Greater or equal to operator.
    constexpr bool operator>=(const milliseconds& d) const noexcept
    { return m_msec >= d.m_msec; }

    /// Less than operator.
    constexpr bool operator<(const milliseconds& d) const noexcept
    { return m_msec < d.m_msec; }

    /// Less or equal to operator.
    constexpr bool operator<=(const milliseconds& d) const noexcept
    { return m_msec <= d.m_msec; }
    
    /// Do the milliseconds sum up to more than one day ?
    constexpr bool more_than_day() const noexcept { return m_msec>max_in_day; }
    
    /// Get the milliseconds as milliseconds::underlying_type.
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_msec; }
    
    /// If the milliseconds sum up to more (or equal to) one day, remove the 
    /// integral days (and return them); reset the milliseconds to milliseconds
    /// of the new day.

    /// \brief Normalize milliseconds and return the integral days.
    ///
    /// If the milliseconds sum up to more (or equal to) one day, remove the
    /// integer days and return them as integer; reset the milliseconds to 
    /// milliseconds of the new day.
    ///
    /// \return The integer number of days (if the milliseconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative milliseconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int remove_days() noexcept
    {
        assert( m_msec >= 0 );
        int day { static_cast<int>(m_msec/max_in_day) };
        m_msec %= max_in_day;
        return day;
    }
    
    /// \brief Cast to days.
    ///
    /// If the milliseconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// \return The integer number of days (if the milliseconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative milliseconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int to_days() const noexcept
    {
        assert( m_msec >= 0 );
        return int{static_cast<int>(m_msec/max_in_day)};
    }
    
    /// Cast to fractional days.
    constexpr double fractional_days() const noexcept
    {
        return static_cast<double>(m_msec)/static_cast<double>(max_in_day);
    }
    
    /// Cast to fractional ngpt::seconds
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(m_msec)*1.0e-3; }
    
    /// \brief Resolve to (integer) seconds and fractional seconds.
    ///
    /// Split the milliseconds to ngpt::seconds and fractional seconds.
    ///
    /// \param[out] fraction Fractional seconds
    /// \return              Integral seconds
    /// \throw               Does not throw
    ///
    /// \note Only works for positive milliseconds.
    constexpr seconds resolve_sec(double& fraction) const noexcept
    {
        assert( m_msec >= 0);
        seconds sec { m_msec / sec_factor<long>() };
        fraction = static_cast<double>(m_msec % sec_factor<long>())*1e-3;
        return sec;
    }
    
    /// Translate to hours, minutes, seconds and milliseconds
    /// \bug need more documentation
    constexpr std::tuple<hours, minutes, seconds, long>
    to_hmsf() const noexcept
    {
        long hr { m_msec/3600000L                  };  // hours
        long mn { (m_msec%3600000L)/60000L         };  // minutes
        long sc { ((m_msec%3600000L)%60000L)/1000L };  // seconds
        long ms { m_msec-((hr*60L+mn)*60L+sc)*1000L};  // milliseconds.
        
        return std::make_tuple( hours  { static_cast<hours::underlying_type>(hr) },
                                minutes{ static_cast<minutes::underlying_type>(mn) },
                                seconds{ sc },
                                ms );
    }

private:
    /// Cast to any arithmetic type.
    template<typename T,
             typename=std::enable_if_t<std::is_arithmetic<T>::value>
             >
    constexpr T cast_to() const noexcept
    { return static_cast<T>( m_msec ); }

    /// Milliseconds as underlying type.
    underlying_type m_msec;

}; /// class milliseconds

/// \brief A wrapper class for microseconds (i.e 10**-6 sec.).
/// 
/// microseconds is just a wrapper class around long integer numbers, i.e. a
/// microsecond is just a long int and can be either positive or negative. 
/// Users are however restricted by integer overflow. The maximum number of days
/// that can be expressed in microseconds without fear of overflow is given by
/// the template function ngpt::max_days_allowed.
/// Negative microseconds are allowed (so that a user can perform basic 
/// operations like e.g. addition), but some functions expect only positive 
/// microiseconds (microseconds::remove_days, microseconds::to_days).
/// microseconds is a class which represents a second subdivision (as is 
/// ngpt::seconds, ngpt::milliseconds, etc); quite a few methods should be 
/// common to all of these classes, all of which have a member variable
/// microseconds::is_of_sec_type which is set to true.
/// 
/// \note microseconds can be cast to ngpt::seconds and ngpt::milliseconds (via
/// a static_cast or a C-type cast) but the opposite is not true; i.e. 
/// ngpt::seconds cannot be cast to microseconds.This is still an open question!
///
/// \see ngpt::seconds
/// \see ngpt::milliseconds
///
class microseconds
{
public:
    /// Microseconds are represented as long integers.
    typedef long underlying_type;
    
    /// Microseconds is a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    
    /// Max microseconds in day.
    static constexpr long max_in_day { 86400L * 1000000L };

    /// The scale factor to transform from seconds to microseconds.
    template<typename T>
        static constexpr T sec_factor() noexcept
    { return static_cast<T>(1000000); }

    /// Constructor; default microseconds is 0.
    explicit constexpr microseconds(underlying_type i=0L) noexcept
    : m_msec(i) {};
    
    /// Constructor from hours, minutes, microseconds.
    explicit constexpr microseconds(hours h, minutes m, microseconds c) noexcept
    : m_msec { c.as_underlying_type()
            +(m.as_underlying_type()*60L
            + h.as_underlying_type()*3600L) * sec_factor<long>() }
    {}
    
    /// Constructor from hours, minutes, fractional seconds.
    /// \note Fractional seconds with precission larger than microseconds are
    ///       not taken into account; i.e. we only consider the fractiona part
    ///       up to 10e-6.
    explicit constexpr microseconds(hours h, minutes m, double fs) noexcept
    : m_msec{ static_cast<long>(fs * sec_factor<double>())
        + (m.as_underlying_type()*60L
        + h.as_underlying_type()*3600L) * sec_factor<long>() }
    {}
    
    /// Microseconds can be cast to milliseconds will a loss of accuracy.
    constexpr explicit operator milliseconds() const
    { return milliseconds(m_msec / 1000L); }
    
    /// Microseconds can be cast to seconds will a loss of accuracy.
    constexpr explicit operator seconds() const
    { return seconds(m_msec / sec_factor<long>()); }
    
    /// Addition between microseconds.
    constexpr void operator+=(const microseconds& ns) noexcept
    { m_msec+=ns.m_msec; }

    /// Subtraction between microseconds.
    constexpr void operator-=(const microseconds& ns) noexcept
    { m_msec-=ns.m_msec; }

    /// Addition between microseconds.
    constexpr microseconds operator+(const microseconds& sec) const noexcept
    { return microseconds{m_msec+sec.m_msec}; }

    /// Subtraction between microseconds.
    constexpr microseconds operator-(const microseconds& n) const noexcept
    { return microseconds{m_msec-n.m_msec}; }
    
    /// Division between microseconds.
    constexpr microseconds operator/(const microseconds& sc) noexcept
    { return microseconds(m_msec/sc.m_msec); }
    
    /// Equality operator.
    constexpr bool operator==(const microseconds& d) const noexcept
    { return m_msec == d.m_msec; }

    /// Greater than operator.
    constexpr bool operator>(const microseconds& d) const noexcept
    { return m_msec > d.m_msec; }

    /// Greater or equal to operator.
    constexpr bool operator>=(const microseconds& d) const noexcept
    { return m_msec >= d.m_msec; }

    /// Less than operator.
    constexpr bool operator<(const microseconds& d) const noexcept
    { return m_msec < d.m_msec; }

    /// Less or equal to operator.
    constexpr bool operator<=(const microseconds& d) const noexcept
    { return m_msec <= d.m_msec; }
    
    /// Do the microseconds sum up to more than one day?
    constexpr bool more_than_day() const noexcept { return m_msec>max_in_day; }
    
    /// Cast to microseconds::underlying_type.
    constexpr underlying_type as_underlying_type() const noexcept
    { return m_msec; }
    
    /// \brief Normalize microseconds and return the integral days.
    ///
    /// If the microseconds sum up to more (or equal to) one day, remove the
    /// integer days and return them as integer; reset the microseconds to 
    /// microseconds of the new day.
    ///
    /// \return The integer number of days (if the microseconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative microseconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int remove_days() noexcept
    {
        int day { static_cast<int>(m_msec/max_in_day) };
        m_msec %= max_in_day;
        return day;
    }
    
    /// \brief Cast to days.
    ///
    /// If the microseconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// \return The integer number of days (if the microseconds are more than a day).
    /// \throw  Does not throw.
    /// \bug    What about negative microseconds?
    ///
    /// \note The calling instance cannot be negative (i don't knwo how to
    ///       normalize negative seconds).
    constexpr int to_days() const noexcept
    { return static_cast<int>(m_msec/max_in_day); }
    
    /// Cast to fractional days.
    constexpr double fractional_days() const noexcept
    {
        return static_cast<double>(m_msec) / static_cast<double>(max_in_day);
    }
    
    /// Cast to fractional seconds
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(m_msec) * 1.0e-6; }
    
    /// Translate to hours, minutes, seconds and microseconds.
    /// \bug need more documentation
    constexpr std::tuple<hours, minutes, seconds, long>
    to_hmsf() const noexcept
    {
        long hr { m_msec/3600000000L                       };  // hours
        long mn { (m_msec%3600000000L)/60000000L           };  // minutes
        long sc { ((m_msec%3600000000L)%60000000L)/1000000L};  // seconds
        long ns { m_msec-((hr*60L+mn)*60L+sc)*1000000L     };  // microsec.
        return std::make_tuple( hours  { static_cast<hours::underlying_type>(hr) },
                                minutes{ static_cast<minutes::underlying_type>(mn) },
                                seconds{ sc },
                                ns );
    }

private:
    /// Cast to any arithmetic type.
    template<typename T,
             typename = std::enable_if_t<std::is_arithmetic<T>::value>
             >
    constexpr T cast_to() const noexcept
    { return static_cast<T>(m_msec); }

    /// Microseconds as long ints.
    underlying_type m_msec;

}; // class microseconds

/// \brief Number of expressible days for any second type.
///
/// This (template) function will return the number of whole days that can be
/// expressed using any instance of a second type (i.e. ngpt::seconds, 
/// ngpt::milliseconds, etc). For any of these types, trying to hold more days
/// than this limit may result in overflow.
///
/// \tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
template<typename S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    constexpr typename S::underlying_type max_days_allowed()
{ return std::numeric_limits<typename S::underlying_type>::max()/S::max_in_day; }

/// \brief Express the difference between two Modified Julian Days as any second
///        type.
///
/// \tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
/// \param[in] d1 The first ngpt::modified_julian_day (difference is d1-d2)
/// \param[in] d2 The second ngpt::modified_julian_day (difference is d1-d2)
/// \return       The difference between d1 and d2 in the second type S (e.g.
///               ngpt::seconds, ngpt::milliseconds, etc)
///
/// \note The difference between two Modified Julian Days is always an integral
///       number of days (no fractional part!).
template<typename S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    S mjd_sec_diff(modified_julian_day d1, modified_julian_day d2) noexcept
{
    modified_julian_day d {d1-d2};
    return S{d.as_underlying_type() * S::max_in_day};
}

/// For user-defined literals, i am going to replace long with
/// unsigned long long int.
namespace ddetail { using ulli = unsigned long long int; }

/// A year can be constructed via "_Y".
constexpr year operator "" _Y(ddetail::ulli i) noexcept
{ return year{static_cast<year::underlying_type>(i)}; }

/// A month can be constructed via "_M".
constexpr month operator "" _M(ddetail::ulli i) noexcept
{ return month{static_cast<month::underlying_type>(i)}; }

/// A day of month can be constructed via "_D".
constexpr day_of_month operator "" _D(ddetail::ulli i) noexcept
{ return day_of_month{static_cast<day_of_month::underlying_type>(i)}; }

/// An hour can be constructed via "_h".
constexpr hours operator "" _h(ddetail::ulli i) noexcept
{ return hours{static_cast<hours::underlying_type>(i)}; }

/// A minute can be constructed via "_m".
constexpr minutes operator "" _m(ddetail::ulli i) noexcept
{ return minutes{static_cast<minutes::underlying_type>(i)}; }

/// Seconds can be constructed via "_sec".
constexpr seconds operator "" _sec(ddetail::ulli i) noexcept
{ return seconds{static_cast<seconds::underlying_type>(i)}; }

/// MilliSeconds can be constructed via "_millisec".
constexpr milliseconds operator "" _millisec(ddetail::ulli i) noexcept
{ return milliseconds{static_cast<milliseconds::underlying_type>(i)}; }

/// MicroSeconds can be constructed via "_microsec".
constexpr microseconds operator "" _microsec(ddetail::ulli i) noexcept
{ return microseconds{static_cast<microseconds::underlying_type>(i)}; }

} // end namespace

#endif // define DATETIME
