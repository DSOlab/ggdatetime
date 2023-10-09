#include "dtfund.hpp"

constexpr dso::ymd_date dso::ydoy_date::to_ymd() const noexcept {
  ymd_date yd;
  int guess = static_cast<int>(__doy.as_underlying_type() * 0.032);
  int leap = __year.is_leap();
  int more = ((__doy.as_underlying_type() -
               dso::core::month_day[leap][guess + 1]) > 0);
  /* assign */
  yd.__year = __year;
  yd.__month = month{guess + more + 1};
  yd.__dom = day_of_month(__doy.as_underlying_type() -
                          dso::core::month_day[leap][guess + more]);
  return yd;
}
