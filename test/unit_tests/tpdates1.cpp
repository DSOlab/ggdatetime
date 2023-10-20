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
constexpr const double PRECISION_SEC = 5e-12;
constexpr const double PRECISION_NSEC = 5e-3;

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
      assert(std::abs(tpd1.sec_of_day<nsec>() -
                      d1.sec().as_underlying_type()) <
             PRECISION_NSEC);
      //const double dd1 = std::abs(tpd1.sec_of_day<seconds>() - d1.sec().to_fractional_seconds());
      
      TwoPartDate tpd2(d2);
      assert(tpd2.imjd() == d2.imjd().as_underlying_type());
      assert(std::abs(tpd2.sec_of_day<nsec>() -
                      d2.sec().as_underlying_type()) <
             PRECISION_NSEC);
      //const double dd2 = std::abs(tpd2.sec_of_day<seconds>() - d2.sec().to_fractional_seconds());
      
      TwoPartDate tpd3(d3);
      assert(tpd3.imjd() == d3.imjd().as_underlying_type());
      assert(std::abs(tpd3.sec_of_day<nsec>() -
                      d3.sec().as_underlying_type()) <
             PRECISION_NSEC);
      //const double dd3 = std::abs(tpd3.sec_of_day<seconds>() - d3.sec().to_fractional_seconds());


      ++testnr;
      if (testnr % 10)
        printf("%d/%ld\r", testnr, num_tests);
    }
  }

  return 0;
}
