#include "dtcalendar.hpp"
#include "sofa.h"
#include <cstdlib>
#include <cerrno>
#include <random>
#include <cassert>
#include <cstdio>

using SecIntType = dso::nanoseconds::underlying_type;

int num_tests = 10000;

std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> ydstr(1990, 2050); // range for years
std::uniform_int_distribution<> mdstr(1,12); // range for months
std::uniform_int_distribution<> ddstr(1, 31); // range for day of month
std::uniform_int_distribution<> hdstr(0, 23); // range for hour of day
std::uniform_int_distribution<> mindstr(0, 59); // range for min in hour
constexpr const SecIntType ulimit = 59 * 1'000'000'000L; // max nanoseconds in minute
std::uniform_int_distribution<SecIntType> nanosdstr(0, ulimit);// nanoseconds

double maxdiff = std::numeric_limits<double>::min();

int make_random_jd(double &d1, double &d2, dso::datetime<dso::nanoseconds> &t) {
  int iy, im, id, ihour, imin;
  double sec;
  // make random datetime
  iy = ydstr(gen);
  im = mdstr(gen);
  id = ddstr(gen);
  ihour = hdstr(gen);
  imin = mindstr(gen);
  SecIntType nanosec = nanosdstr(gen);
  sec = static_cast<double>(nanosec) * 1e-9;
  assert(sec < 60e0);
  
  // Express as two-part JD.
  double d;
  if (iauCal2jd(iy, im, id, &d1, &d2)) {
    /*fprintf(stderr, "ERROR failed call to iauCal2jd; YYYYMMDD was %d%02d%02d\n",
            iy, im, id);*/
    // note that if this fails, then so should the datetime constructor ...
    try {
      dso::datetime<dso::nanoseconds> terr{dso::year(iy), dso::month(im),
          dso::day_of_month(id), dso::hours(ihour), dso::minutes(imin),
          dso::nanoseconds(nanosec)};
      fprintf(stderr, "ERROR. Datetime did not fail constructing an erronuous date!\n");
    } catch (std::exception &) {
      /*printf("Nice, SOFA failed and so did datetime!\n");*/
      ;
    }
    return 1;
  }
  if (iauTf2d('+', ihour, imin, sec, &d)) {
    /*fprintf(stderr,
            "ERROR failed call to iauTf2d; HHMMSS.S was: %02d%02d%.5f\n", ihour,
            imin, sec);*/
    return 1;
  }
  d2 += d;
  
  // print the c++ code to compile the same date using datetime lib
  dso::datetime<dso::nanoseconds> t1{dso::year(iy), dso::month(im),
         dso::day_of_month(id), dso::hours(ihour), dso::minutes(imin),
         dso::nanoseconds(nanosec)};

  // by the way, add a check for calendar to MJD conversion
  // printf("[NOTE] Visual Compare = %.20e //MJD this\n", t1.as_mjd());
  // printf("                      = %.20e //MJD SOFA\n", d2);
  assert(t1.as_mjd() == d2);

  t = t1;
  return 0;
}

int main() {
  for (int i = 0; i < num_tests; i++) {
    // printf("SUBCASE(\"Sub-Case #%d of SOFA tests\") {\n", i);
    double d1, d2;
    dso::datetime<dso::nanoseconds> t;
    while (make_random_jd(d1, d2, t))
      ;

    double e_mine = t.as_julian_epoch();
    double e_sofa = iauEpj(d1, d2);

    // printf("[NOTE] Visual Compare = %.20e //JD Epoch this\n", e_mine);
    // printf("                      = %.20e //JD Epoch SOFA\n", e_sofa);
    assert(e_mine == e_sofa);

    double djm0, djm;
    iauEpj2jd(e_sofa, &djm0, &djm);
    double djm_mine = dso::epj2mjd(e_mine);

    // printf("[NOTE] Visual Compare = %.20e //MJD Original this\n", djm_mine);
    // printf("                      = %.20e //MJD Original SOFA\n", djm);
    assert(djm_mine == djm);
  }

  printf("Number of tests: %d; all ok!\n", num_tests);
  return 0;
}
