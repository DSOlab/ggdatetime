///
/// @file  dtfund.hpp
///
/// @brief Fundamental (core) datetime constants and algorithms.
///
/// This file contains fundamental constants and algorithms for manipulating
/// date and time, targeting GNSS applications.
///
/// @author xanthos
///
/// @bug No known bugs.
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
/// @warning This is not always true in case of UTC dates; the day a leap second
///          is inserted has one more second!
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

/// @brief Calendar date to Modified Julian Day.
///
/// @param[in] iy The year (int).
/// @param[in] im The month (int).
/// @param[in] id The day of month (int).
/// @return    The Modified Julian Date (as long).
/// @throw     A runtime_error if the month and/or day is invalid.
///
/// @note There is another version of this function, that is way more type-safe
///       Whenever possible, use that one instead of this.
///
/// @see ngpt::cal2mjd
///
/// Reference: iauCal2jd
long cal2mjd(int iy, int im, int id);

/// Convert hours, minutes, seconds into fractional days.
/// double hms2fd(int, int, double) noexcept;

/// Decompose fractional days to hours, minutes, seconds and fractional seconds
/// with a given precision.
/// void fd2hms(double, int, int ihmsf[4]);

/// @brief Check if year is leap.
///
/// @param[in] iy The year to check (int).
/// @return true if year is leap, false otherwise.
///
/// @throw Does not throw.
///
inline bool
is_leap(int iy) noexcept
{ return !(iy%4) && (iy%100 || !(iy%400)); }

/// @brief Transform a calendar date to modified_julian_day.
///
/// @param[in] y The year.
/// @param[in] m The month.
/// @param[in] d The day of month.
/// @return      The corresponding Modified Julian Day
/// 
/// @throw       This function will throw if ngpt::cal2mjd throws, i.e. if
///              the input date is invalid.
///
modified_julian_day cal2mjd(year, month, day_of_month);

/// Convert a pair of Year, Day of year toMJDay.
modified_julian_day ydoy2mjd(year, day_of_year) noexcept;

/// @brief For a given UTC date, calculate delta(AT) = TAI-UTC.
///
/// The day of month is actually not needed, since all leap second insertions
/// happen at the begining, i.e. the first day of a month.
///
/// @note In case using MJD (and not calendar date) is more convinient, use the
///       overloaded function ngpt::dat
///
/// @warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// @see IAU SOFA (iau-dat.c)
/// @see ngpt::dat
int dat(year iy, month im) noexcept;

/// @overload ngpt::dat(ngpt::year iy, ngpt::month im) noexcept
///
/// @note In case using calendar date (and not MJD) is more convinient, use the
///       overloaded function ngpt::dat
///
/// @warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// @see IAU SOFA (iau-dat.c)
/// @see ngpt::dat
int dat(modified_julian_day mjd) noexcept;

/*
/// @brief A class to represent GPS time.
///
/// GPS Time is a uniformly counting time scale beginning at the 1/5/1980 to 
/// 1/6/1980 midnight. January 6, 1980 is a Sunday. GPS Time counts in weeks
/// and seconds of a week from this instant. The weeks begin at the
/// Saturday/Sunday transition. The days of the week are numbered, with Sunday
/// being 0, 1 Monday, etc.
/// GPS week 0 began at the beginning of the GPS Time Scale. Within each week
/// the time is usually denoted as the second of the week. This is a number
/// between 0 and 604,800 (60 x 60 x 24 x 7).
/// The word "uniformly" is used above to indicate that there are no
/// "leap seconds" in this time system.
class gps_time
{
public:
private:
};
*/

/// @brief A wrapper class for years.
///
/// A year is represented by just an integer number. There are no limits
/// (excpept from integer overflow) to the range of the year (integer).
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// year (constructor) can only have positive values.
class year
{
public:
    /// Years are represented as integers.
    typedef int underlying_type;

    /// Constructor; default year is 1900.
    explicit constexpr
    year(underlying_type i=1900) noexcept
    : m_year(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i > 0);
#endif
    };

    /// Get the year as year::underlying_type.
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_year; }

    /// Equality operator
    constexpr bool
    operator==(year y) const noexcept
    { return y.m_year == m_year; }

private:
    /// The year as underlying type.
    underlying_type m_year;

}; // class year

/// @brief A wrapper class for months.
///
/// A month is represented by just an integer number. There are no limits
/// (excpept from integer overflow) to the range of the month (integer), i.e. 
/// the month is not checked (by default) to be in the range [1,12]. So, a user
/// can construct a month from whatever integer, e.g. :
/// month m (200);
/// will work fine! To check if the month is within the range [1,12], use the
/// month::is_valid method.
/// Most functions (within ngpt) accept months in the range [1,12]; do not
/// use the range [0,11], except if you realy know what you're doing.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// month (constructor) can only have positive values in the range [1,12].
///
/// @warning Most functions (within ngpt) accept months in the range [1,12];
///          do not use the range [0,11], except if you realy know what you're
///          doing.
class month
{
public:
    /// Months are represented as int.
    typedef int underlying_type;

