#include "dtfund.hpp"
#include "sofa.h"
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
  /* range for JD, 1972to 2035 */
  std::uniform_int_distribution<> jddstr(2441317, 2464328);

  int iy, id, im;
  double fd;
  for (long i = 0; i < num_tests; i++) {
    /* test JD */
    const double ijd = jddstr(gen) + .5e0;
    /* use SOFA to transform to YMD */
    assert(!iauJd2cal(ijd, 0e0, &iy, &im, &id, &fd));
    /* transform it to a MJD */
    const int mjd = (ijd - dso::MJD0_JD);
    /* transform MJD to YMD */
    const auto ymd = modified_julian_day(mjd).to_ymd();
    /* compare results */
    assert(ymd.yr().as_underlying_type() == iy);
    assert(ymd.mn().as_underlying_type() == im);
    assert(ymd.dm().as_underlying_type() == id);
    if (i % 10)
      printf("%ld/%ld\r", i, num_tests);
  }

  printf("All tests ok!\n");
  return 0;
}
