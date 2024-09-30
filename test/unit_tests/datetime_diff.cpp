#include "dtdatetime.hpp"
#include <cassert>
#include <limits>
#include <random>

using namespace dso;

constexpr const long num_tests = 1'000'000;
using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;
static_assert(6 * nsec::max_in_day < std::numeric_limits<SecIntType>::max());
constexpr double SEC_PRECISION = 1e-10;
constexpr double FDAY_PRECISION = 1e-9 / 86400e0;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, nsec::max_in_day); /* range for nsec of day */
  std::uniform_int_distribution<> daydiffrnd(-5,
                                             5); /* range for days difference */
  std::uniform_int_distribution<SecIntType> secdiffrnd(-nsec::max_in_day,
                                                       nsec::max_in_day);

  int testnr = 0, ok;
  datetime<nsec> d1, d24, dm24, d36, dm36, d5ns, d5nsb;
  while (testnr < num_tests) {
    /* do we have a valid date ? */
    try {
      d1 = datetime<nsec>{year(ydstr(gen)), month(mdstr(gen)),
                          day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      ok = 1;
    } catch (std::exception &) {
      ok = 0;
    }

    if (ok) {
      /* one day next */
      d24 = d1 + datetime_interval<nsec>(1, nsec(0));
      assert(d24.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             FractionalDays(1e0));
      assert(d24.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             FractionalSeconds(86400e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalDays>(d24) ==
             FractionalDays(-1e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalSeconds>(d24) ==
             FractionalSeconds(-86400e0));

      /* one day previous */
      dm24 = d1 + datetime_interval<nsec>(-1, nsec(0));
      assert(dm24.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             FractionalDays(-1e0));
      assert(dm24.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             FractionalSeconds(-86400e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalDays>(dm24) ==
             FractionalDays(1e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalSeconds>(dm24) ==
             FractionalSeconds(86400e0));

      /* one and a half day next */
      d36 = d1 + datetime_interval<nsec>(
                     1, nsec((86400L / 2) * nsec::sec_factor<long>()));
      assert(d36.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             FractionalDays(1.5e0));
      assert(d36.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             FractionalSeconds(86400e0 + 86400e0 / 2e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalDays>(d36) ==
             FractionalDays(-1.5e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalSeconds>(d36) ==
             FractionalSeconds(-86400e0 - 86400e0 / 2e0));

      /* one and a half day before */
      dm36 = d1 + datetime_interval<nsec>(
                      -1, nsec((86400L / 2) * nsec::sec_factor<long>()));
      assert(dm36.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             FractionalDays(-1.5e0));
      assert(dm36.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             FractionalSeconds(-86400e0 - 86400e0 / 2e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalDays>(dm36) ==
             FractionalDays(1.5e0));
      assert(d1.diff<DateTimeDifferenceType::FractionalSeconds>(dm36) ==
             FractionalSeconds(86400e0 + 86400e0 / 2e0));

      /* just 5 nsec later */
      d5ns = d1 + datetime_interval<nsec>(0, nsec(5));
      assert(d5ns.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             to_fractional_days(nsec(5)));
      assert(d5ns.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             to_fractional_seconds(nsec(5)));

      /* just 5 nsec before */
      d5nsb = d1 + datetime_interval<nsec>(0, nsec(-5));
      assert(d5nsb.diff<DateTimeDifferenceType::FractionalDays>(d1) ==
             to_fractional_days(nsec(-5)));
      assert(d5nsb.diff<DateTimeDifferenceType::FractionalSeconds>(d1) ==
             to_fractional_seconds(nsec(-5)));

      /* random interval */
      const auto secday = nsec(secdiffrnd(gen));
      const auto days = daydiffrnd(gen);
      const auto intrvl = datetime_interval<nsec>(days, secday);
      const nsec isec =
          intrvl.unsigned_total_sec(); /* total nsec in interval */
      const auto rndd = d1 + intrvl;

      assert(
          std::abs(
              rndd.diff<DateTimeDifferenceType::FractionalDays>(d1).days() -
              to_fractional_days(isec).days() * intrvl.sign()) <=
          FDAY_PRECISION);

      /* better than nsec precision */
      assert(std::abs(rndd.diff<DateTimeDifferenceType::FractionalSeconds>(d1)
                          .seconds() -
                      to_fractional_seconds(isec).seconds() * intrvl.sign()) <
             SEC_PRECISION);

      ++testnr;
    }
  }

  return 0;
}