    /// Constructor; default month is 1.
    explicit constexpr
    month(underlying_type i=1) noexcept
    : m_month(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert( this->is_valid() );
#endif
    };

    /// @brief Constructor from a c-string.
    ///
    /// @param[in] str The month's name; The 3 first characters should match
    ///                a month in the month::short_names array.
    /// @throw         An std::invalid_argument exception is thrown if a) no
    ///                match is found, or b) the input string is too short.
    explicit
    month(const char* str);

    /// Get the month as month::underlying_type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_month; }

    /// Return the corresponding short name (i.e. 3-char name) e.g. "Jan".
    /// @return Returns a pointer to the class's (static member) month::short_names
    ///         string array.
    const char*
    short_name() const noexcept
    { return short_names[m_month-1]; }

    /// Return the corresponding long name (i.e. normal month name) e.g. "January".
    /// @return Returns a pointer to the class's (static member) month::long_names
    ///         string array.
    const char*
    long_name() const noexcept
    { return long_names[m_month-1]; }

    /// Check if the month is within the interval [1,12].
    bool
    is_valid() const noexcept
    { return m_month > 0 && m_month <= 12; }
    
    /// Equality operator
    constexpr bool
    operator==(month m) const noexcept
    { return m_month == m.m_month; }

private:
    /// @brief Decleration of short month names. 
    /// @note  Note that we do need a definition in the .cpp file.
    constexpr
    static const char* short_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    /// @brief  Decleration of long month names.
    /// @note   Note that we do need a definition in the .cpp file.
    constexpr
    static const char* long_names[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    /// The month as underlying_type.
    underlying_type m_month;

}; // class month

/// @brief A wrapper class for day of month.
/// 
/// A day_of_month is just an integer, representing the day of month. No limits
/// are set though, so the user can construct a day_of_month from whatever 
/// integer. Inluding negative numbers!
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// day_of_month (constructor) can only have positive values.
///
/// @todo Provide a day_of_month::validate method.
class day_of_month
{
public:
    /// Days are represented as ints.
    typedef int underlying_type;
    
    /// Constructor; default day of month is 1.
    explicit constexpr
    day_of_month(underlying_type i=1) noexcept
    : m_dom(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>0);
#endif
    };
    
    /// Get the day_of_month as day_of_month::underlying_type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_dom; }

    /// Check if a given instance is valid.
    ///
    /// @param[in] y  The year the dom referes to (will check for leap)
    /// @param[in] m  The month the dom refers to; range [1,12]
    /// @return       If the dom is valid (considering the year and month) true
    ///               is returned; else, the function will return false.
    bool
    is_valid(ngpt::year y, ngpt::month m) const noexcept;
    
    /// Equality operator
    constexpr bool
    operator==(day_of_month d) const noexcept
    { return m_dom == d.m_dom; }

private:
    /// The day of month as underlying_type.
    underlying_type m_dom;

}; // class day_of_month

/// @brief A wrapper class for Modified Julian Day.
///
/// A Modified Julian Day is represented by a long integer (there is no
/// fractional part). Thus, a modified_julian_day only represents a date *not*
/// a datetime.
/// The Modified Julian Day, was introduced by space scientists in the late
/// 1950's. It is defined as \f$MJD = JD - 2400000.5\f$ The half day (used in JD) 
/// is subtracted so that the day starts at midnight in conformance with civil
/// time reckoning.
/// The MJD is a convenient dating system with only 5 digits, sufficient for
/// most modern purposes. To convert between MJD and JD we need the Julian
/// Date of Modified Julian Date zero, aka ngpt::mjd0_jd, which is 2400000.5
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// modified_julian_day can only have positive values, or
/// std::numeric_limits<underlying_type>::min(), which is the only negative
/// value allowed.
///
/// @see http://tycho.usno.navy.mil/mjd.html
class modified_julian_day
{
public:
    /// MJDs are represented as long ints.
    typedef long underlying_type;

    /// Max possible modified_julian_day
    constexpr static modified_julian_day
    plus_infinity() noexcept
    { return modified_julian_day{std::numeric_limits<underlying_type>::max()}; }
    
    /// Min possible modified_julian_day
    constexpr static modified_julian_day
    minus_infinity() noexcept
    { return modified_julian_day{std::numeric_limits<underlying_type>::min()}; }
    
