#include <iostream>
#include <vector>
#include <stdexcept>
#include "dtfund.hpp"

using ngpt::hours;
using ngpt::minutes;
using ngpt::seconds;

int main()
{
  std::cout<<"\n[INFO] How many seconds in day? "<<seconds::max_in_day;
  
  // Testing seconds constructors
  seconds s1 (3600);
  assert(s1==seconds(hours(1), minutes(0), seconds(0)));

  seconds s2 (86400);
  assert(s2==seconds(hours(23), minutes(60), seconds(0)));
  
  seconds s3 (86401);
  assert(s3==seconds(hours(22), minutes(60), seconds(3600+1)));
  assert(s3==seconds(86401.123465)); // Note! this will be cast to an integer
                                     // before assignment; hence, it will be
                                     // exactly 86401 seconds and the fractional
                                     // part will be ignored

  // conversion from ‘int’ to non-scalar type ‘ngpt::seconds’
  // seconds s4 = 86401; CMP_ERROR
  seconds s5;
  s5 = 86402;
  // s5 = s3 + 1; CMP_ERROR

  // we can add and subtract seconds w seconds
  s5 = 3600;
  s5 = s1 + s2;
  assert(s5==seconds(3600+86400));
  assert(s5.more_than_day());
  s5 = s2 - s1;
  assert(s5==seconds(86400-3600));

  // remove whole days and keep fractional part (of day); e.g.
  int days = s5.remove_days();
  assert(days==0 && s5==seconds(86400-3600));
  auto s6 = s3;
  days = s6.remove_days();
  assert(days==1 && s6==seconds(1));
  // Note again that we can't add numerics with seconds!
  // s6 = s6 + 86400; CMP_ERROR
  // or .. do not change the instance and compute whole days
  s6 += seconds(86400.123465);
  assert(s6.to_days()==1);
  // or .. compute fractional days
  assert(s1.fractional_days()==3600e0/86400e0);
  assert(s2.fractional_days()==1e0);
  assert(s3.fractional_days()==1e0+1e0/86400e0);

  // testing operators
  seconds dm1 (1000);
  seconds dm2 (999);
  seconds dm3 (1000);
  assert( dm1 == dm3 );
  assert( dm1 != dm2 );
  assert( dm1 >= dm3 );
  assert( dm1 > dm2 );
  assert( dm2 < dm1 );
  assert( (dm2+=1) == dm1 );                 // seconds += Integral_Type
  --dm2;                                     // --seconds
  assert( ( dm2+=seconds(1) ) == dm1 );      // seconds += seconds
  dm2--;                                     // seconds--
  assert( (dm1-=1) == dm2 );                 // seconds += Integral_Type
  ++dm1;                                     // ++seconds
  assert( ( dm1-=seconds(1) ) == dm2 );      // seconds += seconds
  dm1++;                                     // seconds++
  assert( dm1 - seconds(1) == dm2 ); 
  assert( dm2 + seconds(1) == dm1 );
  // however, we should not be able to do seconds+=microseconds
  // dm1+=(ngpt::milliseconds(1)); CMP_ERROR
  // dm1-=(ngpt::milliseconds(1)); CMP_ERROR
  // we could cast though the milliseconds to seconds and do the operation
  ngpt::milliseconds mlsec(1000);
  assert( (dm2+ngpt::cast_to<ngpt::milliseconds, seconds>(mlsec)) == dm1 );

  // Cast seconds to milli/micro - seconds
  ngpt::milliseconds m1 (1001); // Note that casting milliseconds to seconds
                                // will result in loss of accuracy; 1001 mlsec
                                // is 1.001 sec, from which we only keep the
                                // integral part
  // implicit cast at constructor
  seconds s7 (m1);
  // cast with static_cast<> and using the operator
  seconds s8 = static_cast<seconds>(m1);
  seconds s9 = seconds(m1);
  assert(s7==s8 && s8==s9);
  assert(s7==seconds(1));

  std::cout<<"\nAll checks for ngpt::seconds OK\n";
  return 0;
}
