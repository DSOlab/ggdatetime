#include "tpdate.hpp"
#include <cmath>

using namespace dso;

int main() {

  TwoPartDate d(60224, 0e0);
  printf("Date: %d %.15f\n", d.imjd(), d.seconds());

  for (long i=0; i<1'000'000'000; i++) {
    d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(), std::abs(1e0-d.seconds()));

  d = TwoPartDate(60224, 0e0);
  for (long i=0; i<1'000'000; i++) {
    d.add_seconds(1e-6);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(), std::abs(1e0-d.seconds()));
  
  d = TwoPartDate(60224, 0e0);
  for (long i=0; i<1'000; i++) {
    d.add_seconds(1e-3);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(), std::abs(1e0-d.seconds()));
  
  d = TwoPartDate(60224, 0e0);
  for (long i=0; i<1; i++) {
    d.add_seconds(1);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(), std::abs(1e0-d.seconds()));

  return 0;
}
