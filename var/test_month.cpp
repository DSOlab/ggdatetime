#include "dtfund.hpp"
#include <iostream>
#include <stdexcept>

using dso::month;

int main() {
  month mn1{9};
  month mn2{8};
  // mn2 = 8 +1 -1; CMP_ERROR
  // mn2 = mn2 + 1 -1; CMP_ERROR
  // month mn21 = mn1 + 1; CMP_ERROR
  // month mn22 = mn2.as_underlying_type() + 1; CMP_ERROR
  // month mn23 = 8; CMP_ERROR
  month mn3(mn1);

  // testing constructors from strings
  month mn31{"JUL"};
  month mn32{"jul"};
  month mn33{"Jul"};
  month mn34{"July"};
  month mn35{"JULY"};
  month mn36{"july"};
  assert(mn31 == mn32);
  assert(mn32 == mn33);
  assert(mn33 == mn34);
  assert(mn34 == mn35);
  assert(mn35 == mn36);

  const char *invalid_names[] = {"Jully", "Ju", "J", "Julyy"};
  int sz = sizeof invalid_names / sizeof invalid_names[0];
  bool exception_thrown = false;
  for (int i = 0; i < sz; i++) {
    exception_thrown = false;
    try {
      // std::cout<<"\nUsing invalid month name "<<invalid_names[i]
      //         <<" to initialize month";
      month er(invalid_names[i]);
    } catch (std::invalid_argument &e) {
      // std::cout<<"\nException: "<<e.what();
      exception_thrown = true;
    }
    assert(exception_thrown);
  }

  // testing operators
  assert(mn1 == mn3);
  assert(mn1 != mn2);
  assert(mn1 >= mn3);
  assert(mn1 > mn2);
  assert(mn2 < mn1);
  assert((mn2 += 1) == mn1);        // month += Integral_Type
  --mn2;                            // --month
  assert((mn2 += month(1)) == mn1); // month += month
  mn2--;                            // month--
  assert((mn1 -= 1) == mn2);        // month += Integral_Type
  ++mn1;                            // ++month
  assert((mn1 -= month(1)) == mn2); // month += month
  mn1++;                            // month++

  // testing functions
  assert(mn1.is_valid());
  assert(mn2.is_valid());
  mn1 = month(0);
  mn2 = month(13);
  assert(!mn1.is_valid());
  assert(!mn2.is_valid());

  std::cout << "\nAll checks for dso::month OK\n";
  return 0;
}
