#include "dtfund.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

using dso::day_of_month;
using dso::month;
using dso::year;
using dso::ymd_date;
using dso::ydoy_date;
using dso::day_of_year;

/* one day in a Julian year, as a fractional year */
constexpr const double dayasyear = 1e0/dso::DAYS_IN_JULIAN_YEAR;

int main() {

  for (int y = 1804; y <= 2400; y++) {
    if (year(y).is_leap()) {
      ydoy_date yd365 (year(y), day_of_year(365));
      ydoy_date yd366 (year(y), day_of_year(366));
      const double fy365 =
          yd365.fractional_years<dso::core::YearCount::Julian>();
      const double fy366 =
          yd366.fractional_years<dso::core::YearCount::Julian>();
    }
  }

  return 0;
}
