#include "dtfund.hpp"

using dso::year;

int main() {
  year y1(2023);

  /* not allowed */
  y1 == 2024;

  return 1;
}
