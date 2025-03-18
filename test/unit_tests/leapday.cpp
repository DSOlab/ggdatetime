#include "datetime_write.hpp"
#include "tpdate.hpp"
#include <array>
#include <cassert>
#include <cstdio>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;
constexpr const double S = nanoseconds::sec_factor<double>();

const std::array<ymd_date, 27> leap_insertion_dates = {
    ymd_date{year(1972), month(6), day_of_month(30)},
    ymd_date{year(1972), month(12), day_of_month(31)},
    ymd_date{year(1973), month(12), day_of_month(31)},
    ymd_date{year(1974), month(12), day_of_month(31)},
    ymd_date{year(1975), month(12), day_of_month(31)},
    ymd_date{year(1976), month(12), day_of_month(31)},
    ymd_date{year(1977), month(12), day_of_month(31)},
    ymd_date{year(1978), month(12), day_of_month(31)},
    ymd_date{year(1979), month(12), day_of_month(31)},
    ymd_date{year(1981), month(6), day_of_month(30)},
    ymd_date{year(1982), month(6), day_of_month(30)},
    ymd_date{year(1983), month(6), day_of_month(30)},
    ymd_date{year(1985), month(6), day_of_month(30)},
    ymd_date{year(1987), month(12), day_of_month(31)},
    ymd_date{year(1989), month(12), day_of_month(31)},
    ymd_date{year(1990), month(12), day_of_month(31)},
    ymd_date{year(1992), month(6), day_of_month(30)},
    ymd_date{year(1993), month(6), day_of_month(30)},
    ymd_date{year(1994), month(6), day_of_month(30)},
    ymd_date{year(1995), month(12), day_of_month(31)},
    ymd_date{year(1997), month(6), day_of_month(30)},
    ymd_date{year(1998), month(12), day_of_month(31)},
    ymd_date{year(2005), month(12), day_of_month(31)},
    ymd_date{year(2008), month(12), day_of_month(31)},
    ymd_date{year(2012), month(6), day_of_month(30)},
    ymd_date{year(2015), month(6), day_of_month(30)},
    ymd_date{year(2016), month(12), day_of_month(31)},
};

int main() {

  char buf1[64], buf2[64];

  for (auto const &d : leap_insertion_dates) {
    TwoPartDate tai(modified_julian_day(d).as_underlying_type());
    for (int i = 0; i < 86400 - 1; i++) {
      tai.add_seconds(FractionalSeconds(1e0));
    }
    TwoPartDateUTC utc(modified_julian_day(d).as_underlying_type());
    for (int i = 0; i < 86400 - 1; i++) {
      utc.add_seconds(FractionalSeconds(1e0));
    }

    /* we are now at 23:59:59 */
    assert(tai.imjd() == utc.imjd());
    assert(tai.sec_of_day<nanoseconds>() == utc.sec_of_day<nanoseconds>());

    /* store these dates */
    const auto tai2359(tai);
    const auto utc2359(utc);

    /* add one more seconds */
    tai.add_seconds(FractionalSeconds(1e0));
    utc.add_seconds(FractionalSeconds(1e0));
    assert(tai.imjd() == utc.imjd() + 1);
    /* TAI seconds should be 0 */
    assert(tai.sec_of_day<nanoseconds>() == 0e0 &&
           tai.sec_of_day<seconds>() == 0e0);
    /* UTC seconds should be 86400 */
    assert(utc.sec_of_day<nanoseconds>() == (double)nanoseconds::max_in_day);
    assert(utc.sec_of_day<seconds>() == (double)seconds::max_in_day);

    to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(tai, buf1);
    to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(utc, buf2);

    /* add one more seconds */
    tai.add_seconds(FractionalSeconds(1e0));
    utc.add_seconds(FractionalSeconds(1e0));
    assert(tai.imjd() == utc.imjd());
    /* UTC + 1[sec] = TAI */
    assert(utc.sec_of_day<nanoseconds>() + S == tai.sec_of_day<nanoseconds>());
    assert(utc.sec_of_day<seconds>() + 1e0 == tai.sec_of_day<seconds>());

    to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(tai, buf1);
    to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(utc, buf2);

    /* Datetime differences */
    auto tpd1 = tai - tai2359;
    assert(tpd1.imjd() == 0);
    assert(tpd1.seconds() == FractionalSeconds(2e0));

    tpd1 = utc - utc2359;
    assert(tpd1.imjd() == 0);
    assert(tpd1.seconds() == FractionalSeconds(2e0));

    /* One day after leap insertion */
    TwoPartDate tai00(modified_julian_day(d).as_underlying_type());
    TwoPartDate tai24(modified_julian_day(d).as_underlying_type() + 1);
    tpd1 = tai24 - tai00;
    assert(tpd1.imjd() == 1);
    assert(tpd1.seconds() == FractionalSeconds(0e0));

    TwoPartDateUTC utc00(modified_julian_day(d).as_underlying_type());
    TwoPartDateUTC utc24(modified_julian_day(d).as_underlying_type() + 1);
    tpd1 = utc24 - utc00;
    assert(tpd1.imjd() == 1);
    assert(tpd1.seconds() == FractionalSeconds(1e0));
  }

  return 0;
}
