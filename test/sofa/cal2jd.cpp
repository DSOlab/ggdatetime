#include "dtfund.hpp"
#include "sofa.h"
#include <cassert>
#include <random>

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
    double jd0, jd1;
    long mjd;
    try {
      mjd = dso::core::cal2mjd(iy, im, id);
      /* non-erronuous date, result should be the same as SOFA */
      assert(iauCal2jd(iy, im, id, &jd0, &jd1) == 0);
      assert(jd0 == dso::MJD0_JD);
      assert(jd1 == (double)mjd);
    } catch (std::exception &) {
      /* erronuous date, SOFA should verify that! */
      assert(iauCal2jd(iy, im, id, &jd0, &jd1) < 0);
    }
    if (i % 10)
      printf("%ld/%ld\r", i, num_tests);
  }

  printf("All tests ok!\n");
  return 0;
}
