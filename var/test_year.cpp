#include <iostream>
#include "dtfund.hpp"

using ngpt::year;

int main()
{
  year yr1 {2019};
  year yr2 = 2018;
  // year yr4 = yr1 + 1; CMP_ERROR
  year yr3 = yr2.as_underlying_type() + 1;

  assert( yr1 == yr3 );
  assert( yr1 != yr2 );
  assert( yr1 >= yr3 );
  assert( yr1  = yr2 );
  assert( yr2 < yr1 );
  assert( !(yr1.is_leap()) );
  assert( yr2.is_leap() );

  std::cout<<"\nAll checks for ngpt::year OK";
  return 0;
}
