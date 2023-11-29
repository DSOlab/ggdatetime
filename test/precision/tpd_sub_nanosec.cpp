#include "tpdate.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

using namespace dso;

int main() {
  printf("Using Normal TwoPartDate::sub_seconds()\n");

  TwoPartDate d(60224, 0e0);
  printf("Starting Date: %d %.15f\n", d.imjd(), d.seconds());

//Subtract 1 nano second 1'000'000'000 times to subtract a second 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(86399e0 - d.seconds()));

///Dividing one MJD in 8 parts (every 10800seconds) and running the test to figure out how the precision changes

//test 1: subtracting 1 second in nanoseconds to an MJD with 10800 seconds
  d = TwoPartDate(60224, 10801e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(10800e0 - d.seconds()));

//test 2: subtracting 1 second in nanoseconds to an MJD with 21600 seconds
  d = TwoPartDate(60224, 21601e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(21600e0 - d.seconds()));

//test 3: subtracting 1 second in nanoseconds to an MJD with 32400 seconds
  d = TwoPartDate(60224, 32401e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(32400e0 - d.seconds()));

//test 4: subtracting 1 second in nanoseconds to an MJD with 43200 seconds
  d = TwoPartDate(60224, 43201e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(43200e0 - d.seconds()));

//test 5: subtracting 1 second in nanoseconds to an MJD with 54000 seconds
  d = TwoPartDate(60224, 54001e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(54000e0 - d.seconds()));

//test 6: subtracting 1 second in nanoseconds to an MJD with 64800 seconds
  d = TwoPartDate(60224, 64801e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(64800e0 - d.seconds()));

//test 7: subtracting 1 second in nanoseconds to an MJD with 75600 seconds
  d = TwoPartDate(60224, 75601e0); 
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(75600e0 - d.seconds()));

// //More tests
//test 8: subtracting 1 second in nanoseconds to an MJD with 1 second
  d = TwoPartDate(60224, 2e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(1e0 - d.seconds()));

//test 9: subtracting 1 second in nanoseconds to an MJD with 90 seconds
  d = TwoPartDate(60224, 91e0);
  for (long i = 0; i < 1'000'000'000; i++) {
  d.sub_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
     std::abs(90e0 - d.seconds()));

  return 0; 
}
