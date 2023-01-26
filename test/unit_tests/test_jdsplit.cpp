#include "dtcalendar.hpp"
#include <cassert>
#include <datetime/dtcalendar.hpp>
#include <random>
#include <cstdio>
#include <limits>

/*
 * Unit tests for spliting a TwoPartDate instance
 */

constexpr const int NumTests = 100000;

inline bool approx_equal(const dso::TwoPartDate &d1,
                         const dso::TwoPartDate &d2) {
  const double a = d1._big + d1._small;
  const double b = d2._big + d2._small;
  return a == b || std::abs(a - b) < std::abs(std::min(a, b)) *
                                         std::numeric_limits<double>::epsilon();
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    fprintf(stderr, "Ignoring all input!\n");
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution day(29629e0, 66154e0); // 1940 to 2040 in MJD
  std::uniform_real_distribution fday(0e0, 1e0); // 1940 to 2040 in MJD

  for (int i=0; i<NumTests; i++) {
    // random MJD date
    dso::TwoPartDate d(day(gen), fday(gen));
    d.normalize();
    
    // JdSplitMethod {JD, J2000, MJD, DT};
    auto jd1 = d.jd_split<dso::TwoPartDate::JdSplitMethod::JD>();
    auto jd2 = d.jd_split<dso::TwoPartDate::JdSplitMethod::J2000>();
    auto jd3 = d.jd_split<dso::TwoPartDate::JdSplitMethod::MJD>();
    auto jd4 = d.jd_split<dso::TwoPartDate::JdSplitMethod::DT>();
    //printf("%20.12f %20.12f %20.12f %20.12f\n", jd1._big + jd1._small,
    //       jd2._big + jd2._small, jd3._big + jd3._small, jd4._big + jd4._small);

    assert(approx_equal(jd1,jd2));
    assert(approx_equal(jd1,jd3));
    assert(approx_equal(jd1,jd4));
  }

  printf("All tests passed in %s\n", argv[0]);
  return 0;
}
