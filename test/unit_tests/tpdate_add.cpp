#include "datetime_write.hpp"
#include <cassert>
#include <cstring>

using namespace dso;
constexpr const long SEC = nanoseconds::sec_factor<long>();

int main() {

  /* check dates */
  datetime<nanoseconds> d1(year(2023), month(10), day_of_month(24),
                            nanoseconds(0));
  TwoPartDate td1(d1);
  printf("Central date is                   : %.5f %.15e\n", td1.imjd(), td1.fday());

  for (int i=0; i<86400; i++) {
    td1.add_seconds(1e0);
  }
  printf("After adding   one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());
  
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<2*86400; i++) {
    td1.add_seconds(1e0);
    //if (i%3600==0) printf("\t%.5f %.15e\n", td1.imjd(), td1.fday());
  }
  printf("After adding   two days in seconds: %.5f %.15e\n", td1.imjd(), td1.fday());

  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<86400; i++) {
    td1.add_seconds(-1e0);
  }
  printf("After removing one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());
  
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<2*86400; i++) {
    td1.add_seconds(-1e0);
  }
  printf("After removing two days in seconds: %.5f %.15e\n", td1.imjd(), td1.fday());
  
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<86400; i++) {
    td1.add_seconds(2e0);
    td1.add_seconds(-1e0);
  }
  printf("After adding   one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());

  
  double error = 0e0;
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  printf("Central date is                   : %.5f %.15e\n", td1.imjd(), td1.fday());

  for (int i=0; i<86400; i++) {
    td1.add_seconds(1e0,error);
  }
  printf("After adding   one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());
  
  error = 0e0;
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<2*86400; i++) {
    td1.add_seconds(1e0,error);
    //if (i%3600==0) printf("\t%.5f %.15e\n", td1.imjd(), td1.fday());
  }
  printf("After adding   two days in seconds: %.5f %.15e\n", td1.imjd(), td1.fday());

  error = 0e0;
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<86400; i++) {
    td1.add_seconds(-1e0,error);
  }
  printf("After removing one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());
  
  error = 0e0;
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<2*86400; i++) {
    td1.add_seconds(-1e0,error);
  }
  printf("After removing two days in seconds: %.5f %.15e\n", td1.imjd(), td1.fday());
  
  error = 0e0;
  td1 = datetime<nanoseconds>(year(2023), month(10), day_of_month(24), nanoseconds(0));
  for (int i=0; i<86400; i++) {
    td1.add_seconds(2e0,error);
    td1.add_seconds(-1e0,error);
  }
  printf("After adding   one day in seconds : %.5f %.15e\n", td1.imjd(), td1.fday());

  return 0;
};
