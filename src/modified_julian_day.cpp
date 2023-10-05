#include "dtfund.hpp"

dso::ydoy_date dso::modified_julian_day::to_ydoy() const noexcept {
  const long days_fr_jan1_1901 = m_mjd - dso::JAN11901;
  const long num_four_yrs = days_fr_jan1_1901 / 1461L;
  const long years_so_far = 1901L + 4 * num_four_yrs;
  const long days_left = days_fr_jan1_1901 - 1461 * num_four_yrs;
  const long delta_yrs = days_left / 365 - days_left / 1460;

  return dso::ydoy_date(
      static_cast<year::underlying_type>(years_so_far + delta_yrs),
      static_cast<day_of_year::underlying_type>(days_left - 365 * delta_yrs +
                                                1));
}
