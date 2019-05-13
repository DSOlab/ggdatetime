#include <iostream>
#include <stdexcept>
#include "dtfund.hpp"

using ngpt::year;
using ngpt::month;
using ngpt::day_of_month;

int main()
{
  day_of_month dm1 {9};
  day_of_month dm2 {8};
  dm2 = 8 +1 -1;
  // dm2 = dm2 + 1 -1; CMP_ERROR
  // day_of_month dm21 = dm1 + 1; CMP_ERROR
  // day_of_month dm22 = dm2.as_underlying_type() + 1; CMP_ERROR
  // day_of_month dm23 = 8; CMP_ERROR
  day_of_month dm3(dm1);
  
  // testing operators
  assert( dm1 == dm3 );
  assert( dm1 != dm2 );
  assert( dm1 >= dm3 );
  assert( dm1 > dm2 );
  assert( dm2 < dm1 );
  assert( (dm2+=1) == dm1 );                 // day_of_month += Integral_Type
  --dm2;                                     // --day_of_month
  assert( ( dm2+=day_of_month(1) ) == dm1 ); // day_of_month += day_of_month
  dm2--;                                     // day_of_month--
  assert( (dm1-=1) == dm2 );                 // day_of_month += Integral_Type
  ++dm1;                                     // ++day_of_month
  assert( ( dm1-=day_of_month(1) ) == dm2 ); // day_of_month += day_of_month
  dm1++;                                     // day_of_month++

  // testing functions
  year ly {2016}; // leap year
  year yr {2017};
  assert( dm1.is_valid(yr, month(1)) );
  dm1 = day_of_month(31);
  assert( dm1.is_valid(yr, month(1)) );
  ++dm1;
  assert( !(dm1.is_valid(yr, month(1))) );
  dm1 -= 3; // dm = 28
  assert( dm1.is_valid(yr, month(1)) );
  assert( dm1.is_valid(ly, month(1)) );
  ++dm1;    // dm = 29
  assert( !(dm1.is_valid(yr, month(1))) ); // non-leap year, 20-FEB is invalid
  assert( dm1.is_valid(ly, month(1)) ); // leap year, 20-FEB is valid

  std::cout<<"\nAll checks for ngpt::day_of_month OK\n";
  return 0;
}