    /// Constructor; default Modified Julian Day is 1.
    explicit constexpr
    modified_julian_day(underlying_type i=1) noexcept
    : m_mjd(i) 
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>0 || i==std::numeric_limits<underlying_type>::min());
#endif
    };
    
    /// @brief Constructor from Year and DayOfYear.
    /// 
    /// @param[in] iy The year.
    /// @param[in] id The day of year.
    ///
    /// @see "Remondi Date/Time Algorithms", http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    /// @bug Where the fuck is this definition?
    explicit constexpr
    modified_julian_day(year iy, day_of_year id) noexcept;
    
    /// Get the modified_julian_day as modified_julian_day::underlying_type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_mjd; }
    
    /// Define addition (between MJDs).
    constexpr void
    operator+=(const modified_julian_day& d) noexcept
    { m_mjd += d.m_mjd; }
    
    /// Define subtraction (between MJDs).
    constexpr void
    operator-=(const modified_julian_day& d) noexcept
    {
        m_mjd -= d.m_mjd;
#ifdef USE_DATETIME_CHECKS
        assert( m_mjd > 0 );
#endif
    }
    
    /// Operator - (subtraction).
    constexpr modified_julian_day
    operator-(const modified_julian_day& mjd) const noexcept
    { return modified_julian_day{m_mjd-mjd.m_mjd}; }
    
    /// Operator + (addition).
    constexpr modified_julian_day
    operator+(const modified_julian_day& mjd) const noexcept
    { return modified_julian_day{m_mjd+mjd.m_mjd}; }

    /// Operator == (equality).
    constexpr bool
    operator==(const modified_julian_day& d) const noexcept
    { return m_mjd == d.m_mjd; }
    
    /// Operator != (inequality).
    constexpr bool
    operator!=(const modified_julian_day& d) const noexcept
    { return !(*this == d); }

    /// Operator > (greater than).
    constexpr bool
    operator>(const modified_julian_day& d) const noexcept
    { return m_mjd > d.m_mjd; }

    /// Operator >= (greater or equal to).
    constexpr bool
    operator>=(const modified_julian_day& d) const noexcept
    { return m_mjd >= d.m_mjd; }

    /// Operator < (less than).
    constexpr bool
    operator<(const modified_julian_day& d) const noexcept
    { return m_mjd < d.m_mjd; }

    /// Operator <= (less or equal to).
    constexpr bool
    operator<=(const modified_julian_day& d) const noexcept
    { return m_mjd <= d.m_mjd; }

    /// Operator -- (minus 1 mjd).
    constexpr modified_julian_day&
    operator--() noexcept
    {
        --m_mjd;
#ifdef USE_DATETIME_CHECKS
        assert(m_mjd>0);
#endif
        return *this;
    }

    /// @brief Convert a Modified Julian Day to Year and Day of year.
    ///
    /// @return A tuple with two elements: (year, day_of_year)
    /// @throw  Does not throw.
    ///
    /// @warning No check if performed to see if the resulting day of year is
    ///          valid! If you want to be sure, check the returned value(s).
    ///
    /// @see    "Remondi Date/Time Algorithms",
    ///          http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    std::tuple<year, day_of_year>
    to_ydoy() const noexcept;
    
    /// @brief Convert a Modified Julian Day to Calendar Date.
    ///
    /// @return A tuple with three elements: (year, month, day_of_month)
    /// @throw  Does not throw.
    ///
    /// @warning No check if performed to see if the resulting month and day of
    ///          month is valid! If you want to be sure, check the returned
    ///          value(s).
    ///
    /// @see    "Remondi Date/Time Algorithms",
    ///          http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
    std::tuple<year, month, day_of_month>
    to_ymd() const noexcept;
    
private:
    /// The modified julian day as underlying type.
    underlying_type m_mjd;

}; // class modified_julian_day

/// @brief A wrapper class for day of year.
///
/// A day of year (doy) is represented by an integer; any integer will do, no
/// automatic check is performed to see if the doy is within the range 
/// [1,365/6]. In fact, one can actually use a negative integer to represent
/// a doy. It the user wishes to check wether the given doy is indeed valid,
/// then the method day_of_year::validate() can be used.
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// day_of_year (constructor) can only have zero or positive values.
///
/// @todo Provide a day_of_year::validate method.
class day_of_year
{
public:
    /// Day of year represented as int.
    typedef int underlying_type;
    
    /// Constructor; default day of year is 0.
    explicit constexpr
    day_of_year(underlying_type i=0) noexcept
    : m_doy(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0);
#endif
    };
    
    /// Cast to underlying type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_doy; }
    
    /// Equality operator
    constexpr bool
    operator==(day_of_year d) const noexcept
    { return d.m_doy == m_doy; }

