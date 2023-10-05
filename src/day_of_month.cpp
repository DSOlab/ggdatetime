#include "dtfund.hpp"

constexpr bool dso::day_of_month::is_valid(dso::year y,
                                           dso::month m) const noexcept {
  if (m_dom < 1 || m_dom >= 32 || (!m.is_valid()))
    return false;

  /* If February in a leap year, 1, otherwise 0 */
  int ly((m == dso::month(2)) && y.is_leap());

  /* Validate day, taking into account leap years */
  return (m_dom <= dso::core::mtab[m.as_underlying_type() - 1] + ly);
}
