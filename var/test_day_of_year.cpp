#include <iostream>
#include <stdexcept>
#include "dtfund.hpp"

using dso::year;
using dso::month;
using dso::day_of_year;

int main()
{
  day_of_year dm1 {99};
  day_of_year dm2 {98};
  dm2 = 98 +1 -1;
  // dm2 = dm2 + 1 -1; CMP_ERROR
  // day_of_year dm21 = dm1 + 1; CMP_ERROR
  // day_of_year dm22 = dm2.as_underlying_type() + 1; CMP_ERROR
  // day_of_year dm23 = 8; CMP_ERROR
  day_of_year dm3(dm1);
  
  // testing operators
  assert( dm1 == dm3 );
  assert( dm1 != dm2 );
  assert( dm1 >= dm3 );
  assert( dm1 > dm2 );
  assert( dm2 < dm1 );
  assert( (dm2+=1) == dm1 );                 // day_of_year += Integral_Type
  --dm2;                                     // --day_of_year
  assert( ( dm2+=day_of_year(1) ) == dm1 );  // day_of_year += day_of_year
  dm2--;                                     // day_of_year--
  assert( (dm1-=1) == dm2 );                 // day_of_year += Integral_Type
  ++dm1;                                     // ++day_of_year
  assert( ( dm1-=day_of_year(1) ) == dm2 );  // day_of_year += day_of_year
  dm1++;                                     // day_of_year++

  // testing functions
  year ly {2016}; // leap year
  year yr {2017};
  assert( dm1.is_valid(yr) );
  dm1 = day_of_year(365);
  dm1 = 365;
  assert( dm1.is_valid(yr) );
  ++dm1;    // dm = 366
  assert( !(dm1.is_valid(yr)) );
  dm1 -= 1; // dm = 365
  assert( dm1.is_valid(ly) );
  ++dm1;    // dm = 366
  assert( !(dm1.is_valid(yr)) ); // non-leap year, [1, 365] days
  assert( dm1.is_valid(ly) );   // leap year, [1-366] days

  std::cout<<"\nAll checks for dso::day_of_year OK\n";
  return 0;
}