private:
    /// The day_of_year as day_of_year::underlying_type.
    underlying_type m_doy;   
};

/// @brief A wrapper class for hours.
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
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// hours (constructor) can only have zero or positive values.
class hours
{
public:
    /// Hours are represented by ints.
    typedef int underlying_type;
    
    /// Constructor; default hours is 0.
    explicit constexpr
    hours(underlying_type i=0) noexcept
    : m_hours(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0);
#endif
    };
    
    /// Get the hours as hours::underlying_type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_hours; }

    /// Remove whole days and only keep the remainding hours.
    ///
    /// This function will actually divide the hours with 24, return the
    /// quatient and keep the remainder. After the function invocation, the
    /// hours will be:
    /// hours <- hours % 24
    ///
    /// @return The whole days (priorly) included within the instance, aka
    ///         hours / 24 (integer division)
    ///
    /// @note Cannot be constexpr cause of non-‘constexpr’ function 
    ///       ‘div_t div(int, int)’, at least in g++.
    int
    remove_days() noexcept
    {
        std::div_t dv;
        dv = std::div(m_hours, 24);
        m_hours = dv.rem;
        return dv.quot;
    }
    
    /// Equality operator
    constexpr bool
    operator==(hours h) const noexcept
    { return m_hours == h.m_hours; }

private:
    /// The hours as hours::underlying_type
    underlying_type m_hours;

}; // class hours

/// @brief A wrapper class for minutes.
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
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// minutes (constructor) can only have zero or positive values.
class minutes
{
public:
    /// Minutes are represented by ints
    typedef int underlying_type;
    
    /// Constructor
    explicit constexpr
    minutes(underlying_type i=0) noexcept
    : m_min(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0);
#endif
    };

    /// Get the minutes as minutes::underlying_type
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_min; }
    
    /// Remove whole hours and only keep the remainding minutes.
    ///
    /// This function will actually divide the minutes with 60, return the
    /// quatient and keep the remainder. After the function invocation, the
    /// minutes will be:
    /// minutes <- minutes % 60
    ///
    /// @return The whole hours (priorly) included within the instance, aka
    ///         minutes / 60 (integer division)
    ///
    /// @note Cannot be constexpr cause of non-‘constexpr’ function 
    ///       ‘div_t div(int, int)’, at least in g++.
    hours
    remove_hours() noexcept
    {
        std::div_t dv;
        dv = std::div(m_min, 60);
        m_min = dv.rem;
        return hours{dv.quot};
    }
    
    /// Equality operator
    constexpr bool
    operator==(minutes m) const noexcept
    { return m_min == m.m_min; }

private:
    /// The minutes as underlying type.
    underlying_type m_min;

}; // class minutes

/// @brief A wrapper class for seconds.
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
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// seconds (constructor) can only have zero or positive values.
///
/// @warning The maximum number of days that can be expressed in seconds without
///          fear of overflow is given by the template function
///          ngpt::max_days_allowed
///
/// @see ngpt::milliseconds
/// @see ngpt::microseconds
class seconds {
public:
    /// Seconds are represented as long ints.
    typedef long underlying_type;
    
    /// Seconds is a subdivision of seconds.
    static constexpr bool
    is_of_sec_type { true };
    
    /// Seconds in day.
    static constexpr underlying_type
    max_in_day { 86400L };

    /// The scale factor to transform from seconds to seconds.
    template<typename T>
        static constexpr T sec_factor() noexcept
    { return static_cast<T>(1); }

