#include "dtcalendar.hpp"
#include <cassert>
#include <random>

using namespace dso;

constexpr const long num_tests = 1'000;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;
constexpr const SecIntType TOSEC = nsec::sec_factor<SecIntType>();

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31);  /* range for day of month */
  //std::uniform_int_distribution<> hrdstr(0, 23); /* range for day of month */
  //std::uniform_int_distribution<> mndstr(0, 59); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, nsec::max_in_day); /* range for day of month */

  int testnr = 0,ok;
  datetime<nsec> d1,d2;
  while (testnr < num_tests) {
    /* do we have a valid date ? */
    try {
    d1 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                      day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
    d2 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                      day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
    ok = 1;
    } catch (std::exception &) {
      ok = 0;
    }
    if (ok) {
    const auto interval = d2 - d1;
      //printf("d1=%08ld %015ld\n", d1.imjd().as_underlying_type(), d1.sec().as_underlying_type());
      //printf("d2=%08ld %015ld\n", d2.imjd().as_underlying_type(), d2.sec().as_underlying_type());
      //printf("dt=(%d)%ld:%ld\n", interval.sign(), interval.days(), interval.sec().as_underlying_type());
      const auto d = d1 + interval;
      assert( d == d2 );
      ++testnr;
    } 
  }

  return 0;
}
