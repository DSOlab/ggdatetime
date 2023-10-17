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
      assert(tpd1.imjd() == d1.imjd().as_underlying_type());
      assert(tpd1.fday() == d1.sec().to_fractional_seconds()/SECINDAY);
      /* constructor from MJD and fraction of day */
      tpd1 = TwoPartDate(d1.imjd().as_underlying_type(),
                         d1.sec().to_fractional_seconds() / SECINDAY);
      assert(tpd1.imjd() == d1.imjd().as_underlying_type());
      assert(tpd1.fday() == d1.sec().to_fractional_seconds() / SECINDAY);
      
      TwoPartDate tpd2(d2);
      TwoPartDate tpd3(d3);

      /* how its done using datetime_intervals */
      const auto interval = d2 - d1;
      auto d = d1 + interval;
      assert(d == d2);
      /* same thing using TwoPartDate's */
      const auto i1 = tpd2 - tpd1;
      auto tpd = tpd1 + i1;
      assert(tpd == tpd2);

      /* using datetime_interval ... */
      const auto d32 = d3 - d2;
      d = d2 + d32;
      assert(d == d3);
      /* same thing using TwoPartDate's */
      const auto tpd32 = tpd3 - tpd2;
      tpd = tpd2 + tpd32;
      assert(tpd == tpd3);

      ++testnr;
      if (testnr % 10)
        printf("%d/%ld\r", testnr, num_tests);
    }
  }

  return 0;
}
