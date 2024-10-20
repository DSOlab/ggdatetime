#include "calendar.hpp"

using namespace dso;

int main() {
  constexpr const auto t1 = datetime<nanoseconds>(
      year(2021), month(1), day_of_month(1), nanoseconds(123L * 1'000'000'000));
  constexpr const auto t2 =
      datetime<nanoseconds>(year(2021), month(3), day_of_month(1),
                            nanoseconds(86399L * 1'000'000'000L));

  /* Range1 is:
   * [2021/01/01T00:02:03 to 2021/03/01T23:59:59]
   * Range2 is:
   * [2021/01/01T00:00:01 to 2021/01/01T00:02:02]
   * Range3 is:
   * [2021/01/01T00:00:01 to 2021/01/01T00:02:03]
   */
  {
    constexpr const auto i1 =
        datetime<nanoseconds>(year(2021), month(1), day_of_month(1),
                              nanoseconds(1L * 1'000'000'000L));
    constexpr const auto i2 =
        datetime<nanoseconds>(year(2021), month(1), day_of_month(1),
                              nanoseconds(122 * 1'000'000'000L));
    /* no overlap */
    static_assert(
        intervals_overlap<nanoseconds,
                          datetime_ranges::OverlapComparissonType::Strict>(
            t1, t2, i1, i2) == false);

    /* overlap exatcly on boundary */
    constexpr const auto i3 =
        datetime<nanoseconds>(year(2021), month(1), day_of_month(1),
                              nanoseconds(123 * 1'000'000'000L));
    static_assert(
        intervals_overlap<nanoseconds,
                          datetime_ranges::OverlapComparissonType::Strict>(
            t1, t2, i1, i3) == false);
    static_assert(
        intervals_overlap<
            nanoseconds,
            datetime_ranges::OverlapComparissonType::AllowEdgesOverlap>(
            t1, t2, i1, i3) == true);
  }

  /* Range1 is:
   * [2021/01/01T00:02:03 to 2021/03/01T23:59:59]
   * Range2 is:
   * [2021/03/02T00:00:00 to 2022/01/01T00:02:02]
   * Range3 is:
   * [2021/03/01T23:59:59 to 2022/01/01T00:02:02]
   */
  {
    constexpr const auto i1 = datetime<nanoseconds>(
        year(2021), month(3), day_of_month(2), nanoseconds(0));
    constexpr const auto i2 =
        datetime<nanoseconds>(year(2022), month(1), day_of_month(1),
                              nanoseconds(122 * 1'000'000'000L));
    /* no overlap */
    static_assert(
        intervals_overlap<nanoseconds,
                          datetime_ranges::OverlapComparissonType::Strict>(
            t1, t2, i1, i2) == false);

    /* overlap exatcly on boundary */
    constexpr const auto i3 =
        datetime<nanoseconds>(year(2021), month(3), day_of_month(1),
                              nanoseconds((86400L - 1) * 1'000'000'000L));
    static_assert(
        intervals_overlap<nanoseconds,
                          datetime_ranges::OverlapComparissonType::Strict>(
            t1, t2, i3, i2) == false);
    static_assert(
        intervals_overlap<
            nanoseconds,
            datetime_ranges::OverlapComparissonType::AllowEdgesOverlap>(
            t1, t2, i3, i2) == true);
  }

  return 0;
}
