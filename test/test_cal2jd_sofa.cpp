#include "dtcalendar.hpp"
#include "sofa.h"
#include "dtfund.hpp"
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>

// number of tests to perform
int num_tests = 1000000;

// store max difference (for some difference) ...
double fmjd_diff = std::numeric_limits<double>::min();

// count calls to Cal2jd that have failed (due to erronuous dates)
static std::size_t cal2jd_fails = 0;

// Test for Non-UTC datetimes
template <typename S> using Datetime = dso::datetime<S, dso::TimeScale::TAI>;

// Generators for random numbers ...
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> ydstr(1972, 2050); // range for years
std::uniform_int_distribution<> mdstr(1, 12);      // range for months
std::uniform_int_distribution<> ddstr(1, 31);      // range for day of month
std::uniform_int_distribution<> hdstr(0, 23);      // range for hour of day
std::uniform_int_distribution<> mindstr(0, 59);    // range for min in hour

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
int make_random_jd(double &d1, double &d2, Datetime<S> &t, long &fails) {
  // one S in fractional days
  constexpr const double max_diff = S(1).fractional_days() * 1e0;
  int iy, im, id, ihour, imin;
  double sec;
  bool has_sofa_diff = false;

  // random generator of second type S, from 0 to 1 minute
  // the *seconds (of any type) are generated as integers **NOT** float
  // numbers. This can help to check the accuracy.
  using SecIntType = typename S::underlying_type;
  constexpr const SecIntType ulimit =
      60 * S::template sec_factor<SecIntType>(); // max *seconds in minute
  std::uniform_int_distribution<SecIntType> sdstr(0, ulimit);
  S anysec;

  // keep on trying to transform calendar date(time) to JD, untill we succeed.
  // Note that it is mandatory when iauCal2jd fails, the Datetime constructor
  // also fails (aka throws).
  bool dt_created = false;
  while (!dt_created) {
    // make random datetime
    iy = ydstr(gen);
    im = mdstr(gen);
    id = ddstr(gen);
    ihour = hdstr(gen);
    imin = mindstr(gen);
    anysec = sdstr(gen); // seconds in S (milli, nano, etc)
    // actual fractional seconds
    sec = static_cast<double>(anysec.as_underlying_type()) /
          S::template sec_factor<double>();
    assert(sec <= 60e0);

    // Express as two-part JD.
    double d;
    if (iauCal2jd(iy, im, id, &d1, &d2)) {
      ++cal2jd_fails;
      // note that if this fails, then so should the datetime constructor ...
      try {
        Datetime<S> terr{dso::year(iy),         dso::month(im),
                         dso::day_of_month(id), dso::hours(ihour),
                         dso::minutes(imin),    anysec};
        // if the cinstructor did not throw, we have an error ...
        fprintf(
            stderr,
            "ERROR. Datetime did not fail constructing an erronuous date!\n");
        return 1;
      } catch (std::exception &) {
        ;
      }
      // Date part ok, now cast thetmie to fractional Jdays ...
    } else {
      if (!iauTf2d('+', ihour, imin, sec, &d)) {
        d2 += d;
        dt_created = true;
      }
    }
  }
  // -------------------------------------------------------------------------
  // BreakPoint [1] If we have reached this far, it means that both SOFA and
  // datetime agree on what calendar date is indeed a valid date.
  // Calendar date to [M]JD transformation has been performed.
  // -------------------------------------------------------------------------

  // SOFA created the JD (from calendar; MJD is stored in the d2 variable)
  // the same date using datetime lib. The two dates, when represented as MJD,
  // should differ by less than 1 sec of the the given sec-type; in fractional
  // days, that is:  max_diff
  Datetime<S> t1{dso::year(iy),     dso::month(im),     dso::day_of_month(id),
                 dso::hours(ihour), dso::minutes(imin), anysec};
  //assert(std::abs(t1.as_mjd()-d2)<max_diff);
  if (t1.as_mjd() != d2) {
    if (std::abs(t1.as_mjd()-d2) > fmjd_diff) fmjd_diff = std::abs(t1.as_mjd()-d2);
    // look into the difference if requested ...
    SecIntType totalsec = anysec.as_underlying_type() +
                          60 * S::template sec_factor<SecIntType>() *
                              (static_cast<SecIntType>(imin) +
                               static_cast<SecIntType>(ihour) * 60);
    fprintf(stderr,
            "Cal2Jd:: Strict comparission would fail for date: %d-%02d-%02d "
            "%02d:%02d:%.15f (1)\n",
            iy, im, id, ihour, imin, sec);
    fprintf(stderr, "\t*Seconds = %ld / %ld\n", anysec.as_underlying_type(), totalsec);
    fprintf(stderr, "\tSOFA fractional day : %22.15f (disregard the integral part)\n", d2);
    fprintf(stderr, "\tDatetime f day      : %22.15f\n", t1.sec().fractional_days());
    fprintf(stderr, "\t(Aka, whole MJD is) : %22.15f\n", t1.as_mjd());
    fprintf(stderr, "\tMjd frac. difference: %.15f\n",
            std::abs(t1.as_mjd() - d2));
    fprintf(stderr, "\tFrac. day limit:      %.15f\n", max_diff);
    fprintf(stderr, "\tHard fail ? %s\n",
            std::abs(t1.as_mjd() - d2) < max_diff ? "no" : "yes");
    ++fails;
    has_sofa_diff = true;
  }
  assert(std::abs(t1.as_mjd()-d2)<max_diff); // fractional MJD must match !!

  // by the way, we could also transform the hours, minutes and seconds to
  // seconds, and pass this to the constructor. Should be the same!
  SecIntType totalsec =
      anysec.as_underlying_type() +
      60 * S::template sec_factor<SecIntType>() *
          (static_cast<SecIntType>(imin) + static_cast<SecIntType>(ihour) * 60);
  Datetime<S> t2{dso::year(iy), dso::month(im), dso::day_of_month(id),
                 S(totalsec)};
  assert(t2.as_mjd() == t1.as_mjd()); // fractional MJD must match !!!!!!!

  // since we are here and have all the data ...
  // let's try the inverse: From MJD to calendar date;
  double fpart = t1.sec().fractional_days();
  SecIntType secs = static_cast<SecIntType>(
      std::round((fpart * 86400e0) * S::template sec_factor<double>()));
  Datetime<S> t3{t1.mjd(), S(secs)};
  assert(t1.sec() == t3.sec()); // original *seconds must match!
  if (has_sofa_diff) {
    int iy2, im2, id2, ihmsf[4];;
    double dfrac;
    assert(!iauJd2cal(d1, d2, &iy2, &im2, &id2, &dfrac));
    assert(iy == iy2 && im2 == im && id2 == id);
    assert(iauD2dtf("TAI", 9, d1, d2, &iy, &im, &id, ihmsf)>= 0);
    if (std::abs(dfrac - t3.sec().fractional_days()) > max_diff) {
      printf("iauJd2cal:: Strict comparisson would fail:\n");
      printf("\tSofa fday: %.15f\n", dfrac);
      printf("\tDatetime : %.15f\n", t3.sec().fractional_days());
      // printf("\tSofa milliseconds: %d.%09d\n",ihmdf[2], ihmdf[3]);
      // printf("\tDatetime         : %ld\n",);
    }
  }

  t = t1;
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc>1) fprintf(stderr, "Note: ignoring all command line arguments ...\n");

  printf("Checking random calendar dates to JD via %s\n", argv[0]);
  printf("This set of tests will perform the following tasks:\n");
  printf("\t* Select randaom calendar dates, aka sets of year, month day, hours\n");
  printf("\t  minutes and seconds (of given resolution)\n");
  printf("\t* Try to convert the calendar date to JD/MJD using both SOFA and\n");
  printf("\t  the datetime library. Check that results are the same and that\n");
  printf("\t  datetime fails whenever SOFA does\n");
  printf("\t- Perform the inverse computation, aka from (the computed) MJD\n");
  printf("\t  compute the calendar date; check that the seconds match the initial\n");
  printf("\t  seconds\n");

  printf("--> Note that this test is for non-UTC datetimes <--\n");
  long failures = 0;

  printf("-------------------------------------------------------------------"
         "----------\n");
  printf("Performing subtests for second-type = seconds ...\n");
  for (int i = 0; i < num_tests; i++) {
    Datetime<dso::seconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t, failures)) {
      fprintf(stderr, "[ERROR] Test failed (S=seconds)! %d/%d\n", i, num_tests);
    }
  }
  printf("Results -> Nums tests: %d Failed %ld aka %.3f%%\n", num_tests,
         failures, failures * 100e0 / num_tests);

  printf("-----------------------------------------------------------------------------\n");
  printf("Performing subtests for second-type = milliseconds ...\n");
  failures = 0;
  for (int i = 0; i < num_tests; i++) {
    Datetime<dso::milliseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t, failures)) {
      fprintf(stderr, "[ERROR] Test failed (S=milliseconds)! %d/%d\n", i,
              num_tests);
    }
  }
  printf("Results -> Nums tests: %d Failed %ld aka %.3f%%\n", num_tests,
  failures, failures * 100e0 / num_tests);

  printf("-----------------------------------------------------------------------------\n");
  printf("Performing subtests for second-type = microseconds ...\n");
  failures = 0;
  for (int i = 0; i < num_tests; i++) {
    Datetime<dso::microseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t, failures)) {
      fprintf(stderr, "[ERROR] Test failed (S=microseconds)! %d/%d\n", i,
              num_tests);
      return 1;
    }
  }
  printf("Results -> Nums tests: %d Failed %ld aka %.3f%%\n", num_tests,
  failures, failures * 100e0 / num_tests);

  printf("\n-----------------------------------------------------------------------------\n");
  printf("Performing subtests for second-type = nanoseconds ...\n");
  for (int i = 0; i < num_tests; i++) {
    Datetime<dso::nanoseconds> t;
    double d1, d2;
    if (make_random_jd(d1, d2, t, failures)) {
      fprintf(stderr, "[ERROR] Test failed (S=nanoseconds)! %d/%d\n", i,
              num_tests);
      return 1;
    }
  }
  
  printf("Results -> Nums tests: %d Failed %ld aka %.3f%%\n", num_tests,
  failures, failures * 100e0 / num_tests);

  printf("-------------------------------------------------------------------"
         "----------\n");
  printf("Number of tests: %d (for each realization) Number of fails: %ld\n", num_tests, failures);
  printf("Total erronuous dates, which failed gracefully: %lu\n", cal2jd_fails);
  return 0;
}
