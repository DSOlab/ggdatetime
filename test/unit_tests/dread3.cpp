#include "datetime_read.hpp"
#include "datetime_write.hpp"
#include "tpdate.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <type_traits>

using namespace dso;

const std::array<ymd_date, 3> leap_insertion_dates = {
    /*
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
      */
    ymd_date{year(2012), month(6), day_of_month(30)},
    ymd_date{year(2015), month(6), day_of_month(30)},
    ymd_date{year(2016), month(12), day_of_month(31)},
};

const std::array<const char *, 3> leap_insertion_dates_str = {
    /*    "1972-06-30",
        "1972-12-31",
        "1973-12-31",
        "1974-12-31",
        "1975-12-31",
        "1976-12-31",
        "1977-12-31",
        "1978-12-31",
        "1979-12-31",
        "1981-06-30",
        "1982-06-30",
        "1983-06-30",
        "1985-06-30",
        "1987-12-31",
        "1989-12-31",
        "1990-12-31",
        "1992-06-30",
        "1993-06-30",
        "1994-06-30",
        "1995-12-31",
        "1997-06-30",
        "1998-12-31",
        "2005-12-31",
        "2008-12-31",
        */
    "2012-06-30",
    "2015-06-30",
    "2016-12-31",
};

const char *s_235959 = " 23:59:59";
const char *s_235959f = " 23:59:59.000000000";
const char *s_2359599 = " 23:59:59.999999999";
const char *s_235960 = " 23:59:60";
const char *s_2359600 = " 23:59:60.000000000";

constexpr const double EPSILON = 1e-12;
/* see https://embeddeduse.com/2019/08/26/qt-compare-two-floats/ */
template <typename T> /* T is either TwoPartDate or TwoPartDateUTC */
inline bool fequal(const T &a, const T &b, double seceps = EPSILON) noexcept {
  if (a.imjd() != b.imjd())
    return false;
  if (std::abs(a.seconds() - b.seconds()) <= seceps)
    return true;
  return std::abs(a.seconds() - b.seconds()) <=
         (seceps * std::max(std::abs(a.seconds()), std::abs(b.seconds())));
}
/* see https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon */
template <class T>
std::enable_if_t<not std::numeric_limits<T>::is_integer, bool>
equal_within_ulps(T x, T y, std::size_t n) {
  const T m = std::min(std::fabs(x), std::fabs(y));
  const int exp = m < std::numeric_limits<T>::min()
                      ? std::numeric_limits<T>::min_exponent - 1
                      : std::ilogb(m);
  return std::fabs(x - y) <=
         n * std::ldexp(std::numeric_limits<T>::epsilon(), exp);
}

constexpr const int BUFSZ = 64;
inline char *reset_buffer(char *buf) {
  std::memset(buf, '\0', BUFSZ);
  return buf;
}

int main() {

  char buf1[64];

  int it = 0;
  double err = 0e0;
  for (auto const &d : leap_insertion_dates) {

    /* one seconds before midnight */
    TwoPartDate tai(modified_julian_day(d).as_underlying_type(),
                    FractionalSeconds(0e0));
    tai.add_seconds(86400 - 1);
    {
      TwoPartDate d1;
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_235959);
      d1 = from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(d1 == tai);

      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_235959f);
      d1 = from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(d1 == tai);

      /* reach 23:59:59.999 999 999 */
      err = 0e0;
      for (int i = 0; i < (int)(1e9) - 1; i++)
        tai.add_seconds(1e-9, err);
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_2359599);
      d1 = from_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(fequal(d1, tai));
      assert(equal_within_ulps(d1.seconds(), tai.seconds(), 1));

      /* one more nanosec will take to the next day */
      tai.add_seconds(1e-9, err);
      assert(tai.imjd() == d1.imjd() + 1);
      d1 = TwoPartDate(modified_julian_day(d).as_underlying_type() + 1,
                       FractionalSeconds(0e0));
      assert(fequal(d1, tai));
      assert(equal_within_ulps(d1.seconds(), tai.seconds(), 1));

      // printf("%d %.15f\n", tai.imjd(), tai.seconds());
      // printf("%d %.15f\n", d1.imjd(), d1.seconds());
    }

    TwoPartDateUTC utc(modified_julian_day(d).as_underlying_type(),
                       FractionalSeconds(0e0));
    utc.add_seconds(86400 - 1);
    {
      TwoPartDateUTC d1;
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_235959);
      d1 = from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(d1 == utc);

      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_235959f);
      d1 = from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(d1 == utc);

      /* reach 23:59:59.999 999 999 */
      err = 0e0;
      for (int i = 0; i < (int)(1e9) - 1; i++)
        utc.add_seconds(1e-9, err);
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_2359599);
      d1 = from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(fequal(d1, utc));
      assert(equal_within_ulps(d1.seconds(), utc.seconds(), 1));

      /* one more nanosec will take to next second, NOT the next day */
      utc.add_seconds(1e-9, err);
      assert(utc.imjd() == d1.imjd());
      /* equal to YYYY-MM-DD 23:59:60 */
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_235960);
      d1 = from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(fequal(d1, utc));
      assert(equal_within_ulps(d1.seconds(), utc.seconds(), 1));
      /* equal to YYYY-MM-DD 23:59:60.000000000 */
      std::strcat(reset_buffer(buf1), leap_insertion_dates_str[it]);
      std::strcat(buf1, s_2359600);
      d1 = from_utc_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(buf1);
      assert(fequal(d1, utc));
      assert(equal_within_ulps(d1.seconds(), utc.seconds(), 1));
    }

    /* augment string index */
    ++it;
  }

  return 0;
}
