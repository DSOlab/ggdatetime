#include "calendar.hpp"
#include <random>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;

constexpr const int MIN_MJD = 41317 - 5;
constexpr const int MAX_MJD = 57754 + 5;
constexpr const std::array<ymd_date, 28> changes = {{
    ymd_date{year(1972), month(1), day_of_month(1)},
    ymd_date{year(1972), month(7), day_of_month(1)},
    ymd_date{year(1973), month(1), day_of_month(1)},
    ymd_date{year(1974), month(1), day_of_month(1)},
    ymd_date{year(1975), month(1), day_of_month(1)},
    ymd_date{year(1976), month(1), day_of_month(1)},
    ymd_date{year(1977), month(1), day_of_month(1)},
    ymd_date{year(1978), month(1), day_of_month(1)},
    ymd_date{year(1979), month(1), day_of_month(1)},
    ymd_date{year(1980), month(1), day_of_month(1)},
    ymd_date{year(1981), month(7), day_of_month(1)},
    ymd_date{year(1982), month(7), day_of_month(1)},
    ymd_date{year(1983), month(7), day_of_month(1)},
    ymd_date{year(1985), month(7), day_of_month(1)},
    ymd_date{year(1988), month(1), day_of_month(1)},
    ymd_date{year(1990), month(1), day_of_month(1)},
    ymd_date{year(1991), month(1), day_of_month(1)},
    ymd_date{year(1992), month(7), day_of_month(1)},
    ymd_date{year(1993), month(7), day_of_month(1)},
    ymd_date{year(1994), month(7), day_of_month(1)},
    ymd_date{year(1996), month(1), day_of_month(1)},
    ymd_date{year(1997), month(7), day_of_month(1)},
    ymd_date{year(1999), month(1), day_of_month(1)},
    ymd_date{year(2006), month(1), day_of_month(1)},
    ymd_date{year(2009), month(1), day_of_month(1)},
    ymd_date{year(2012), month(7), day_of_month(1)},
    ymd_date{year(2015), month(7), day_of_month(1)},
    ymd_date{year(2017), month(1), day_of_month(1)},
}};

int main() {
  for (const auto &d : changes) {
    const modified_julian_day mjd(d);
    assert(mjd.is_leap_insertion_day() == 0);
    const modified_julian_day mjdm1 = mjd - modified_julian_day(1);
    assert(mjdm1.is_leap_insertion_day() == 1);
    const modified_julian_day mjdm2 = mjd - modified_julian_day(2);
    assert(mjdm2.is_leap_insertion_day() == 0);
    const modified_julian_day mjdp1 = mjd + modified_julian_day(1);
    assert(mjdp1.is_leap_insertion_day() == 0);
  }

  constexpr const modified_julian_day mjd{std::get<0, ymd_date, 28>(changes)};
  static_assert(mjd.is_leap_insertion_day() == 0);
  constexpr const modified_julian_day mjdm1{mjd - modified_julian_day(1)};
  static_assert(mjdm1.is_leap_insertion_day() == 1);

  return 0;
}
