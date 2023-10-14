#include "dtfund.hpp"

using dso::hours;

int main() {
  hours h1(2023);

  /* not allowed */
  [[maybe_unused]] auto x = h1 + dso::minutes(1);

  return 1;
}
