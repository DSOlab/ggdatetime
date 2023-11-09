#include "tpdate.hpp"
#include <cmath>
#include <chrono>
#include <iostream>

using namespace dso;

int main() {

  printf("Using Normal TwoPartDate::add_seconds()\n");
  printf("=================================================================\n");
  auto start = std::chrono::high_resolution_clock::now();
  TwoPartDate d(60224, 0e0);
  printf("Date: %d %.15f\n", d.imjd(), d.seconds());

  for (long i = 0; i < 1'000'000'000; i++) {
    d.add_seconds(1e-9);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  for (long i = 0; i < 1'000'000; i++) {
    d.add_seconds(1e-6);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  for (long i = 0; i < 1'000; i++) {
    d.add_seconds(1e-3);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  for (long i = 0; i < 1; i++) {
    d.add_seconds(1);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << duration.count() << "\n";

  printf("Using Kahan Summation\n");
  printf("=================================================================\n");
  start = std::chrono::high_resolution_clock::now();
  d = TwoPartDate(60224, 0e0);
  printf("Date: %d %.15f\n", d.imjd(), d.seconds());

  double err = 0;
  for (long i = 0; i < 1'000'000'000; i++) {
    d.add_seconds(1e-9,err);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  err = 0;
  for (long i = 0; i < 1'000'000; i++) {
    d.add_seconds(1e-6,err);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  err = 0;
  for (long i = 0; i < 1'000; i++) {
    d.add_seconds(1e-3,err);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));

  d = TwoPartDate(60224, 0e0);
  err = 0;
  for (long i = 0; i < 1; i++) {
    d.add_seconds(1,err);
  }
  printf("Date: %d %.15f Dsec=%.5e\n", d.imjd(), d.seconds(),
         std::abs(1e0 - d.seconds()));
  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << duration.count() << "\n";
  
  return 0;
}
