#include "dtfund.hpp"
#include <stdexcept>

dso::ymd_date::ymd_date(const dso::ydoy_date &ydoy) {
  if (!ydoy.is_valid()) {
    throw std::invalid_argument(
        "[ERROR] Tring to compute year/month/day from an invalid "
        "year/day_of_year instance (traceback:" +
        std::string(__func__) + ")\n");
  }
  const auto ymd = ydoy.to_ymd();
  __year = ymd.yr();
  __month = ymd.mn();
  __dom = ymd.dm();
}


dso::ydoy_date dso::ymd_date::to_ydoy() const {
  if (!is_valid()) {
    throw std::invalid_argument(
        "[ERROR] Trying to compute year/day_of_year from an invalid "
        "year/month/day instance (traceback:" +
        std::string(__func__) + ")\n");
  }
  int leap = yr().is_leap();
  int md = mn().as_underlying_type() - 1;
  return dso::ydoy_date(yr(), dso::day_of_year(core::month_day[leap][md] +
                                               dm().as_underlying_type()));
}
