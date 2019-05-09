#include <iostream>
#include "dtfund.hpp"

using ngpt::year;

int main()
{
  year yr1 {2019};
  year yr2 = 2018;
  yr2 = 2018 +1 -1;
  // year + Int is not allowed, hence:
  // year yr4 = yr1 + 1; CMP_ERROR
  year yr3 = yr2.as_underlying_type() + 1;
  year leap_year = 2016;

  // testing operators
  assert( yr1 == yr3 );
  assert( yr1 != yr2 );
  assert( yr1 >= yr3 );
  assert( yr1 > yr2 );
  assert( yr2 < yr1 );
  assert( (yr2+=1) == yr1 );         // year += Integral_Type
  --yr2;                             // --year
  assert( ( yr2+=year(1) ) == yr1 ); // year += year
  yr2--;                             // year--
  assert( (yr1-=1) == yr2 );         // year += Integral_Type
  ++yr1;                             // ++year
  assert( ( yr1-=year(1) ) == yr2 ); // year += year
  yr1++;                             // year++

  // testing functions
  assert( !(yr1.is_leap()) );
  assert( leap_year.is_leap() );

  std::cout<<"\nAll checks for ngpt::year OK\n";
  return 0;
}
