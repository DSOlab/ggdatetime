#include "dtcalendar.hpp"
#include "tpdate.hpp"
#include <cassert>
#include <random>

using namespace dso;

constexpr const long num_tests = 1'000'000;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;
constexpr const SecIntType TOSEC = nsec::sec_factor<SecIntType>();
constexpr const double SECINDAY = nsec::max_in_day;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, nsec::max_in_day); /* range for day of month */

  int testnr = 0, ok;
  datetime<nsec> d1, d2, d3, max_diff_date;
  double max_diff_nsec = 0e0;
  double max_diff_sec = 0e0;
  while (testnr < num_tests) {
    /* do we have a valid date ? */
    try {
      d1 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      d2 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      /* d3 on same day as d2 */
      d3 = datetime<nsec>{d2.imjd(), nsec(nsdstr(gen))};
      ok = 1;
    } catch (std::exception &) {
      ok = 0;
    }
    if (ok) {
      /* construct a TwoPartDate from a datetime */
      TwoPartDate tpd1(d1);
      assert(tpd1.imjd() == d1.imjd().as_underlying_type());
      /* fraction of day to nanoseconds */
      const double ns1 = tpd1.sec_of_day<nanoseconds>();
      /* difference from original nanoseconds */
      const double fd1 = ns1 - (double)d1.sec().as_underlying_type();
      TwoPartDate tpd2(d2);
      assert(tpd2.imjd() == d2.imjd().as_underlying_type());
      const double ns2 = tpd2.sec_of_day<nanoseconds>();
      const double fd2 = ns2 - (double)d2.sec().as_underlying_type();
      TwoPartDate tpd3(d3);
      assert(tpd3.imjd() == d3.imjd().as_underlying_type());
      const double ns3 = tpd3.sec_of_day<nanoseconds>();
      const double fd3 = ns3 - (double)d3.sec().as_underlying_type();

      /* maximum difference */
      if (std::abs(fd1) > std::abs(max_diff_nsec)) {
        max_diff_nsec = fd1;
        max_diff_date = d1;
      }
      if (std::abs(fd2) > std::abs(max_diff_nsec)) {
        max_diff_nsec = fd2;
        max_diff_date = d2;
      }
      if (std::abs(fd3) > std::abs(max_diff_nsec)) {
        max_diff_nsec = fd3;
        max_diff_date = d3;
      }

      /* do the same, but with seconds instead of nsec */
      const double fs1 =
          tpd1.sec_of_day<seconds>() - to_fractional_seconds(d1.sec());
      const double fs2 =
          tpd2.sec_of_day<seconds>() - to_fractional_seconds(d2.sec());
      const double fs3 =
          tpd3.sec_of_day<seconds>() - to_fractional_seconds(d3.sec());

      /* maximum difference */
      if (std::abs(fs1) > std::abs(max_diff_sec)) {
        max_diff_sec = fs1;
      }
      if (std::abs(fs2) > std::abs(max_diff_sec)) {
        max_diff_sec = fs2;
      }
      if (std::abs(fs3) > std::abs(max_diff_sec)) {
        max_diff_sec = fs3;
      }

      ++testnr;
      if (testnr % 10)
        printf("%d/%ld\r", testnr, num_tests);
    }
  }

  printf("Maximum difference between a datetime<nanoseconds> and a TwoPartDate "
         "instances is:\n%+.9e[nanosec] at datetime: %ld[MJD] %ld[nanosec]\n",
         max_diff_nsec, max_diff_date.imjd().as_underlying_type(),
         max_diff_date.sec().as_underlying_type());
  printf("Maximum difference between a datetime<nanoseconds> and a TwoPartDate "
         "instances is:\n%+.9e[sec]\n",
         max_diff_sec);
  printf("When transforming a datetime<nanoseconds> to a TwoPartDate instance, "
         "the worst case scenario is an accuracy of about %.1e[nanosec]\n",
         max_diff_nsec);
  printf("When transforming a datetime<nanoseconds> to a TwoPartDate instance, "
         "the worst case scenario is an accuracy of about %.1e[sec]\n",
         max_diff_sec);

  return 0;
}
