#include "dtfund.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>

using dso::hours;
using dso::milliseconds;
using dso::minutes;
using dso::nanoseconds;
using dso::seconds;
using dso::t_hmsf;

int main() {
  seconds s1(1);
  t_hmsf h1(s1);
  assert(h1._hours == hours(0) && h1._minutes == minutes(0) &&
         h1._seconds == seconds(1) && h1._fraction == 0e0);

  s1 = seconds(60);
  t_hmsf h2(s1);
  assert(h2._hours == hours(0) && h2._minutes == minutes(1) &&
         h2._seconds == seconds(0) && h2._fraction == 0e0);

  s1 = seconds(120);
  t_hmsf h3(s1);
  assert(h3._hours == hours(0) && h3._minutes == minutes(2) &&
         h3._seconds == seconds(0) && h3._fraction == 0e0);

  s1 = seconds(123);
  t_hmsf h4(s1);
  assert(h4._hours == hours(0) && h4._minutes == minutes(2) &&
         h4._seconds == seconds(3) && h4._fraction == 0e0);

  s1 = seconds(60 * 60);
  t_hmsf h5(s1);
  assert(h5._hours == hours(1) && h5._minutes == minutes(0) &&
         h5._seconds == seconds(0) && h5._fraction == 0e0);

  s1 = seconds(60 * 60 + 60 + 1);
  t_hmsf h6(s1);
  assert(h6._hours == hours(1) && h6._minutes == minutes(1) &&
         h6._seconds == seconds(1) && h6._fraction == 0e0);

  s1 = seconds(60 * 60 + 15 * 60 + 19);
  t_hmsf h7(s1);
  assert(h7._hours == hours(1) && h7._minutes == minutes(15) &&
         h7._seconds == seconds(19) && h7._fraction == 0e0);

  hours hr(1);
  // t_hmsf h8(hr); CMP_ERROR
  minutes mn(1);
  // t_hmsf h9(mn); CMP_ERROR

  s1 = seconds(60 * 60 + 15 * 60 + 19 + 24 * 60 * 60);
  t_hmsf h10(s1);
  assert(h10._hours == hours(1 + 24) && h10._minutes == minutes(15) &&
         h10._seconds == seconds(19) && h10._fraction == 0e0);

  // test milliseconds
  milliseconds ss1(1);
  t_hmsf hh1(ss1);
  assert(hh1._hours == hours(0) && hh1._minutes == minutes(0) &&
         hh1._seconds == seconds(0) && hh1._fraction == 1e-3);

  ss1 = milliseconds(60 * 1000);
  t_hmsf hh2(ss1);
  assert(hh2._hours == hours(0) && hh2._minutes == minutes(1) &&
         hh2._seconds == seconds(0) && hh2._fraction == 0e0);

  ss1 = milliseconds(120 * 1000);
  t_hmsf hh3(ss1);
  assert(hh3._hours == hours(0) && hh3._minutes == minutes(2) &&
         hh3._seconds == seconds(0) && hh3._fraction == 0e0);

  ss1 = milliseconds(123 * 1000);
  t_hmsf hh4(ss1);
  assert(hh4._hours == hours(0) && hh4._minutes == minutes(2) &&
         hh4._seconds == seconds(3) && hh4._fraction == 0e0);

  ss1 = milliseconds(60 * 60 * 1000);
  t_hmsf hh5(ss1);
  assert(hh5._hours == hours(1) && hh5._minutes == minutes(0) &&
         hh5._seconds == seconds(0) && hh5._fraction == 0e0);

  ss1 = milliseconds((60 * 60 + 60 + 1) * 1000);
  t_hmsf hh6(ss1);
  assert(hh6._hours == hours(1) && hh6._minutes == minutes(1) &&
         hh6._seconds == seconds(1) && hh6._fraction == 0e0);

  ss1 = milliseconds((60 * 60 + 15 * 60 + 19) * 1000);
  t_hmsf hh7(ss1);
  assert(hh7._hours == hours(1) && hh7._minutes == minutes(15) &&
         hh7._seconds == seconds(19) && hh7._fraction == 0e0);

  ss1 = milliseconds((60 * 60 + 15 * 60 + 19) * 1000 + 3);
  t_hmsf hh8(ss1);
  assert(hh8._hours == hours(1) && hh8._minutes == minutes(15) &&
         hh8._seconds == seconds(19) && hh8._fraction == 3e-3);

  ss1 = milliseconds((60 * 60 + 15 * 60 + 19) * 1000 + 999);
  t_hmsf hh9(ss1);
  assert(hh9._hours == hours(1) && hh9._minutes == minutes(15) &&
         hh9._seconds == seconds(19) && hh9._fraction == 0.999e0);

  ss1 = milliseconds((60 * 60 + 15 * 60 + 19 + 24 * 60 * 60) * 1000 + 1001);
  t_hmsf hh10(ss1);
  assert(hh10._hours == hours(1 + 24) && hh10._minutes == minutes(15) &&
         hh10._seconds == seconds(20) && hh10._fraction == 1e-3);

  // test nanoseconds
  long s2ns = 1000000000;
  long millis2ns = 1000000;
  long micros2ns = 1000;
  nanoseconds sn1;

  // sn1 = cast<nanoseconds>(ss1);
  sn1 = dso::cast_to<milliseconds, nanoseconds>(ss1);
  t_hmsf hn1(sn1);
  assert(hh10 == hn1);

  std::cout << "\nAll checks for dso::t_hmsf OK\n";
  return 0;
}