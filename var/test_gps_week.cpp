#include <iostream>
#include <stdexcept>
#include "dtfund.hpp"

using dso::gps_week;

int main()
{
  gps_week gw1 {1920};
  gps_week gw2 {1919};
  gw2 = 1919 +1 -1;
  // gw21 = gw2 + 1 -1; CMP_ERROR
  // gps_week gw22 = gw1 + 1; CMP_ERROR
  // gps_week gw24 = gw2.as_underlying_type() + 1; CMP_ERROR
  // gps_week gw24 = 8; CMP_ERROR
  gps_week gw3(gw1);
  
  // testing operators
  assert( gw1 == gw3 );
  assert( gw1 != gw2 );
  assert( gw1 >= gw3 );
  assert( gw1 > gw2 );
  assert( gw2 < gw1 );
  assert( (gw2+=1) == gw1 );             // gps_week += Integral_Type
  --gw2;                                 // --gps_week
  assert( ( gw2+=gps_week(1) ) == gw1 ); // gps_week += gps_week
  gw2--;                                 // gps_week--
  assert( (gw1-=1) == gw2 );             // gps_week += Integral_Type
  ++gw1;                                 // ++gps_week
  assert( ( gw1-=gps_week(1) ) == gw2 ); // gps_week += gps_week
  gw1++;                                 // gps_week++

  // testing functions
  assert( gw1.is_valid() );
  assert( gw2.is_valid() );
  gw1 = gps_week(-500);
  gw2 = gps_week(-1);
  assert( !gw1.is_valid() );
  assert( !gw2.is_valid() );

  std::cout<<"\nAll checks for dso::gps_week OK\n";
  return 0;
}
