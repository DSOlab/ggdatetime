#include "dtfund.hpp"
#include "sofa.h"
#include "tpdate.hpp"
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
  std::uniform_int_distribution<> msstr(
      0, 86400 * 1000); /* range for milliseconds of day */

  double maxdiffs[10], avediffs[10];
  for (int i=0; i<10; i++) {
    maxdiffs[i] = 0e0;
    avediffs[i] = 0e0;
  }
  int n=0;
  for (long i = 0; i < num_tests; i++) {
    const int iy = ydstr(gen);
    const int im = mdstr(gen);
    const int id = ddstr(gen);
    const int ms = msstr(gen);
    try {
      long imjd = core::cal2mjd(iy, im, id);
      const TwoPartDate d(imjd, (ms / 1e3));

      {
        const double je = d.epj1();
        TwoPartDate di = epjtp1(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[0]) maxdiffs[0] = std::abs(d.seconds()-di.seconds());
        avediffs[0] += std::abs(d.seconds()-di.seconds());
      }
      {
        const double je = d.epj2();
        TwoPartDate di = epjtp1(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[1]) maxdiffs[1] = std::abs(d.seconds()-di.seconds());
        avediffs[1] += std::abs(d.seconds()-di.seconds());
      }
      {
        const double je = d.epj3();
        TwoPartDate di = epjtp1(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[2]) maxdiffs[2] = std::abs(d.seconds()-di.seconds());
        avediffs[2] += std::abs(d.seconds()-di.seconds());
      }
      {
        const double je = d.epj1();
        TwoPartDate di = epjtp2(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[3]) maxdiffs[3] = std::abs(d.seconds()-di.seconds());
        avediffs[3] += std::abs(d.seconds()-di.seconds());
      }
      {
        const double je = d.epj2();
        TwoPartDate di = epjtp2(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[4]) maxdiffs[4] = std::abs(d.seconds()-di.seconds());
        avediffs[4] += std::abs(d.seconds()-di.seconds());
      }
      {
        const double je = d.epj3();
        TwoPartDate di = epjtp2(je);
        //printf("DeltaMjd = %+d DeltaSec=%+.6f[sec]\n", d.imjd() - di.imjd(),
        //       d.seconds() - di.seconds());
        assert(d.imjd() - di.imjd() == 0);
        if (std::abs(d.seconds()-di.seconds())>maxdiffs[5]) maxdiffs[5] = std::abs(d.seconds()-di.seconds());
        avediffs[5] += std::abs(d.seconds()-di.seconds());
      }
      
      /* do the same with SOFA */
      {
        const double jd1 = d.seconds() / SEC_PER_DAY;
        const double jd2 = d.imjd() + MJD0_JD;
        const double sje = iauEpj(jd2, jd1);
        double jd11, jd21;
        iauEpj2jd(sje, &jd11, &jd21);
        //printf("Sofa DeltaSec=%+.6f[sec]\n", (jd21 - d.as_mjd()) * 86400e0);
        const double dsec = ((jd2-jd11) + (jd1-jd21)) * SEC_PER_DAY;
        if (std::abs(dsec) > maxdiffs[6]) maxdiffs[6] = std::abs(dsec);
        avediffs[6] += std::abs(dsec);
      }
      ++i;
    } catch (std::exception &) {
      ;
    }
    if (i % 10)
      printf("%ld/%ld\r", i, num_tests);
  }

  for (int i=0; i<6; i++) {
    printf("[%d]: MaxDiff=%+.6f[sec] AveDiff=%.6f[sec]\n", i, maxdiffs[i], avediffs[i]/n);
  }

  return 0;
}
