#include <iostream>
#include <vector>
#include <stdexcept>
#include "dtfund.hpp"

using ngpt::modified_julian_day;

/// @todo operator mjd+int should not be allowed -> Ok fixed!

int main()
{
  modified_julian_day dm1 {58484};
  modified_julian_day dm2 {58483};
  dm2 = 58483 + 1 - 1;
  // MJD +- IntegralType is not allowed;
  // dm2 = dm2 + 1 -1; CMP_ERROR
  // modified_julian_day dm21 = dm1 + 1; CMP_ERROR
  modified_julian_day dm22 = dm2.as_underlying_type() + 1;
  modified_julian_day dm23 = 8;
  modified_julian_day dm3(dm1);

  // Constructors/transformation to calendar dates
  modified_julian_day dm12(ngpt::year(2019), ngpt::day_of_year(1));
  assert( dm12 == dm1 );
  modified_julian_day dm24(ngpt::year(2018), ngpt::day_of_year(365));
  assert( dm24 == dm2 );
  modified_julian_day dm13(ngpt::year(2097), ngpt::day_of_year(1));
  assert( modified_julian_day(86974) == dm13 );
  modified_julian_day dm14(ngpt::year(1858), ngpt::month(11), ngpt::day_of_month(1));
  assert( dm14.to_julian_day() == 2399984.5e0 );
  modified_julian_day dm15(ngpt::year(1858), ngpt::month(1), ngpt::day_of_month(1));
  assert( dm15.to_julian_day() == 2399680.5e0 );
  //modified_julian_day dm16(ngpt::year(-4713), ngpt::month(1), ngpt::day_of_month(2));
  //assert( dm16.to_julian_day() == 0.5e0 );
  std::vector<modified_julian_day> mjds;
  mjds.push_back(dm12);
  mjds.push_back(dm24);
  mjds.push_back(dm13);
  mjds.push_back(dm14);
  mjds.push_back(dm15);
  //mjds.push_back(dm16);
  for (auto m : mjds) {
    assert( m >= modified_julian_day::min() && m<= modified_julian_day::max() );
  }
  
  // testing operators
  assert( dm1 == dm3 );
  assert( dm1 != dm2 );
  assert( dm1 >= dm3 );
  assert( dm1 > dm2 );
  assert( dm2 < dm1 );
  assert( (dm2+=1) == dm1 );                 // modified_julian_day += Integral_Type
  --dm2;                                     // --modified_julian_day
  assert( ( dm2+=modified_julian_day(1) ) == dm1 ); // modified_julian_day += modified_julian_day
  dm2--;                                     // modified_julian_day--
  assert( (dm1-=1) == dm2 );                 // modified_julian_day += Integral_Type
  ++dm1;                                     // ++modified_julian_day
  assert( ( dm1-=modified_julian_day(1) ) == dm2 ); // modified_julian_day += modified_julian_day
  dm1++;                                     // modified_julian_day++
  assert( dm1 - modified_julian_day(1) == dm2 ); // MJD + MJD
  assert( dm2 + modified_julian_day(1) == dm1 ); // MJD + MJD

  // testing functions

  std::cout<<"\nAll checks for ngpt::modified_julian_day OK\n";
  return 0;
}
