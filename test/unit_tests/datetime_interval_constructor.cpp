#include "datetime_interval.hpp"

using namespace dso;

int main() {

  /* template parameter is seconds */
  {
    constexpr const datetime_interval<seconds> i1{0, seconds(2)};
    constexpr const datetime_interval<seconds> i2{-0, seconds(2)};
    constexpr const datetime_interval<seconds> i3{0, seconds(-2)};
    constexpr const datetime_interval<seconds> i4{-0, seconds(-2)};

    static_assert(i1 == i2);
    static_assert(i3 == i4);
    static_assert(i1.sec() == i2.sec());
    static_assert(i3.sec() == i4.sec());

    /* sec() return positive seconds */
    static_assert(i1.sec() == i3.sec());
    static_assert(i1.sec().as_underlying_type() ==
                  i3.sec().as_underlying_type());

    /* signed_sec() return the seconds signed */
    static_assert(i1.signed_sec().as_underlying_type() ==
                  i3.signed_sec().as_underlying_type() * -1);

    /* sign of the interval */
    static_assert(i1.sign() == -1 * i3.sign());

    /* total seconds, ignoring the sign */
    static_assert(i1.unsigned_total_sec() == i3.unsigned_total_sec());

    /* total seconds, with the correct sign */
    static_assert(i1.signed_total_sec().as_underlying_type() ==
                  i3.signed_total_sec().as_underlying_type() * -1);
  }

  /* template parameter is nanoseconds */
  {
    constexpr const datetime_interval<nanoseconds> i1{0, nanoseconds(2)};
    constexpr const datetime_interval<nanoseconds> i2{-0, nanoseconds(2)};
    constexpr const datetime_interval<nanoseconds> i3{0, nanoseconds(-2)};
    constexpr const datetime_interval<nanoseconds> i4{-0, nanoseconds(-2)};

    static_assert(i1 == i2);
    static_assert(i3 == i4);
    static_assert(i1.sec() == i2.sec());
    static_assert(i3.sec() == i4.sec());

    /* sec() return positive seconds */
    static_assert(i1.sec() == i3.sec());
    static_assert(i1.sec().as_underlying_type() ==
                  i3.sec().as_underlying_type());

    /* signed_sec() return the seconds signed */
    static_assert(i1.signed_sec().as_underlying_type() ==
                  i3.signed_sec().as_underlying_type() * -1);

    /* sign of the interval */
    static_assert(i1.sign() == -1 * i3.sign());

    /* total seconds, ignoring the sign */
    static_assert(i1.unsigned_total_sec() == i3.unsigned_total_sec());

    /* total seconds, with the correct sign */
    static_assert(i1.signed_total_sec().as_underlying_type() ==
                  i3.signed_total_sec().as_underlying_type() * -1);
  }

  /* template parameter is nanoseconds */
  {
    constexpr const long NS = 86400L * 1'000'000'000 * 3 + 2;
    constexpr const datetime_interval<nanoseconds> i1{3, nanoseconds(2)};
    constexpr const datetime_interval<nanoseconds> i2{nanoseconds(NS)};
    constexpr const datetime_interval<nanoseconds> i3{-3, nanoseconds(2)};
    constexpr const datetime_interval<nanoseconds> i4{nanoseconds(-NS)};

    static_assert(i1 == i2);
    static_assert(i3 == i4);
    static_assert(i1.sec() == i2.sec());
    static_assert(i3.sec() == i4.sec());

    /* sec() return positive seconds */
    static_assert(i1.sec() == i3.sec());
    static_assert(i1.sec().as_underlying_type() ==
                  i3.sec().as_underlying_type());

    /* signed_sec() return the seconds signed */
    static_assert(i1.signed_sec().as_underlying_type() ==
                  i3.signed_sec().as_underlying_type() * -1);

    /* sign of the interval */
    static_assert(i1.sign() == -1 * i3.sign());

    /* total seconds, ignoring the sign */
    static_assert(i1.unsigned_total_sec() == i3.unsigned_total_sec());

    /* total seconds, with the correct sign */
    static_assert(i1.signed_total_sec().as_underlying_type() ==
                  i3.signed_total_sec().as_underlying_type() * -1);
  }

  /* template parameter is picoseconds */
  {
    constexpr long PS = 86400L * 1'000'000'000'000L * 3 + 2;
    constexpr datetime_interval<picoseconds> i1{3, picoseconds(2)};
    constexpr datetime_interval<picoseconds> i2{picoseconds(PS)};
    constexpr datetime_interval<picoseconds> i3{-3, picoseconds(2)};
    constexpr datetime_interval<picoseconds> i4{picoseconds(-PS)};

    // printf("i1: %d days + %ldps = %ld\n", i1.days(),
    // i1.signed_sec().as_underlying_type(),
    // i1.signed_total_sec().as_underlying_type()); printf("i2: %d days + %ldps
    // = %ld\n", i2.days(), i2.signed_sec().as_underlying_type(),
    // i2.signed_total_sec().as_underlying_type()); printf("i3: %d days + %ldps
    // = %ld\n", i3.days(), i3.signed_sec().as_underlying_type(),
    // i3.signed_total_sec().as_underlying_type()); printf("i1: %d days + %ldps
    // = %ld\n", i1.days(), i1.signed_sec().as_underlying_type(),
    // i1.signed_total_sec().as_underlying_type());

    static_assert(i1 == i2);
    static_assert(i3 == i4);
    static_assert(i1.sec() == i2.sec());
    static_assert(i3.sec() == i4.sec());

    /* sec() return positive seconds */
    static_assert(i1.sec() == i3.sec());
    static_assert(i1.sec().as_underlying_type() ==
                  i3.sec().as_underlying_type());

    /* signed_sec() return the seconds signed */
    static_assert(i1.signed_sec().as_underlying_type() ==
                  i3.signed_sec().as_underlying_type() * -1);

    /* sign of the interval */
    static_assert(i1.sign() == -1 * i3.sign());

    /* total seconds, ignoring the sign */
    static_assert(i1.unsigned_total_sec() == i3.unsigned_total_sec());

    /* total seconds, with the correct sign */
    static_assert(i1.signed_total_sec().as_underlying_type() ==
                  i3.signed_total_sec().as_underlying_type() * -1);
  }

  return 0;
}
