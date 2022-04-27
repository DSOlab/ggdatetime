#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>

// number of tests to perform
int num_tests = 1000000;

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
struct CalendarDate {
  int iy, im, id, ihour, imin;
  S sec;

  void normalize() {
    if (sec == S(60L * S::template sec_factor<long>())) {
      sec = S(0);
      imin += 1;
    }
    if (imin == 60) {
      imin = 0;
      ihour += 1;
    }
    if (ihour == 24) {
      ihour = 0;
      id += 1;
    }
  }

  void print() const {
    printf("%d/%2d/%2d %2d:%2d:%ld [%s]\n", iy, im, id, ihour, imin,
           sec.as_underlying_type(), S::unit_literal());
  }
}; // CalendarDate

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
int cal2jd_random(Datetime<S> &t, CalendarDate<S> &cald) {
  // one S in fractional days
  int iy, im, id, ihour, imin;

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
    anysec = sdstr(gen); // seconds in S (milli, nano, etc) in range [0, 1min]

    // Transform (random) calendar date to MJD
    try {
      Datetime<S> terr{dso::year(iy),         dso::month(im),
                       dso::day_of_month(id), dso::hours(ihour),
                       dso::minutes(imin),    anysec};
      dt_created = true;
      t = terr;
      cald.iy = iy;
      cald.im = im;
      cald.id = id;
      cald.ihour = ihour;
      cald.imin = imin;
      cald.sec = anysec;
    } catch (std::exception &) {
      dt_created = false;
    }
  }

  cald.normalize();
  return 0;
}

#if __cplusplus >= 202002L
template <gconcepts::is_sec_dt S>
#else
template <typename S, typename = std::enable_if_t<S::is_of_sec_type>>
#endif
int mjd2cal(const Datetime<S> &t, const CalendarDate<S> &cald) {

  // date to YMD
  dso::ymd_date ymd(t.as_ymd());
  assert((int)ymd.__year.as_underlying_type() == cald.iy);
  assert((int)ymd.__month.as_underlying_type() == cald.im);
  assert((int)ymd.__dom.as_underlying_type() == cald.id);

  // resolve time, aka HMS struct
  dso::hms_time<S> hms(t.sec());
  // cald.print();
  // printf("           %2d:%2d:%ld [%s]\n", hms._hours.as_underlying_type(),
  // hms._minutes.as_underlying_type(), hms._sec.as_underlying_type(),
  // S::unit_literal());
  assert((int)hms._hours.as_underlying_type() == cald.ihour);
  assert((int)hms._minutes.as_underlying_type() == cald.imin);
  assert(hms._sec == cald.sec);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc > 1)
    fprintf(stderr, "Note: ignoring all command line arguments ...\n");

  printf("////////////////////////////////////////////////////////////////\n");
  printf("%s\n", argv[0]);
  printf("////////////////////////////////////////////////////////////////\n");
  printf("Testing conversions between Calendar dates and (Modified) Julian "
         "Dates\n");
  printf("This program will create a random (but valid) calendar date and\n");
  printf("and transform it to an (M)JD; it will then transform the (M)JD back\n");
  printf("to a calendar date and check that we have no loss of precision. If\n");
  printf("the initial and transformed-back calendar dates are not the same, \n");
  printf("the program will exit with error (an assertion will fail)\n");

  printf("testing conversions for template parameter: seconds ...");
  Datetime<dso::seconds> t1;
  CalendarDate<dso::seconds> ct1;
  for (int i = 0; i < num_tests; i++) {
    cal2jd_random(t1, ct1);
    mjd2cal(t1, ct1);
  }
  printf("done\n");

  printf("testing conversions for template parameter: milliseconds ...");
  Datetime<dso::milliseconds> t2;
  CalendarDate<dso::milliseconds> ct2;
  for (int i = 0; i < num_tests; i++) {
    cal2jd_random(t2, ct2);
    mjd2cal(t2, ct2);
  }
  printf("done\n");

  printf("testing conversions for template parameter: microseconds ...");
  Datetime<dso::microseconds> t3;
  CalendarDate<dso::microseconds> ct3;
  for (int i = 0; i < num_tests; i++) {
    cal2jd_random(t3, ct3);
    mjd2cal(t3, ct3);
  }
  printf("done\n");

  printf("testing conversions for template parameter: nanoseconds ...");
  Datetime<dso::nanoseconds> t4;
  CalendarDate<dso::nanoseconds> ct4;
  for (int i = 0; i < num_tests; i++) {
    cal2jd_random(t4, ct4);
    mjd2cal(t4, ct4);
  }
  printf("done\n");

  return 0;
}
