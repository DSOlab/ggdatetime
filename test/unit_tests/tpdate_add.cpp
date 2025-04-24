#include "datetime_write.hpp"
#include <cassert>
#include <cstring>
#include <random>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

/* test the addition of positive/negative seconds in a TwoPartDate istance */

using namespace dso;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;

constexpr const long num_tests = 10'000;

int main() {

  { /* first part using known dates */
    /* check dates */
    datetime<nanoseconds> d1(year(2023), month(10), day_of_month(24),
                             nanoseconds(0));
    TwoPartDate td1(d1);

    for (int i = 0; i < 86400; i++) {
      td1.add_seconds_inplace(FractionalSeconds(1e0));
    }
    assert(td1.imjd() - 1 == d1.imjd().as_underlying_type());
    assert(td1.seconds() == FractionalSeconds(0e0));

    td1 = TwoPartDate(datetime<nanoseconds>(year(2023), month(10),
                                            day_of_month(24), nanoseconds(0)));
    for (int i = 0; i < 2 * 86400; i++) {
      td1.add_seconds_inplace(FractionalSeconds(1e0));
    }
    assert(td1.imjd() - 2 == d1.imjd().as_underlying_type());
    assert(td1.seconds() == FractionalSeconds(0e0));

    td1 = TwoPartDate(datetime<nanoseconds>(year(2023), month(10),
                                            day_of_month(24), nanoseconds(0)));
    for (int i = 0; i < 86400; i++) {
      td1.add_seconds_inplace(FractionalSeconds(-1e0));
    }
    assert(td1.imjd() + 1 == d1.imjd().as_underlying_type());
    assert(td1.seconds() == FractionalSeconds(0e0));

    td1 = TwoPartDate(datetime<nanoseconds>(year(2023), month(10),
                                            day_of_month(24), nanoseconds(0)));
    for (int i = 0; i < 2 * 86400; i++) {
      td1.add_seconds_inplace(FractionalSeconds(-1e0));
    }
    assert(td1.imjd() + 2 == d1.imjd().as_underlying_type());
    assert(td1.seconds() == FractionalSeconds(0e0));

    td1 = TwoPartDate(datetime<nanoseconds>(year(2023), month(10),
                                            day_of_month(24), nanoseconds(0)));
    for (int i = 0; i < 86400; i++) {
      td1.add_seconds_inplace(FractionalSeconds(2e0));
      td1.add_seconds_inplace(FractionalSeconds(-1e0));
    }
    assert(td1.imjd() - 1 == d1.imjd().as_underlying_type());
    assert(td1.seconds() == FractionalSeconds(0e0));
  }

  { /* second part using random dates */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
    std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
    std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
    std::uniform_int_distribution<SecIntType> nsdstr(
        0, nsec::max_in_day); /* range for day of month */

    int testnr = 0, ok;
    datetime<nsec> d;
    TwoPartDate td;
    while (testnr < num_tests) {
      /* do we have a valid date ? */
      try {
        d = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                           day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
        ok = 1;
      } catch (std::exception &) {
        ok = 0;
      }
      if (ok) {
        td = TwoPartDate(d);
        for (int i = 0; i < 86400; i++) {
          td.add_seconds_inplace(FractionalSeconds(-1e0));
        }
        assert(td.imjd() + 1 == d.imjd().as_underlying_type());
        assert(std::abs(td.sec_of_day<nsec>() - d.sec().as_underlying_type()) <
               2e-2);
        td = TwoPartDate(d);
        for (int i = 0; i < 86400; i++) {
          td.add_seconds_inplace(FractionalSeconds(2e0));
        }
        assert(td.imjd() - 2 == d.imjd().as_underlying_type());
        assert(std::abs(td.sec_of_day<nsec>() - d.sec().as_underlying_type()) <
               2e-2);
        ++testnr;
      }
    }
  }

  return 0;
};
