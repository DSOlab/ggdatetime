#include "dtfund.hpp"

using dso::hours;

int main() {
  hours h1(2023);

  /* not allowed */
  h1 == 2024;

  return 1;
}
