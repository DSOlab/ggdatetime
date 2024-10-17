#include "date_integral_types.hpp"

dso::ymd_date dso::ydoy_date::to_ymd() const noexcept {
  int guess = __doy.as_underlying_type() * 0.032;
  int leap = yr().is_leap();
  int more =
      ((dy().as_underlying_type() - dso::core::month_day[leap][guess + 1]) > 0);
  /* assign */
  ymd_date yd;
  yd.yr() = yr();
  yd.mn() = month(guess + more + 1);
  yd.dm() = day_of_month(dy().as_underlying_type() -
                         dso::core::month_day[leap][guess + more]);
  return yd;
}
