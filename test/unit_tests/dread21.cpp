#include "datetime_read.hpp"
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
constexpr const nanoseconds N1S =
    nanoseconds(nanoseconds::sec_factor<nanoseconds::underlying_type>());
constexpr const nanoseconds::underlying_type F =
    nanoseconds::sec_factor<nanoseconds::underlying_type>();

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
    datetime<nanoseconds> tai{modified_julian_day(d)};
    tai.add_seconds(seconds(86400 - 1));
    datetime_utc<nanoseconds> utc{modified_julian_day(d)};
    utc.add_seconds(seconds(86400 - 1));

    /* we are now at 23:59:59 */
    assert(tai.imjd() == utc.imjd());
    assert(tai.sec() == utc.sec());
    {
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(tai, buf1);
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(utc, buf2);
      datetime<nanoseconds> d1 =
          from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(buf1);
      assert(d1 == tai);
      datetime_utc<nanoseconds> d2 =
          from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(
              buf2);
      assert(d2 == utc);
    }

    /* store these dates */
    const auto tai2359(tai);
    const auto utc2359(utc);

    /* add one more second; TAI advances to next day, but UTC has the leap
     * second! */
    tai.add_seconds(seconds(1e0));
    utc.add_seconds(seconds(1e0));
    assert(tai.imjd() == utc.imjd() + modified_julian_day(1));
    /* TAI seconds should be 0 */
    assert(tai.sec() == nanoseconds(0));
    /* UTC seconds should be 86400 */
    assert(utc.sec() == (nanoseconds)nanoseconds::max_in_day);
    {
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(tai, buf1);
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(utc, buf2);
      datetime<nanoseconds> d1 =
          from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(buf1);
      assert(d1 == tai);
      datetime_utc<nanoseconds> d2 =
          from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(
              buf2);
      assert(d2 == utc);
    }

    /* add one more seconds */
    tai.add_seconds(seconds(1)); // 2 seconds into day
    utc.add_seconds(seconds(1)); // 1 second into day
    assert(tai.imjd() == utc.imjd());
    /* UTC + 1[sec] = TAI */
    assert(utc.sec() + N1S == tai.sec());
    {
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(tai, buf1);
      to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(utc, buf2);
      datetime<nanoseconds> d1 =
          from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(buf1);
      assert(d1 == tai);
      datetime_utc<nanoseconds> d2 =
          from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF, nanoseconds>(
              buf2);
      assert(d2 == utc);
    }

    /* Datetime differences */
    auto tpd1 = tai - tai2359;
    assert(tpd1.days() == 0);
    assert(tpd1.sec() == nanoseconds(2 * F));

    tpd1 = utc - utc2359;
    assert(tpd1.days() == 0);
    assert(tpd1.sec() == nanoseconds(2 * F));

    /* One day after leap insertion */
    datetime<nanoseconds> tai00{modified_julian_day(d)};
    datetime<nanoseconds> tai24{modified_julian_day(d) +
                                modified_julian_day(1)};
    tpd1 = tai24 - tai00;
    assert(tpd1.days() == 1);
    assert(tpd1.signed_sec() == nanoseconds(0));

    datetime_utc<nanoseconds> utc00{modified_julian_day(d)};
    datetime_utc<nanoseconds> utc24{modified_julian_day(d) +
                                    modified_julian_day(1)};
    tpd1 = utc24 - utc00;
    assert(tpd1.days() == 1);
    assert(tpd1.signed_sec() == nanoseconds(1 * F));
  }

  return 0;
}
