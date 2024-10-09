#include "dtfund.hpp"

namespace {
  /** avoid magic numbers */
  constexpr const int DAYS_IN_YEAR = 365;
} /* unnamed namespace */

dso::ydoy_date dso::modified_julian_day::to_ydoy() const noexcept {
  const int days_fr_jan1_1901 = m_mjd - dso::JAN11901;
  const int num_four_yrs = days_fr_jan1_1901 / 1461;
  const int years_so_far = 1901 + 4 * num_four_yrs;
  const int days_left = days_fr_jan1_1901 - 1461 * num_four_yrs;
  const int delta_yrs = days_left / DAYS_IN_YEAR - days_left / 1460;

  return {dso::year(years_so_far + delta_yrs),
          dso::day_of_year(days_left - DAYS_IN_YEAR * delta_yrs + 1)};
}
