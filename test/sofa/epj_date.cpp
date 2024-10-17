#include "calendar.hpp"
#include "sofa.h"
#include "tpdate.hpp"
#include <random>

using namespace dso;

constexpr const double EPSILON = 1e-12; /* fractional days */
template <typename T>
inline bool fequal(const T &a, const T &b, double eps = EPSILON) noexcept {
  if (std::abs(a - b) <= eps)
    return true;
  return std::abs(a - b) <= (eps * std::max(std::abs(a), std::abs(b)));
}

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

#ifdef DEBUG
  double maxdiffs[10], avediffs[10];
  for (int i = 0; i < 10; i++) {
    maxdiffs[i] = 0e0;
    avediffs[i] = 0e0;
  }
  int n = 0;
#endif
  for (long i = 0; i < num_tests; i++) {
    const int iy = ydstr(gen);
    const int im = mdstr(gen);
    const int id = ddstr(gen);
    const int ms = msstr(gen);
    try {
      long imjd = core::cal2mjd(iy, im, id);
      const TwoPartDate d(imjd, FractionalSeconds(ms / 1e3));

      /* TwoPartDate to Julian Epoch and back */
      double epj_lib;
      {
        const double je = d.epj();
        TwoPartDate di = epj2tpd(je);
        assert(d.imjd() - di.imjd() == 0);
#ifdef DEBUG
        if (std::abs(d.seconds() - di.seconds()) > maxdiffs[0])
          maxdiffs[0] = std::abs(d.seconds() - di.seconds());
        avediffs[0] += std::abs(d.seconds() - di.seconds());
#endif
        epj_lib = je;

        /* compare initial to resulting dates */
        assert(fequal(d.seconds(), di.seconds(), 1e-5));
      }

      /* do the same with SOFA */
      double epj_sofa;
      {
        const double jd1 = d.seconds() / SEC_PER_DAY;
        const double jd2 = d.imjd() + MJD0_JD;
        const double sje = iauEpj(jd2, jd1);
        double jd11, jd21;
        iauEpj2jd(sje, &jd11, &jd21);
#ifdef DEBUG
        const double dsec = ((jd2 - jd11) + (jd1 - jd21)) * SEC_PER_DAY;
        if (std::abs(dsec) > maxdiffs[1])
          maxdiffs[1] = std::abs(dsec);
        avediffs[1] += std::abs(dsec);
#endif
        epj_sofa = sje;
      }

      /* compare SOFA's epj to lib's epj */
      assert(fequal(epj_lib, epj_sofa));
#ifdef DEBUG
      ++n;
#endif
    } catch (std::exception &) {
      ;
    }
  }

#ifdef DEBUG
  for (int i = 0; i < 2; i++) {
    printf("[%d]: MaxDiff=%+.6f[sec] AveDiff=%.6f[sec]\n", i, maxdiffs[i],
           avediffs[i] / n);
  }
#endif

  return 0;
}