    /// Constructor; default seconds is 0.
    explicit constexpr
    seconds(underlying_type i=0L) noexcept
    : m_sec(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0L);
#endif
    };

    /// Constructor from hours, minutes, seconds.
    explicit constexpr
    seconds(hours h, minutes m, seconds c) noexcept
    : m_sec {  c.as_underlying_type()
             + m.as_underlying_type()*60L
             + h.as_underlying_type()*3600L}
    {};

    /// Constructor from hours, minutes, fractional seconds.
    ///
    /// @note Only the integral part of the input seconds (fs) is considered;
    ///       the fractional part is ommited.
    explicit constexpr
    seconds(hours h, minutes m, double fs) noexcept
    : m_sec{  static_cast<long>(fs)
            + m.as_underlying_type()*60L
            + h.as_underlying_type()*3600L}
    {
#ifdef USE_DATETIME_CHECKS
        assert(fs>=0e0);
#endif
    }
    
    /// Addition operator between seconds.
    constexpr void
    operator+=(const seconds& sc) noexcept
    { m_sec+=sc.m_sec; }

    /// Subtraction operator between seconds.
    constexpr void
    operator-=(const seconds& sc) noexcept
    {
        m_sec-=sc.m_sec;
#ifdef USE_DATETIME_CHECKS
        assert(m_sec >= 0);
#endif
    }
    
    /// Overload - operator (subtraction).
    constexpr seconds
    operator-(const seconds& n) const noexcept
    { return seconds{m_sec - n.m_sec}; }

    /// Overload + operator (addition).
    constexpr seconds
    operator+(const seconds& sec) const noexcept
    { return seconds{m_sec+sec.m_sec}; }

    /*
    /// Overload operator '/'
    /// @note This is integer division; no fractional part considered.
    constexpr seconds
    operator/(const seconds& n) const noexcept
    { return seconds{m_sec/n.m_sec}; }
    */
  
    /// Equality operator.
    constexpr bool
    operator==(const seconds& d) const noexcept
    { return m_sec == d.m_sec; }

    /// Greater than operator.
    constexpr bool
    operator>(const seconds& d) const noexcept
    { return m_sec > d.m_sec; }

    /// Greater or equal to operator.
    constexpr bool
    operator>=(const seconds& d) const noexcept
    { return m_sec >= d.m_sec; }
    
    /// Less than operator.
    constexpr bool
    operator<(const seconds& d) const noexcept
    { return m_sec < d.m_sec; }

    /// Less or equal to operator.
    constexpr bool
    operator<=(const seconds& d) const noexcept
    { return m_sec <= d.m_sec; }
    
    /// Do the secods sum up to more than one day?
    constexpr bool
    more_than_day() const noexcept
    { return m_sec>max_in_day; }
    
    /// Get the seconds as seconds::underlying_type .
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_sec; }
    
    /// @brief Normalize seconds and return the integeral days.
    ///
    /// If the seconds sum up to more (or equal to) one day, remove the integer
    /// days and return them as integer; reset the seconds to seconds of the
    /// new day.
    ///
    /// @return The integer number of days (if the seconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the seconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned and
    ///         m_sec will be set to 0).
    /// 
    constexpr int
    remove_days() noexcept
    {
        /* assert( m_sec >= 0 ); */
        long d { static_cast<int>(m_sec / max_in_day) };
        m_sec %= max_in_day;
        return (int)d;
    }
    
    /// @brief Cast to days.
    ///
    /// If the seconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// @return The integer number of days (if the seconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the seconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
    ///
    constexpr int
    to_days() const noexcept
    {
        /* assert( m_sec >= 0 ); */
        return static_cast<int>(m_sec/max_in_day);
    }
    
    /// @brief Seconds as fractional days.
    ///
    /// Interpret (cast) the seconds as fractional days; returns a double.
    constexpr double
    fractional_days() const noexcept
    {
        return static_cast<double>(m_sec) / static_cast<double>(max_in_day);
    }
    
    /// Cast to double (i.e. fractional seconds).
    constexpr double
    to_fractional_seconds() const noexcept
    { return static_cast<double>(m_sec); }

    /// Translate to hours, minutes, seconds and fractional seconds
    /// @bug needs mor documentation
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

/// @brief A wrapper class for milliseconds (i.e. 10**-3 sec).
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
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// milliseconds (constructor) can only have zero or positive values.
/// 
/// @note milliseconds can be cast to ngpt::seconds (via a static_cast or
/// a C-type cast) but the opposite is not true; i.e. ngpt::seconds cannot be
/// cast to milliseconds. This is still an open question!
///
/// @see ngpt::seconds
/// @see ngpt::microseconds
///
class milliseconds
{
public:
    /// MilliSeconds are represented as long ints.
    typedef long underlying_type;
    
    /// MilliSeconds are a subdivision of seconds.
    static constexpr bool
    is_of_sec_type { true };
    
    /// Max milliseconds in one day.
    static constexpr long
    max_in_day { 86400L * 1000L };

    /// The scale factor to transform from seconds to milliseconds.
    template<typename T>
        static constexpr T sec_factor() noexcept
    { return static_cast<T>(1000); }

