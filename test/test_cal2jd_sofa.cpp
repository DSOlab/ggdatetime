#include "dtcalendar.hpp"
#include "sofa.h"
#include <cstdlib>
#include <cerrno>
#include <random>
#include <cassert>
#include <cstdio>

int num_tests = 10000;
double maxdiff = std::numeric_limits<double>::min();
static std::size_t cal2jd_fails = 0;

// Leap dates post 1972
struct {
  int iy, im, leap;
} LeapYmd[] = {{1972, 1, 10}, {1972, 7, 11}, {1973, 1, 12}, {1974, 1, 13},
               {1975, 1, 14}, {1976, 1, 15}, {1977, 1, 16}, {1978, 1, 17},
               {1979, 1, 18}, {1980, 1, 19}, {1981, 7, 20}, {1982, 7, 21},
               {1983, 7, 22}, {1985, 7, 23}, {1988, 1, 24}, {1990, 1, 25},
               {1991, 1, 26}, {1992, 7, 27}, {1993, 7, 28}, {1994, 7, 29},
               {1996, 1, 30}, {1997, 7, 31}, {1999, 1, 32}, {2006, 1, 33},
               {2009, 1, 34}, {2012, 7, 35}, {2015, 7, 36}, {2017, 1, 37}};

std::random_device rd;  // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> ydstr(1990, 2050); // range for years
std::uniform_int_distribution<> mdstr(1, 12);      // range for months
std::uniform_int_distribution<> ddstr(1, 31);      // range for day of month
std::uniform_int_distribution<> hdstr(0, 23);      // range for hour of day
std::uniform_int_distribution<> mindstr(0, 59);    // range for min in hour

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
int check_at_leap() {
  using SecIntType = typename S::underlying_type;
  int maxn = sizeof(LeapYmd) / sizeof(LeapYmd[0]);
  double d, d1, d2;

  for (int i = 0; i < maxn; i++) {
    //
    // [first check] 0-second of the injection date
    int iy = LeapYmd[i].iy;
    int im = LeapYmd[i].im;
    int id = 1;
    int ihour = 0;
    int imin = 0;
    double sec = 0e0;
    S ssec = S(0);
    iauCal2jd(iy, im, id, &d1, &d2);
    iauTf2d('+', ihour, imin, sec, &d);
    // printf("\t%4d%02d%02d-%02d%02d%.10f -> %.20ef\n", iy,im,id,ihour,imin,sec,d2+d);
    dso::datetime<S> t{dso::year(iy), dso::month(im),
                              dso::day_of_month(id), dso::hours(ihour),
                              dso::minutes(imin), S(ssec)};
    // printf("\t                        -> %.20e\n", t.as_mjd());
    assert(t.as_mjd() == d2+d);

    //
    // [second check] 1st-second of the injection date
    sec =  1e0;
    ssec = static_cast<SecIntType>(1 * S::template sec_factor<SecIntType>());
    iauCal2jd(iy, im, id, &d1, &d2);
    iauTf2d('+', ihour, imin, sec, &d);
    // printf("\t%4d%02d%02d-%02d%02d%.10f -> %.20ef\n", iy,im,id,ihour,imin,sec,d2+d);
    dso::datetime<S> t1{dso::year(iy), dso::month(im),
                              dso::day_of_month(id), dso::hours(ihour),
                              dso::minutes(imin), S(ssec)};
    // printf("\t                        -> %.20e\n", t.as_mjd());
    assert(t1.as_mjd() == d2+d);

    //
    // [third check] last-second of the date prior to injection date
    if (im == 7) {
      --im;
      id = 30;
    } else {
      --iy;
      im = 12;
      id = 31;
    }
    ihour = 23;
    imin = 59;
    sec = 60e0;
    ssec = static_cast<SecIntType>(60 * S::template sec_factor<SecIntType>());
    iauCal2jd(iy, im, id, &d1, &d2);
    iauTf2d('+', ihour, imin, sec, &d);
    // printf("\t%4d%02d%02d-%02d%02d%.10f -> %.20ef\n", iy,im,id,ihour,imin,sec,d2+d);
    dso::datetime<S> t2{dso::year(iy), dso::month(im),
                              dso::day_of_month(id), dso::hours(ihour),
                              dso::minutes(imin), S(ssec)};
    // printf("\t                        -> %.20e\n", t.as_mjd());
    assert(t2.as_mjd() == d2+d);

    //
    // [fourth check] pre-last-second of the date prior to injection date
    sec = 59e0;
    ssec = static_cast<SecIntType>(59 * S::template sec_factor<SecIntType>());
    iauCal2jd(iy, im, id, &d1, &d2);
    iauTf2d('+', ihour, imin, sec, &d);
    // printf("\t%4d%02d%02d-%02d%02d%.10f -> %.20ef\n", iy,im,id,ihour,imin,sec,d2+d);
    dso::datetime<S> t3{dso::year(iy), dso::month(im),
                              dso::day_of_month(id), dso::hours(ihour),
                              dso::minutes(imin), S(ssec)};
    // printf("\t                        -> %.20e\n", t.as_mjd());
    assert(t3.as_mjd() == d2+d);
  }

  return 0;
}

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
int make_random_jd(double &d1, double &d2, dso::datetime<S> &t) {
  int iy, im, id, ihour, imin;
  double sec;

  // random generator of second type S, from 0 to 1 minute
  using SecIntType = typename S::underlying_type;
  constexpr const SecIntType ulimit =
      60 * S::template sec_factor<SecIntType>(); // max *seconds in minute
  std::uniform_int_distribution<SecIntType> sdstr(0, ulimit);
  S anysec;

  // what fraction of day is 1 second type S?
  // S onesec(1);
  // double Sfrac = onesec.fractional_days();

  bool dt_created = false;
  while (!dt_created) {
    // make random datetime
    iy = ydstr(gen);
    im = mdstr(gen);
    id = ddstr(gen);
    ihour = hdstr(gen);
    imin = mindstr(gen);
    anysec = sdstr(gen);
    sec = static_cast<double>(anysec.as_underlying_type()) /
          S::template sec_factor<double>();
    assert(sec <= 60e0);

    // Express as two-part JD.
    double d;
    if (iauCal2jd(iy, im, id, &d1, &d2)) {
      ++cal2jd_fails;
      // note that if this fails, then so should the datetime constructor ...
      try {
        dso::datetime<S> terr{dso::year(iy),         dso::month(im),
                              dso::day_of_month(id), dso::hours(ihour),
                              dso::minutes(imin),    S(anysec)};
        // if the cinstructor did not throw, we have an error ...
        fprintf(
            stderr,
            "ERROR. Datetime did not fail constructing an erronuous date!\n");
        return 1;
      } catch (std::exception &) {
        ;
      }
    } else {
      if (!iauTf2d('+', ihour, imin, sec, &d)) {
        d2 += d;
        dt_created = true;
      }
    }
  }

  // the same date using datetime lib
  dso::datetime<S> t1{dso::year(iy),         dso::month(im),
                      dso::day_of_month(id), dso::hours(ihour),
                      dso::minutes(imin),    S(anysec)};
  assert(t1.as_mjd() == d2);

  // by the way, we could also transform the hours, minutes and seconds to
  // seconds, and pass this to the constructor. Should be the same!
  SecIntType totalsec =
      anysec.as_underlying_type() +
      60 * S::template sec_factor<SecIntType>() *
          (static_cast<SecIntType>(imin) + static_cast<SecIntType>(ihour) * 60);
  dso::datetime<S> t2{dso::year(iy), dso::month(im), dso::day_of_month(id),
                      S(totalsec)};
  assert(t2.as_mjd() == d2);

  t = t1;
  return 0;
}

