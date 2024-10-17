#include "calendar.hpp"

using namespace dso;

int main() {

  // cast milliseconds to seconds
  {
    constexpr seconds sec1 =
        cast_to<milliseconds, seconds>(milliseconds(10 * 86400L * 1000));
    static_assert(sec1 == seconds(10 * 86400L));
  }

  // cast seconds to milliseconds
  {
    constexpr milliseconds sec1 =
        cast_to<seconds, milliseconds>(seconds(10 * 86401L));
    static_assert(sec1 == milliseconds(10 * 86401L * 1000));
  }

  // cast nanoseconds to seconds
  {
    constexpr seconds sec1 =
        cast_to<nanoseconds, seconds>(nanoseconds(10 * 86401L * 1000000000L));
    static_assert(sec1 == seconds(10 * 86401L));
  }

  // cast nanoseconds to seconds
  {
    constexpr seconds sec1 =
        cast_to<nanoseconds, seconds>(nanoseconds(10 * 86399L * 1000000000L));
    static_assert(sec1 == seconds(10 * 86399L));
  }

  // cast seconds to nanoseconds
  {
    constexpr nanoseconds sec1 =
        cast_to<seconds, nanoseconds>(seconds(10 * 86401L));
    static_assert(sec1 == nanoseconds(10 * 86401L * 1000000000L));
  }

  // cast picoseconds to seconds
  {
    constexpr seconds sec1 = cast_to<picoseconds, seconds>(
        picoseconds(10 * 86400L * 1000000000000L));
    static_assert(sec1 == seconds(10 * 86400L));
  }

  // cast seconds to picoseconds
  {
    constexpr picoseconds sec1 =
        cast_to<seconds, picoseconds>(seconds(10 * 86401L));
    static_assert(sec1 == picoseconds(10 * 86401L * 1000000000000L));
  }

  // cast seconds to picoseconds
  {
    constexpr picoseconds sec1 =
        cast_to<seconds, picoseconds>(seconds(10 * 86399L));
    static_assert(sec1 == picoseconds(10 * 86399L * 1000000000000L));
  }

  return 0;
}
