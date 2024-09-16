#include "dtdatetime.hpp"
#include "tpdate.hpp"
#include <cassert>
#include <random>

/*
 * Check the precision of a TwoPartDate when constructed from a
 * datetime<T> instance, with T being microseconds
 */

using namespace dso;

constexpr const long num_tests = 1'000'000;
using nsec = dso::microseconds;
typedef nsec::underlying_type SecIntType;
constexpr const double PRECISION_NSEC = 2e-5;

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
  datetime<nsec> d1, d2, d3;
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
      /* the difference between the two dates, in nsec should be below
       * PRECISION_NSEC
       */
      assert(tpd1.imjd() == d1.imjd().as_underlying_type());
      assert(std::abs(tpd1.sec_of_day<nsec>() - d1.sec().as_underlying_type()) <
             PRECISION_NSEC);

      TwoPartDate tpd2(d2);
      assert(tpd2.imjd() == d2.imjd().as_underlying_type());
      assert(std::abs(tpd2.sec_of_day<nsec>() - d2.sec().as_underlying_type()) <
             PRECISION_NSEC);

      TwoPartDate tpd3(d3);
      assert(tpd3.imjd() == d3.imjd().as_underlying_type());
      assert(std::abs(tpd3.sec_of_day<nsec>() - d3.sec().as_underlying_type()) <
             PRECISION_NSEC);

      ++testnr;
    }
  }

  return 0;
}
