#include "dtfund.hpp"

dso::ydoy_date dso::ymd_date::to_ydoy() const noexcept {
  int leap = __year.is_leap();
  int md = __month.as_underlying_type() - 1;
  return dso::ydoy_date(__year, dso::day_of_year(core::month_day[leap][md] +
                                                 __dom.as_underlying_type()));
}
