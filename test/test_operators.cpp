#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>

#include "dtcalendar.hpp"
#include "dtfund.hpp"

int main() {

  //
  // Test fundamental types operators
  // -----------------------------------------------------------------------
  //
  std::cout << "\nTesting operators of fundamental types.";
  std::cout
      << "\n-------------------------------------------------------------";
  std::cout << "\n>Testing operators on year";
  ngpt::year y(2019), y1(2018), y2(2020), y3(2019);
  assert(y == y3);
  assert(y >= y1);
  assert(y <= y2);
  assert(y != y1);
  assert((y += 1) == y2);
  assert((y -= 2) == y1);
  assert((y += ngpt::year{2}) == y2);
  assert((y -= ngpt::year{2}) == y1);
  assert(++y == ngpt::year{2019});
  y++;
  assert(y == y2);
  y--;
  assert(y == y3);
  assert(--y == y1);
  assert((y = 2020) == y2);
  std::cout << "\n\tAll tests for year OK!";

  std::cout << "\n>Testing operators on mdified_julian_day";
  ngpt::modified_julian_day m(2019), m1(2018), m2(2020), m3(2019);
  assert(m == m3);
  assert(m >= m1);
  assert(m <= m2);
  assert(m != m1);
  assert((m += 1) == m2);
  assert((m -= 2) == m1);
  assert((m += ngpt::modified_julian_day{2}) == m2);
  assert((m -= ngpt::modified_julian_day{2}) == m1);
  assert(++m == ngpt::modified_julian_day{2019});
  m++;
  assert(m == m2);
  m--;
  assert(m == m3);
  assert(--m == m1);
  assert((m = 2020) == m2);
  std::cout << "\n\tAll tests for modified_julian_day OK!";

  std::cout << "\n>Testing operators on milliseconds";
  ngpt::milliseconds s(2019), s1(2018), s2(2020), s3(2019);
  assert(s == s3);
  assert(s >= s1);
  assert(s <= s2);
  assert(s != s1);
  assert((s += 1) == s2);
  assert((s -= 2) == s1);
  assert((s += ngpt::milliseconds{2}) == s2);
  assert((s -= ngpt::milliseconds{2}) == s1);
  assert(++s == ngpt::milliseconds{2019});
  s++;
  assert(s == s2);
  s--;
  assert(s == s3);
  assert(--s == s1);
  assert((s = 2020) == s2);
  std::cout << "\n\tAll tests for milliseconds OK!";

  std::cout << "\n";
  return 0;
}
