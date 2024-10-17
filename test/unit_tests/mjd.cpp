#include "calendar.hpp"
#include <cassert>
#include <cmath>
#include <random>

using namespace dso;

/* number of tests to perform (pre template parameter) */
long num_tests = 1'000'000;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */

  for (long i = 0; i < num_tests; i++) {
    const int iy = ydstr(gen);
    const int im = mdstr(gen);
    const int id = ddstr(gen);
    ymd_date ymd{year(iy), month(im), day_of_month(id)};
    if (ymd.is_valid()) {
      ydoy_date ydoy(ymd);
      /* ymd_date to MJD */
      const modified_julian_day d1(ymd);
      /* ydoy_date to MJD */
      const modified_julian_day d2(ydoy);
      /* assert results */
      assert(d1 == d2);
    }
  }

  return 0;
}