    /// Constructor; default milliseconds is 0.
    explicit constexpr milliseconds(underlying_type i=0L) noexcept
    : m_msec(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0L);
#endif
    };
 
    /// Constructor from hours, minutes, milliseconds.   
    explicit constexpr
    milliseconds(hours h, minutes m, milliseconds c) noexcept
    : m_msec { c.as_underlying_type()
        + m.as_underlying_type()*60L  * sec_factor<long>()
        + h.as_underlying_type()*3600L* sec_factor<long>()}
    {};
    
    /// @brief Constructor from hours, minutes, fractional seconds.
    ///
    /// Note that the fractional seconds are only considered with millisecond
    /// precision; i.e. anything smaller than 10e-3 will not be considered (for
    /// the fs input argument).
    explicit constexpr
    milliseconds(hours h, minutes m, double fs) noexcept
    : m_msec{ static_cast<long>(fs * sec_factor<double>())
        + (m.as_underlying_type()*60L
        + h.as_underlying_type()*3600L) * sec_factor<long>()}
    {
#ifdef USE_DATETIME_CHECKS
        assert(fs>=0e0);
#endif
    };
    
    /// @brief Cast to ngpt::seconds.
    /// Milliseconds can be cast to seconds (with a loss of precission).
    constexpr explicit
    operator seconds() const
    { return seconds{m_msec / sec_factor<long>()}; }
    
    /// Addition operator.
    constexpr milliseconds
    operator+(const milliseconds& sec) const noexcept
    { return milliseconds{m_msec+sec.m_msec}; }
    
    /// Addition operator.
    constexpr void
    operator+=(const milliseconds& ms) noexcept
    { m_msec+=ms.m_msec; }

    /// Subtraction operator.
    constexpr void
    operator-=(const milliseconds& ms) noexcept
    {
        m_msec-=ms.m_msec;
#ifdef USE_DATETIME_CHECKS
        assert(m_msec>=0);
#endif
    }
    
    /// Subtraction operator.
    constexpr milliseconds
    operator-(const milliseconds& n) const noexcept
    { return milliseconds{m_msec - n.m_msec}; }
    
    /*
    /// Division operator between milliseconds.
    constexpr milliseconds
    operator/(const milliseconds& sc) noexcept
    { return milliseconds{m_msec/sc.m_msec}; }
    */
    
    /// Equality operator.
    constexpr bool
    operator==(const milliseconds& d) const noexcept
    { return m_msec == d.m_msec; }

    /// Greater than operator.
    constexpr bool
    operator>(const milliseconds& d) const noexcept
    { return m_msec > d.m_msec; }

    /// Greater or equal to operator.
    constexpr bool
    operator>=(const milliseconds& d) const noexcept
    { return m_msec >= d.m_msec; }

    /// Less than operator.
    constexpr bool
    operator<(const milliseconds& d) const noexcept
    { return m_msec < d.m_msec; }

    /// Less or equal to operator.
    constexpr bool
    operator<=(const milliseconds& d) const noexcept
    { return m_msec <= d.m_msec; }
    
    /// Do the milliseconds sum up to more than one day ?
    constexpr bool
    more_than_day() const noexcept
    { return m_msec>max_in_day; }
    
    /// Get the milliseconds as milliseconds::underlying_type.
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_msec; }
    
    /// @brief Normalize milliseconds and return the integeral days.
    ///
    /// If the milliseconds sum up to more (or equal to) one day, remove the integer
    /// days and return them as integer; reset the milliseconds to milliseconds of the
    /// new day.
    ///
    /// @return The integer number of days (if the milliseconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the seconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned and
    ///         m_sec will be set to 0).
    /// 
    constexpr int
    remove_days() noexcept
    {
        /* assert( m_sec >= 0 ); */
        long d { static_cast<long>(m_msec / max_in_day) };
        m_msec %= max_in_day;
        return (int)d;
    }
    
    /// @brief Cast to days.
    ///
    /// If the milliseconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// @return The integer number of days (if the milliseconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the seconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
    ///
    constexpr int
    to_days() const noexcept
    {
        /* assert( m_msec >= 0 ); */
        return int{static_cast<int>(m_msec/max_in_day)};
    }
    
    /// Cast to fractional days.
    constexpr double
    fractional_days() const noexcept
    {
        return static_cast<double>(m_msec)/static_cast<double>(max_in_day);
    }
    
    /// Cast to fractional ngpt::seconds
    constexpr double
    to_fractional_seconds() const noexcept
    { return static_cast<double>(m_msec)*1.0e-3; }
    
    /// @brief Resolve to (integer) seconds and fractional seconds.
    ///
    /// Split the milliseconds to ngpt::seconds and fractional seconds.
    ///
    /// @param[out] fraction Fractional seconds
    /// @return              Integral seconds
    /// @throw               Does not throw
    ///
    /// @note Only works for positive milliseconds.
    constexpr seconds
    resolve_sec(double& fraction) const noexcept
    {
        /* assert( m_msec >= 0); */
        seconds sec { m_msec / sec_factor<long>() };
        fraction = static_cast<double>(m_msec % sec_factor<long>())*1e-3;
        return sec;
    }
    
    /// Translate to hours, minutes, seconds and milliseconds
    /// @bug need more documentation
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
        constexpr T
        cast_to() const noexcept
    { return static_cast<T>( m_msec ); }

    /// Milliseconds as underlying type.
    underlying_type m_msec;

}; /// class milliseconds

