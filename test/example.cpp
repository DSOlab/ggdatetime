#include <iostream>
#include <chrono>
#include <cassert>
#include <limits>

#include "dtfund.hpp"
#include "dtcalendar.hpp"
#include "datetime_read.hpp"

using namespace ngpt;

constexpr long MilliSec = 1000L;
constexpr long MicroSec = 1000000L;
constexpr long NanoSec  = 1000000000L;
constexpr long maxL     = std::numeric_limits<long int>::max();

int main()
{
    //
    // Implementation on this arch.
    // -----------------------------------------------------------------------
    //
    std::cout<<"\nSize of datetime class is "<< sizeof(datetime<seconds>) << " bytes.\n";
    std::cout<<"Max representable (long) int is " << maxL << "\n";
    static_assert(86400L*MilliSec < maxL, "-- MilliSeconds Overflow --");
    static_assert(86400L*MicroSec < maxL, "-- MicroSeconds Overflow --");
    static_assert(86400L*NanoSec < maxL, "-- NanoSeconds Overflow --");
    std::cout<<"A whole day fits in long for:\n";
    std::cout<<"\tMilliseconds: 86400 * 1000       = " << 86400*MilliSec <<", can fit about " << maxL/(86400*MilliSec) <<" days\n";
    std::cout<<"\tMicroseconds: 86400 * 1000000    = " << 86400*MicroSec <<", can fit about " << maxL/(86400*MicroSec) <<" days\n";
    std::cout<<"\tNanoseconds:  86400 * 1000000000 = " << 86400*NanoSec  <<", can fit about " << maxL/(86400*NanoSec) <<" days\n";
    std::cout<<"Part A -- OK\n\n";

    //
    // Let's try the user defined literals (these need namespace ngpt)
    // -----------------------------------------------------------------------
    //
    std::cout<<"Testing constructors\n";
    auto yr1 = 2016_Y; // year  = 2016
    auto mt1 = 12_M;   // month = 12
    auto dm1 = 30_D;   // day of month = 30
    auto hr1 = 12_h;   //
    auto mn1 = 59_m;   //
    constexpr auto ss1 = 30_sec; //
    constexpr auto sm1 = 30000_millisec;    //
    constexpr auto sm2 = 30000000_microsec; //
    static_assert( sm1 == (milliseconds)sm2, "-- Terribly Wrong --" );
    static_assert( ss1 == (seconds)sm1 && ss1 == (seconds)sm2, "-- Terribly Wrong --" );
    std::cout<<"Part B -- OK\n\n";
    
    //
    // Let's try reading dates
    // -----------------------------------------------------------------------
    //
    /*char* __end = nullptr;*/
    std::cout<<"Testing reading dates\n";
    const char* date1_str = "2015-12-30 12:09:30";
    const char* date2_str = "2015/12/30 12:09:30";
    const char* date3_str = "2015-12-30 12:09:30.000011";
    const char* date4_str = "2015 12 30 12 9 30";
    datetime<seconds> dfs1 = ngpt::strptime_ymd_hms<seconds>(date1_str/*, &__end*/);
    datetime<seconds> dfs2 = ngpt::strptime_ymd_hms<seconds>(date2_str/*, &__end*/);
    datetime<seconds> dfs3 = ngpt::strptime_ymd_hms<seconds>(date3_str/*, &__end*/);
    datetime<seconds> dfs4 = ngpt::strptime_ymd_hms<seconds>(date4_str/*, &__end*/);
    datetime<seconds> dfs1_ {year(2015), month(12), day_of_month(30), hours(12), minutes(9), seconds(30)};
    assert( dfs1 == dfs1_ && dfs1 == dfs2 && dfs2 == dfs3 && dfs3 == dfs4 );
    
    auto dfs5 = ngpt::strptime_ymd_hms<microseconds>(date3_str);
    datetime<microseconds> dfs5_ {year(2015), month(12), day_of_month(30), hours(12), minutes(9), microseconds(30000011)};
    assert( dfs5 == dfs5_ );
    std::cout << "dfs5  = " << dfs5.stringify() << " (" << dfs5.secs() << ")\n";

    const char* date6_str = "2015-12-30";
    datetime<seconds> dfs6 = ngpt::strptime_ymd<seconds>(date6_str);
    const char* date7_str = "2015-12-30 0 0 0";
    datetime<seconds> dfs7 = ngpt::strptime_ymd<seconds>(date7_str);
    datetime<seconds> dfs7_ {year(2015), month(12), day_of_month(30), hours(0), minutes(0), seconds(0)};
    assert( dfs6 == dfs7 && dfs7 == dfs7_ );
    const char* date8_str = "2015 Dec 30 12 9 30";
    const char* date9_str = "2015/DEC/30 12 9 30";
    const char* date10_str= "2015-DEC-30 12 9 30";
    try{
        datetime<seconds> dfs8 = ngpt::strptime_yod_hms<seconds>(date8_str);
        std::cerr<<dfs8.stringify();
    }catch (std::invalid_argument& e) {
        e.what();
        std::cerr<<"\nInvalid date 1\n";
    }
    try{
        datetime<seconds> dfs9 = ngpt::strptime_yod_hms<seconds>(date9_str);
        std::cerr<<dfs9.stringify();
    }catch (std::invalid_argument& e) {
        e.what();
        std::cerr<<"\nInvalid date 2\n";
    }
    try{
        datetime<seconds> dfs10= ngpt::strptime_yod_hms<seconds>(date10_str);
        std::cerr<<dfs10.stringify();
    }catch (std::invalid_argument& e) {
        e.what();
        std::cerr<<"\nInvalid date 3\n";
    }
    
    // assert( dfs1 == dfs1_ && dfs1 == dfs8 && dfs8 == dfs9 && dfs9 == dfs10 );
    std::cout<<"Part C -- OK\n\n";
    
    //
    // Seconds, MicroSec and Millisec
    // -----------------------------------------------------------------------
    //
    seconds sec1 {10};
    milliseconds mlsec1 {10};
    microseconds mcsec1 {10};
    seconds sec2 {mcsec1};        // casting microsec to sec is allowed
    seconds sec3 {mlsec1};        // casting millisec to sec is allowed
    milliseconds mlsec2 {mcsec1}; // casting microsec to millisec is allowed
    // However, it is not allowed to cast from lower to higher precsision, e.g.
    // milliseconds ml1 {sec1};   // ERROR!
    // microseconds mc1 {mlsec1}; // ERROR!

    //
    // Construction of datetime objects
    // -----------------------------------------------------------------------
    //
    datetime<seconds> d2(year(2015), month(12), day_of_month(30));
    std::cout << "d2  = " << d2.stringify() << " (" << d2.secs() << ")\n";
    
    // the following won't compile; the template parameter can be
    // seconds, milliseconds or microseconds.
    // datetime<year> d3(year(2015), month(12), day_of_month(30)); // ERROR
 
    // this is fine; microseconds to seconds is allowed (BUT fractional sec 
    // are ignored!)
    datetime<seconds> d21 (year(2015), month(12), day_of_month(30),
        milliseconds(MilliSec));
    std::cout << "d21 = " << d21.stringify() << " (" << d21.secs() << ")\n";
    // the opposite however id not allowed!
    // datetime<microseconds> d5(year(2015), month(12), day_of_month(30),
    //     seconds(100)); ERROR!
    // we can also use time (i.e. hours, minutes, etc..)
    datetime<seconds> d22 (year(2015), month(12), day_of_month(30), hours(12),
        minutes(50), seconds(30));
    std::cout << "d22 = " << d22.stringify() << " (" << d22.secs() << ")\n";
    // or
    datetime<seconds> d23 (year(2015), month(12), day_of_month(30), hours(12),
        minutes(50), microseconds(30000001));
    std::cout << "d23 = " << d23.stringify() << " (" << d23.secs() << ")\n";
    // or
    datetime<microseconds> d24 (year(2015), month(12), day_of_month(30), hours(12),
        minutes(50), microseconds(30000001));
    std::cout << "d24 = " << d24.stringify() << " (" << d24.secs() << ")\n";
    // but not (seconds cannot be cast to milliseconds) ERROR!
    // datetime<milliseconds> d25 (year(2015), month(12), day_of_month(30), hours(12),
    //     minutes(50), seconds(30000001));
    // std::cout << "d25 = " << d25.stringify() << " (" << d25.secs() << ")\n";

    // this is fine; use fractional seconds (which are skipped!)
    datetime<seconds> d3 (year(2015), month(12), day_of_month(30), hours(12), 
        minutes(50), 30.001234);
    std::cout << "d3  = " << d3.stringify() << " (" << d3.secs() << ")\n";
    // or, for bigger accuracy ..
    datetime<microseconds> d31 (year(2015), month(12), day_of_month(30), hours(12),
        minutes(5), 30.0000010);
    std::cout << "d31 = " << d31.stringify() << " (" << d31.secs() << ")\n";
    
    //
    // Manipulation of datetime objects
    // -----------------------------------------------------------------------
    //
    d2.add_seconds(seconds(10));

    std::cout<<"\n\nSequentialy adding seconds to a date.\n";
    std::chrono::steady_clock::time_point begin, end;
    double mjd1=d2.as_mjd(),
           mjd2;

    std::cout <<"d2: " << d2.stringify() << ", MJD = " << d2.as_mjd() << "\n";
    begin = std::chrono::steady_clock::now();
    for (int i = 0; i < 86400 * 2.5; ++i) { /* sequentialy add 2+1/2 days */
        d2.add_seconds( seconds(1L) );
    }
    end = std::chrono::steady_clock::now();
    mjd2 = d2.as_mjd();
    std::cout << "Adding 2+1/2 days in to d2 takes about " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microsec.\n";
    std::cout << "New mjd is " << mjd2 << "\n";
    std::cout << "Difference = " << mjd2 - mjd1 << " days, or " << (mjd2 - mjd1)*86400.0 << " seconds";
    std::cout << " or " << (mjd2 - mjd1)*86400000.0 << " milliseconds.\n";
    std::cout << "The folowing number should be zero: " << ((mjd2 - mjd1)*86400000.0 - 2.5*86400000.0) << ", is it? " << std::boolalpha << (((mjd2 - mjd1)*86400000.0 - 2.5*86400000.0) == 0.0e0) << "\n";
    std::cout << "d2: " << d2.stringify() << ", MJD = " << d2.as_mjd() << "\n";

    std::cout << "\n";
    return 0;   
}
