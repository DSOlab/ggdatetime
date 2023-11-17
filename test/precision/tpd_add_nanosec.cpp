#include "tpdate.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

using namespace dso;

int main() {
  printf("Using Normal TwoPartDate::add_seconds()\n");

  TwoPartDate d(60224, 0e0);
  printf("Starting Date: %d %.15f\n", d.imjd(), d.seconds());

//Add 1 nano second 1'000'000'000 times to add a second 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(1e0 - d.seconds()));

///Dividing one MJD in 8 parts (every 10800seconds) and running the test to figure out how the precision changes

//test 1: adding 1 second in nanoseconds to an MJD with 10800 seconds
  d = TwoPartDate(60224, 10800e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(10801e0 - d.seconds()));

//test 2: adding 1 second in nanoseconds to an MJD with 21600 seconds
  d = TwoPartDate(60224, 21600e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(21601e0 - d.seconds()));

//test 3: adding 1 second in nanoseconds to an MJD with 32400 seconds
  d = TwoPartDate(60224, 32400e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(32401e0 - d.seconds()));

//test 4: adding 1 second in nanoseconds to an MJD with 43200 seconds
  d = TwoPartDate(60224, 43200e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(43201e0 - d.seconds()));

//test 5: adding 1 second in nanoseconds to an MJD with 54000 seconds
  d = TwoPartDate(60224, 54000e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(54001e0 - d.seconds()));

//test 6: adding 1 second in nanoseconds to an MJD with 64800 seconds
  d = TwoPartDate(60224, 64800e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(64801e0 - d.seconds()));

//test 7: adding 1 second in nanoseconds to an MJD with 75600 seconds
  d = TwoPartDate(60224, 75600e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(75601e0 - d.seconds()));

// //Encountering MJD change// //
//test 8: adding 1 second in nanoseconds to an MJD with 86399 seconds
  d = TwoPartDate(60224, 86399e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(0e0 - d.seconds()));

// // //More tests
//test 9: adding 1 second in nanoseconds to an MJD with 1 second
  d = TwoPartDate(60224, 1e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(2e0 - d.seconds()));

//test 10: adding 1 second in nanoseconds to an MJD with 90 seconds
  d = TwoPartDate(60224, 90e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(91e0 - d.seconds()));

// //Adding more than a second in nanoseconds
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