/// @brief A wrapper class for microseconds (i.e 10**-6 sec.).
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
/// If the code is compiled with the switch USE_DATETIME_CHECKS, then the
/// microseconds (constructor) can only have zero or positive values.
/// 
/// @note microseconds can be cast to ngpt::seconds and ngpt::milliseconds (via
/// a static_cast or a C-type cast) but the opposite is not true; i.e. 
/// ngpt::seconds cannot be cast to microseconds.This is still an open question!
///
/// @see ngpt::seconds
/// @see ngpt::milliseconds
///
class microseconds
{
public:
    /// Microseconds are represented as long integers.
    typedef long underlying_type;
    
    /// Microseconds is a subdivision of seconds.
    static constexpr bool
    is_of_sec_type { true };
    
    /// Max microseconds in day.
    static constexpr long
    max_in_day { 86400L * 1000000L };

    /// The scale factor to transform from seconds to microseconds.
    template<typename T>
        static constexpr T
        sec_factor() noexcept
    { return static_cast<T>(1000000); }

    /// Constructor; default microseconds is 0.
    explicit constexpr
    microseconds(underlying_type i=0L) noexcept
    : m_msec(i)
    {
#ifdef USE_DATETIME_CHECKS
        assert(i>=0L);
#endif
    };
    
    /// Constructor from hours, minutes, microseconds.
    explicit constexpr
    microseconds(hours h, minutes m, microseconds c) noexcept
    : m_msec { c.as_underlying_type()
            +(m.as_underlying_type()*60L
            + h.as_underlying_type()*3600L) * sec_factor<long>() }
    {};
    
    /// Constructor from hours, minutes, fractional seconds.
    /// @note Fractional seconds with precission larger than microseconds are
    ///       not taken into account; i.e. we only consider the fractiona part
    ///       up to 10e-6.
    explicit constexpr
    microseconds(hours h, minutes m, double fs) noexcept
    : m_msec{ static_cast<long>(fs * sec_factor<double>())
        + (m.as_underlying_type()*60L
        + h.as_underlying_type()*3600L) * sec_factor<long>() }
    {};
    
    /// Microseconds can be cast to milliseconds will a loss of accuracy.
    constexpr explicit
    operator milliseconds() const
    { return milliseconds(m_msec / 1000L); }
    
    /// Microseconds can be cast to seconds will a loss of accuracy.
    constexpr explicit
    operator seconds() const
    { return seconds(m_msec / sec_factor<long>()); }
    
    /// Addition between microseconds.
    constexpr void
    operator+=(const microseconds& ns) noexcept
    { m_msec+=ns.m_msec; }

    /// Subtraction between microseconds.
    constexpr void
    operator-=(const microseconds& ns) noexcept
    {
        m_msec-=ns.m_msec;
#ifdef USE_DATETIME_CHECKS
        assert(m_msec>=0);
#endif
    }

    /// Addition between microseconds.
    constexpr microseconds
    operator+(const microseconds& sec) const noexcept
    { return microseconds{m_msec+sec.m_msec}; }

    /// Subtraction between microseconds.
    constexpr microseconds
    operator-(const microseconds& n) const noexcept
    { return microseconds{m_msec-n.m_msec}; }
    
    /*
    /// Division between microseconds.
    constexpr microseconds operator/(const microseconds& sc) noexcept
    { return microseconds(m_msec/sc.m_msec); }
    */
    
    /// Equality operator.
    constexpr bool
    operator==(const microseconds& d) const noexcept
    { return m_msec == d.m_msec; }

    /// Greater than operator.
    constexpr bool
    operator>(const microseconds& d) const noexcept
    { return m_msec > d.m_msec; }

    /// Greater or equal to operator.
    constexpr bool
    operator>=(const microseconds& d) const noexcept
    { return m_msec >= d.m_msec; }

    /// Less than operator.
    constexpr bool
    operator<(const microseconds& d) const noexcept
    { return m_msec < d.m_msec; }

    /// Less or equal to operator.
    constexpr bool
    operator<=(const microseconds& d) const noexcept
    { return m_msec <= d.m_msec; }
    
    /// Do the microseconds sum up to more than one day?
    constexpr bool
    more_than_day() const noexcept
    { return m_msec>max_in_day; }
    
    /// Cast to microseconds::underlying_type.
    constexpr underlying_type
    as_underlying_type() const noexcept
    { return m_msec; }
    
    /// @brief Normalize microseconds and return the integeral days.
    ///
    /// If the microseconds sum up to more (or equal to) one day, remove the integer
    /// days and return them as integer; reset the microseconds to microseconds of the
    /// new day.
    ///
    /// @return The integer number of days (if the microseconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the microseconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned and
    ///         m_sec will be set to 0).
    /// 
    constexpr int
    remove_days() noexcept
    {
        /* assert( m_sec >= 0 ); */
        long d { static_cast<int>(m_msec / max_in_day) };
        m_msec %= max_in_day;
        return (int)d;
    }
    
