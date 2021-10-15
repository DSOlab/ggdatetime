#include <iostream>
#include <vector>
#include <stdexcept>
#include "dtfund.hpp"

using dso::hours;
using dso::minutes;
using dso::microseconds;
using dso::seconds;

constexpr auto factor = microseconds::sec_factor<typename microseconds::underlying_type>();

int main()
{
  std::cout<<"\n[INFO] How many microseconds in day? "<<microseconds::max_in_day;
  
  // Testing microseconds constructors
  microseconds s1 (3600 * factor);
  assert(s1==microseconds(hours(1), minutes(0), microseconds(0)));

  microseconds s2 (86400 * factor);
  assert(s2==microseconds(hours(23), minutes(60), microseconds(0)));
  
  microseconds s3 (86401 * factor);
  assert(s3==microseconds(hours(22), minutes(60), microseconds(3600*factor+1*factor)));
  assert(s3==microseconds(86401.123465 *(double)factor)); // Note! this will be cast to an integer
                                     // before assignment; hence, it will be
                                     // exactly 86401 microseconds and the fractional
                                     // part will be ignored

  // conversion from ‘int’ to non-scalar type ‘dso::microseconds’
  // microseconds s4 = 86401; CMP_ERROR
  microseconds s5;
  s5 = 86402 * factor;
  // s5 = s3 + 1; CMP_ERROR

  // we can add and subtract microseconds w microseconds
  s5 = 3600 * factor;
  s5 = s1 + s2;
  assert(s5==microseconds(3600*factor+86400*factor));
  assert(s5.more_than_day());
  s5 = s2 - s1;
  assert(s5==microseconds(86400*factor-3600*factor));

  // remove whole days and keep fractional part (of day); e.g.
  int days = s5.remove_days();
  assert(days==0 && s5==microseconds(86400*factor-3600*factor));
  auto s6 = s3;
  days = s6.remove_days();
  assert(days==1 && s6==microseconds(1));
  // Note again that we can't add numerics with microseconds!
  // s6 = s6 + 86400; CMP_ERROR
  // or .. do not change the instance and compute whole days
  s6 += microseconds(86400.123465*factor);
  assert(s6.to_days()==1);
  // or .. compute fractional days
  assert(s1.fractional_days()==3600e0/86400e0);
  assert(s2.fractional_days()==1e0);
  assert(s3.fractional_days()==1e0+1e0/86400e0);

  // testing operators
  microseconds dm1 (1000);
  microseconds dm2 (999);
  microseconds dm3 (1000);
  assert( dm1 == dm3 );
  assert( dm1 != dm2 );
  assert( dm1 >= dm3 );
  assert( dm1 > dm2 );
  assert( dm2 < dm1 );
  assert( (dm2+=1) == dm1 );                 // microseconds += Integral_Type
  --dm2;                                     // --microseconds
  assert( ( dm2+=microseconds(1) ) == dm1 );      // microseconds += microseconds
  dm2--;                                     // microseconds--
  assert( (dm1-=1) == dm2 );                 // microseconds += Integral_Type
  ++dm1;                                     // ++microseconds
  assert( ( dm1-=microseconds(1) ) == dm2 );      // microseconds += microseconds
  dm1++;                                     // microseconds++
  assert( dm1 - microseconds(1) == dm2 ); 
  assert( dm2 + microseconds(1) == dm1 );

  std::cout<<"\nAll checks for dso::microseconds OK\n";
  return 0;
}
