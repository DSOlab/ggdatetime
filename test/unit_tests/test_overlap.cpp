#include "datetime_write.hpp"
#include "dtcalendar.hpp"
#include <cassert>
#include <cstdio>

/*
 * Unit tests to check the dso::intervals_overlap<> template function
 * aka, check if two datetime ranges overlap
 */

using dso::datetime;
using dso::nanoseconds;
using dso::datetime_ranges::OverlapComparissonType;

void print_ranges(const datetime<nanoseconds> &r11,
                  const datetime<nanoseconds> &r12,
                  const datetime<nanoseconds> &r21,
                  const datetime<nanoseconds> &r22) {
  char buf1[64], buf2[64];
  dso::strftime_ymd_hmfs(r11, buf1);
  dso::strftime_ymd_hmfs(r12, buf2);
  printf("[%s to %s] ", buf1, buf2);
  dso::strftime_ymd_hmfs(r21, buf1);
  dso::strftime_ymd_hmfs(r22, buf2);
  printf("[%s to %s]\n", buf1, buf2);
}

int main([[maybe_unused]]int argc, char *argv[]) {
  datetime<nanoseconds> r1_start, r1_end, r2_start, r2_end;
  r1_start = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                   dso::day_of_month(1), nanoseconds(0));
  r1_end = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                 dso::day_of_month(2), nanoseconds(0));
  r2_start = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                   dso::day_of_month(2), nanoseconds(1));
  r2_end = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                 dso::day_of_month(3), nanoseconds(0));

  // these ranges do not overlap
  bool overlap =
      dso::intervals_overlap<nanoseconds, OverlapComparissonType::Strict>(
          r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  print_ranges(r1_start, r1_end, r2_start, r2_end);
  printf("Case 1: Overlap ? %d\n", overlap);
#endif
  assert(!overlap);
  overlap = dso::intervals_overlap<nanoseconds,
                                   OverlapComparissonType::AllowEdgesOverlap>(
      r1_start, r1_end, r2_start, r2_end);
  assert(!overlap);

  // ranges overlap
  r1_end = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                 dso::day_of_month(2), nanoseconds(2));
  overlap = dso::intervals_overlap<nanoseconds, OverlapComparissonType::Strict>(
      r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  print_ranges(r1_start, r1_end, r2_start, r2_end);
  printf("Case 2: Overlap ? %d\n", overlap);
#endif
  assert(overlap);
  overlap = dso::intervals_overlap<nanoseconds,
                                   OverlapComparissonType::AllowEdgesOverlap>(
      r1_start, r1_end, r2_start, r2_end);
  assert(overlap);

  // ranges do not overlap
  r2_start = datetime<nanoseconds>(dso::year(2021), dso::month(12),
                                   dso::day_of_month(30), nanoseconds(1));
  r2_end = datetime<nanoseconds>(dso::year(2021), dso::month(12),
                                 dso::day_of_month(31),
                                 nanoseconds(nanoseconds::max_in_day - 1));
  overlap = dso::intervals_overlap<nanoseconds, OverlapComparissonType::Strict>(
      r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  print_ranges(r1_start, r1_end, r2_start, r2_end);
  printf("Case 3: Overlap ? %d\n", overlap);
#endif
  assert(!overlap);
  overlap = dso::intervals_overlap<nanoseconds,
                                   OverlapComparissonType::AllowEdgesOverlap>(
      r1_start, r1_end, r2_start, r2_end);
  assert(!overlap);

  // ranges overlap only on edge point!
  r2_start = datetime<nanoseconds>(dso::year(2021), dso::month(12),
                                   dso::day_of_month(30), nanoseconds(1));
  r2_end = datetime<nanoseconds>(dso::year(2021), dso::month(12),
                                 dso::day_of_month(31),
                                 nanoseconds(nanoseconds::max_in_day));
  overlap = dso::intervals_overlap<nanoseconds, OverlapComparissonType::Strict>(
      r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  print_ranges(r1_start, r1_end, r2_start, r2_end);
  printf("Case 4: Overlap ? %d\n", overlap);
#endif
  assert(overlap);
  overlap = dso::intervals_overlap<nanoseconds,
                                   OverlapComparissonType::AllowEdgesOverlap>(
      r1_start, r1_end, r2_start, r2_end);
  assert(!overlap);

  r1_start = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                   dso::day_of_month(1), nanoseconds(0));
  r1_end = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                 dso::day_of_month(2), nanoseconds(0));
  r2_start = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                   dso::day_of_month(2), nanoseconds(0));
  r2_end = datetime<nanoseconds>(dso::year(2022), dso::month(1),
                                 dso::day_of_month(3), nanoseconds(0));
  overlap = dso::intervals_overlap<nanoseconds, OverlapComparissonType::Strict>(
      r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  print_ranges(r1_start, r1_end, r2_start, r2_end);
  printf("Case 5: Overlap ? %d\n", overlap);
#endif
  assert(overlap);
  // non-strict comparisson should give us non-overlapping ranges though!
  overlap = dso::intervals_overlap<nanoseconds,
                                   OverlapComparissonType::AllowEdgesOverlap>(
      r1_start, r1_end, r2_start, r2_end);
#ifdef VERBOSE_TESTS
  printf("Case 6: Overlap ? %d\n", overlap);
#endif
  assert(!overlap);

  printf("All tests passed in %s\n", argv[0]);
  return 0;
}