int main() {
  printf("Checking random calendar dates to JD\n");
  for (int i = 0; i < num_tests; i++) {
    dso::datetime<dso::seconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t)) {
      fprintf(stderr, "[ERROR] Test failed (S=seconds)! %d/%d\n", i, num_tests);
      return 1;
    }
  }
  printf("\tSubtest[S=seconds]: Number of tests: %d; all ok!\n", num_tests);

  for (int i = 0; i < num_tests; i++) {
    dso::datetime<dso::milliseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t)) {
      fprintf(stderr, "[ERROR] Test failed (S=milliseconds)! %d/%d\n", i,
              num_tests);
      return 1;
    }
  }
  printf("\tSubtest[S=milliseconds]: Number of tests: %d; all ok!\n", num_tests);

  for (int i = 0; i < num_tests; i++) {
    dso::datetime<dso::microseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t)) {
      fprintf(stderr, "[ERROR] Test failed (S=microseconds)! %d/%d\n", i,
              num_tests);
      return 1;
    }
  }
  printf("\tSubtest[S=microseconds]: Number of tests: %d; all ok!\n", num_tests);

  for (int i = 0; i < num_tests; i++) {
    dso::datetime<dso::nanoseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t)) {
      fprintf(stderr, "[ERROR] Test failed (S=nanoseconds)! %d/%d\n", i,
              num_tests);
      return 1;
    }
  }
  printf("\tSubtest[S=nanoseconds]: Number of tests: %d; all ok!\n", num_tests);


  printf("Checking conversions at leap second injection dates\n");
  check_at_leap<dso::seconds>();
  printf("\tSubtest[S=seconds] ok!\n");
  check_at_leap<dso::milliseconds>();
  printf("\tSubtest[S=milliseconds] ok!\n");
  check_at_leap<dso::microseconds>();
  printf("\tSubtest[S=microseconds] ok!\n");
  check_at_leap<dso::nanoseconds>();
  printf("\tSubtest[S=nanoseconds] ok!\n");

  printf("Number of tests: %d (for each realization); all ok!\n", num_tests);
  printf("Total erronuous date, which failed gracefully: %lu\n", cal2jd_fails);
  return 0;
}
