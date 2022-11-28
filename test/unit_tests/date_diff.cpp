#include "dtcalendar.hpp"
#include <random>

/*
 * Unit test test_date_diff
 * tests template function dso::date_diff()
 */

using dso::datetime;
using dso::nanoseconds;
using dso::seconds;

typedef nanoseconds::underlying_type SecIntType;
constexpr const SecIntType factor = nanoseconds::sec_factor<SecIntType>();

template <typename ST> double fsec2fdays(ST sec) noexcept {
  const double fsec = static_cast<double>(sec.as_underlying_type());
  return fsec / ST::template sec_factor<double>();
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    fprintf(stderr, "Ignoring all input!\n");
  }

  // first check, see that everything works
  datetime<nanoseconds> t1(dso::year(2022), dso::month(1), dso::day_of_month(1),
                           dso::hours(0), dso::minutes(0), nanoseconds(0));
  auto t2 = t1;
  t2.add_seconds(dso::cast_to<seconds, nanoseconds>(seconds(86400 / 2)));

  assert(dso::date_diff<dso::DateTimeDifferenceType::FractionalDays>(t2, t1) ==
         0.5e0);
  assert(dso::date_diff<dso::DateTimeDifferenceType::FractionalSeconds>(
             t2, t1) == 86400 / 2e0);

  [[maybe_unused]] unsigned num_failed = 0;
  for (int k = 0; k < 10; k++) {
    // start from date t1 and randomly add seconds, see what happens
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<SecIntType> distr(-86400 * 5, 86400 * 5);
    for (int i = 0; i < 1000; i++) {
      auto trand = t1;

      // trand = t1 + x
      // date_diff(d1,d2) acts as: d1 - d2 (here t1 - trand)
      // hence x = trand - t1

      // random (nano)seconds to add
      const seconds iSecDiff(distr(gen));
      trand.add_seconds(dso::cast_to<seconds, nanoseconds>(iSecDiff));

#ifdef VERBOSE_TESTS
      printf("Added seconds: %ld\n", iSecDiff.as_underlying_type());
#endif

      // actual difference in fractional days
      const double days_diff = iSecDiff.fractional_days();
#ifdef VERBOSE_TESTS
      printf("Difference is %.12f, expected: %.12f\n",
             dso::date_diff<dso::DateTimeDifferenceType::FractionalDays>(t1,
                                                                         trand),
             days_diff);
#endif
      assert(dso::date_diff<dso::DateTimeDifferenceType::FractionalDays>(
                 t1, trand) == -days_diff);
      // if (dso::date_diff<dso::DateTimeDifferenceType::FractionalDays>(
      //            t1, trand) != -days_diff) {
      //   printf("-------> Would fail #1!\n");
      //   ++num_failed;
      // }

      // actuall difference in fractional seconds
      const double sec_diff = iSecDiff.to_fractional_seconds();
#ifdef VERBOSE_TESTS
      printf("Difference is %.12f, expected: %.12f\n",
             dso::date_diff<dso::DateTimeDifferenceType::FractionalSeconds>(
                 t1, trand),
             sec_diff);
#endif
      assert(dso::date_diff<dso::DateTimeDifferenceType::FractionalSeconds>(
                 t1, trand) == -sec_diff);
      // if (dso::date_diff<dso::DateTimeDifferenceType::FractionalSeconds>(
      //            t1, trand) != -sec_diff) {
      //   printf("-------> Would fail #2!\n");
      // }
    }
  }

  // printf("Failed: %u\n", num_failed);
  printf("All tests passed in %s\n", argv[0]);
  return 0;
}
