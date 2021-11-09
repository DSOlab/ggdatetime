#include "dtfund.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cassert>

using dso::milliseconds;
using dso::microseconds;
using dso::nanoseconds;

int main() {
  std::vector<double> fsec = {1e0, 0e0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10, 1e-11, 1e-12};

  printf("%14s %14s %14s %14s\n", "F. Seconds", "From Milli", "From Micro", "From Nano");
  for (const auto fs : fsec) {
    milliseconds mls {static_cast<long>(fs * milliseconds::sec_factor<double>())};
    microseconds mrs {static_cast<long>(fs * microseconds::sec_factor<double>())};
    nanoseconds nns {static_cast<long>(fs * nanoseconds::sec_factor<double>())};

    printf("%.12f %.12f %.12f %.12f %E %E %E\n", fs, mls.to_fractional_seconds(), mrs.to_fractional_seconds(), nns.to_fractional_seconds(), mls.to_fractional_seconds(), mrs.to_fractional_seconds(), nns.to_fractional_seconds());

    if (fs >= 1e0/milliseconds::sec_factor<double>())
      assert(mls.to_fractional_seconds() == fs);
    else
      assert(mls.to_fractional_seconds() == 0e0);
    
    if (fs >= 1e0/microseconds::sec_factor<double>())
      assert(mrs.to_fractional_seconds() == fs);
    else
      assert(mrs.to_fractional_seconds() == 0e0);
    
    if (fs >= 1e0/nanoseconds::sec_factor<double>())
      assert(nns.to_fractional_seconds() == fs);
    else
      assert(nns.to_fractional_seconds() == 0e0);
  }

  printf("All checks for converting from/to fractional seconds OK\n");
  return 0;
}
