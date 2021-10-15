#include <iostream>
#include <vector>
#include <stdexcept>
#include "dtfund.hpp"

using dso::modified_julian_day;
using dso::year;
using dso::month;
using dso::day_of_year;
using dso::day_of_month;

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
  modified_julian_day dm12 (year(2019), day_of_year(1));
  modified_julian_day dm12_(year(2019), month(1), day_of_month(1));
  assert( dm12 == dm1 && dm12==dm12_ );

  modified_julian_day dm24(year(2018), dso::day_of_year(365));
  modified_julian_day dm24_(year(2018), month(12), day_of_month(31));
  assert( dm24 == dm2 && dm24==dm24_ );

  modified_julian_day dm13(year(2097), dso::day_of_year(1));
  modified_julian_day dm13_(year(2097), month(1), day_of_month(1));
  assert( modified_julian_day(86974) == dm13 && dm13==dm13_ );

  modified_julian_day dm14(year(1858), month(11), day_of_month(1));
  assert( dm14.to_julian_day() == 2399984.5e0 );
  
  modified_julian_day dm15(year(1858), month(1), day_of_month(1));
  assert( dm15.to_julian_day() == 2399680.5e0 );
  
  std::vector<modified_julian_day> mjds;
  mjds.push_back(dm12);
  mjds.push_back(dm24);
  mjds.push_back(dm13);
  mjds.push_back(dm14);
  mjds.push_back(dm15);
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
  // Start of the JD count is from 0 at 12 noon 1 JAN -4712 (4713 BC), Julian 
  // proleptic calendar
  // Results from SOFA
  // YYYY /MM/DD ->      JD
  // -4713/01/01 ->     -327.500
  // -4712/01/01 ->       37.500
  // -4711/01/01 ->      403.500
  // -0001/01/01 ->  1720694.500
  // 0000/01/01  ->  1721059.500
  // 0001/01/01  ->  1721425.500
  modified_julian_day dms1(year(-4713), month(1), day_of_month(1));
  assert( dms1.to_julian_day() == -327.5e0 );
  auto _dms1 = dms1.to_ymd();
  assert( _dms1.__year == year(-4713) && 
    (_dms1.__month == month(1) && _dms1.__dom == day_of_month(1)) );

  modified_julian_day dms2(year(-4712), month(1), day_of_month(1));
  assert( dms2.to_julian_day() == 37.5e0 );
  auto _dms2 = dms2.to_ymd();
  assert( _dms2.__year == year(-4712) && 
    (_dms2.__month == month(1) && _dms2.__dom == day_of_month(1)) );

  modified_julian_day dms3(year(-4711), month(1), day_of_month(1));
  assert( dms3.to_julian_day() == 403.5e0 );
  auto _dms3 = dms3.to_ymd();
  assert( _dms3.__year == year(-4711) && 
    (_dms3.__month == month(1) && _dms3.__dom == day_of_month(1)) );
  
  modified_julian_day dms4(year(-1), month(1), day_of_month(1));
  assert( dms4.to_julian_day() == 1720694.5e0 );
  auto _dms4 = dms4.to_ymd();
  assert( _dms4.__year == year(-1) && 
    (_dms4.__month == month(1) && _dms4.__dom == day_of_month(1)) );
  
  modified_julian_day dms5(year(0), month(1), day_of_month(1));
  assert( dms5.to_julian_day() == 1721059.5e0 );
  auto _dms5 = dms5.to_ymd();
  assert( _dms5.__year == year(0) && 
    (_dms5.__month == month(1) && _dms5.__dom == day_of_month(1)) );
  
  modified_julian_day dms6(year(1), month(1), day_of_month(1));
  assert( dms6.to_julian_day() == 1721425.5e0 );
  auto _dms6 = dms6.to_ymd();
  assert( _dms6.__year == year(1) && 
    (_dms6.__month == month(1) && _dms6.__dom == day_of_month(1)) );

  modified_julian_day dms7(year(-4713), month(11), day_of_month(24));
  assert( dms7.to_julian_day() == -0.5e0 );
  auto _dms7 = dms7.to_ymd();
  assert( _dms7.__year == year(-4713) && 
    (_dms7.__month == month(11) && _dms7.__dom == day_of_month(24)) );

  std::cout<<"\nAll checks for dso::modified_julian_day OK\n";
  return 0;
}