    /// @brief Cast to days.
    ///
    /// If the microseconds sum up to more (or equal to) one day, return the 
    /// (integral) number of days.
    ///
    /// @return The integer number of days (if the microseconds are more than a day).
    /// @throw  Does not throw.
    ///
    /// @note   The number of days returned can be negative (!!), if the seconds
    ///         are negative (i.e. if m_sec = -86400, -1 will be returned).
    ///
    constexpr int
    to_days() const noexcept
    { return static_cast<int>(m_msec/max_in_day); }
    
    /// Cast to fractional days.
    constexpr double
    fractional_days() const noexcept
    {
        return static_cast<double>(m_msec) / static_cast<double>(max_in_day);
    }
    
    /// Cast to fractional seconds
    constexpr double
    to_fractional_seconds() const noexcept
    { return static_cast<double>(m_msec) * 1.0e-6; }
    
    /// Translate to hours, minutes, seconds and microseconds.
    /// @bug need more documentation
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
        constexpr T
        cast_to() const noexcept
    { return static_cast<T>(m_msec); }

    /// Microseconds as long ints.
    underlying_type m_msec;

}; // class microseconds

/// @@rief Number of expressible days for any second type.
///
/// This (template) function will return the number of whole days that can be
/// expressed using any instance of a second type (i.e. ngpt::seconds, 
/// ngpt::milliseconds, etc). For any of these types, trying to hold more days
/// than this limit may result in overflow.
///
/// @tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
template<typename S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    constexpr typename S::underlying_type
    max_days_allowed()
{ return std::numeric_limits<typename S::underlying_type>::max()/S::max_in_day; }

/// @brief Express the difference between two Modified Julian Days as any second
///        type.
///
/// @tparam S Any class of second type, i.e. any class S that has a (static)
///           member variable S::is_of_sec_type set to true.
/// @param[in] d1 The first ngpt::modified_julian_day (difference is d1-d2)
/// @param[in] d2 The second ngpt::modified_julian_day (difference is d1-d2)
/// @return       The difference between d1 and d2 in the second type S (e.g.
///               ngpt::seconds, ngpt::milliseconds, etc)
///
/// @note 
///       - The difference between two Modified Julian Days is always an integral
///         number of days (no fractional part!).
///       - If the code is compiled with the switch USE_DATETIME_CHECKS, then the
///         difference can only produce a positive amount; hence be sure that
///         d1>=d2
template<typename S,
        typename = std::enable_if_t<S::is_of_sec_type>
        >
    S
    mjd_sec_diff(modified_julian_day d1, modified_julian_day d2) noexcept
{
    modified_julian_day d {d1-d2};
    return S{d.as_underlying_type() * S::max_in_day};
}

/// For user-defined literals, i am going to replace long with
/// unsigned long long int.
namespace ddetail { using ulli = unsigned long long int; }

/// A year can be constructed via "_Y".
constexpr year
operator "" _Y(ddetail::ulli i) noexcept
{ return year{static_cast<year::underlying_type>(i)}; }

/// A month can be constructed via "_M".
constexpr month
operator "" _M(ddetail::ulli i) noexcept
{ return month{static_cast<month::underlying_type>(i)}; }

/// A day of month can be constructed via "_D".
constexpr day_of_month
operator "" _D(ddetail::ulli i) noexcept
{ return day_of_month{static_cast<day_of_month::underlying_type>(i)}; }

/// An hour can be constructed via "_h".
constexpr hours
operator "" _h(ddetail::ulli i) noexcept
{ return hours{static_cast<hours::underlying_type>(i)}; }

/// A minute can be constructed via "_m".
constexpr minutes
operator "" _m(ddetail::ulli i) noexcept
{ return minutes{static_cast<minutes::underlying_type>(i)}; }

/// Seconds can be constructed via "_sec".
constexpr seconds
operator "" _sec(ddetail::ulli i) noexcept
{ return seconds{static_cast<seconds::underlying_type>(i)}; }

/// MilliSeconds can be constructed via "_millisec".
constexpr milliseconds
operator "" _millisec(ddetail::ulli i) noexcept
{ return milliseconds{static_cast<milliseconds::underlying_type>(i)}; }

/// MicroSeconds can be constructed via "_microsec".
constexpr microseconds
operator "" _microsec(ddetail::ulli i) noexcept
{ return microseconds{static_cast<microseconds::underlying_type>(i)}; }

} // end namespace

#endif // define DATETIME
