#include "dtdatetime.hpp"
#include <cstdio>
#include <limits>

using namespace dso;

int main() {
  printf("Maximum number of days representable by each *sec type\n");
  printf("%-13s %17ld, size: %ld\n", seconds::unit_literal(),
         std::numeric_limits<seconds::underlying_type>::max() /
             seconds::max_in_day, sizeof(seconds));
  printf("%-13s %17ld, size: %ld\n", milliseconds::unit_literal(),
         std::numeric_limits<milliseconds::underlying_type>::max() /
             milliseconds::max_in_day, sizeof(milliseconds));
  printf("%-13s %17ld, size: %ld\n", microseconds::unit_literal(),
         std::numeric_limits<microseconds::underlying_type>::max() /
             microseconds::max_in_day, sizeof(microseconds));
  printf("%-13s %17ld, size: %ld\n", nanoseconds::unit_literal(),
         std::numeric_limits<nanoseconds::underlying_type>::max() /
             nanoseconds::max_in_day, sizeof(nanoseconds));
  printf("%-13s %17ld, size: %ld\n", picoseconds::unit_literal(),
         std::numeric_limits<picoseconds::underlying_type>::max() /
             picoseconds::max_in_day, sizeof(picoseconds));

  return 0;
}
