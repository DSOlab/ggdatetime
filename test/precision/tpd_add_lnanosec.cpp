#include "tpdate.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

using namespace dso;

int main() {
  printf("Using Normal TwoPartDate::add_seconds()\n");

  TwoPartDate d(60224, 0e0);
  printf("Starting Date: %d %.15f\n", d.imjd(), d.seconds());

  d = TwoPartDate(60224, 0e0);

//Add 1 nano second 10'000'000'000 times to add 10 seconds
  for (long i = 0; i < 10'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(10e0 - d.seconds()));
    
  d = TwoPartDate(60224, 0e0);
//Add 1 nano second 2'000'000'000 times to add 2 seconds
  for (long i = 0; i < 2'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(2e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
//Add 1 nano second 1'500'000'000 times to add 1.5 seconds
  for (long i = 0; i < 1'500'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(1.5e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
//Add 1 nano second 500'000'000 times to add 0.5 seconds
  for (long i = 0; i < 500'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(.5e0 - d.seconds()));
 return 0; 
}
