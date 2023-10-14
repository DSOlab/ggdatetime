#include "dtfund.hpp"

using dso::hours;

int main() {
  hours h1(2023);

  /* not allowed */
  const auto h = h1 + 1;

  return 1;
}
