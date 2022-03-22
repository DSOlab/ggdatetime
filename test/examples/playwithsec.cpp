#include "dtfund.hpp"
#include <iomanip>
#include <iostream>

using dso::microseconds;
using dso::milliseconds;
using dso::seconds;

double to_sec(dso::hours h, dso::minutes m, dso::seconds s, double fs) {
  long sec = h.as_underlying_type() * 3600 + m.as_underlying_type() * 60 +
             s.as_underlying_type();
  return (double)sec + fs;
}

int main() {
  seconds sec{1123};
  milliseconds mllsec{1234567};
  microseconds mcrsec{1234567890};

  std::cout << "\nThis is how many seconds:      " << sec.as_underlying_type();
  std::cout << "\nThis is how many milliseconds: "
            << mllsec.as_underlying_type();
  std::cout << "\nThis is how many microseconds: "
            << mcrsec.as_underlying_type();

  auto hmsf = sec.to_hmsf();
  std::cout << "\nHours: " << std::get<0>(hmsf).as_underlying_type()
            << ", Minutes: " << std::get<1>(hmsf).as_underlying_type()
            << ", Seconds: " << std::get<2>(hmsf).as_underlying_type();
  std::cout << "\n\tSeconds = "
            << to_sec(std::get<0>(hmsf), std::get<1>(hmsf), std::get<2>(hmsf),
                      0e0);

  std::cout << std::fixed;

  hmsf = mllsec.to_hmsf();
  // get milliseconds
  auto secnds = std::get<3>(hmsf);
  // milliseconds to fractional seconds
  double fractional_seconds =
      (double)secnds / milliseconds::sec_factor<double>();
  // add (integral) seconds
  fractional_seconds += (double)std::get<2>(hmsf).as_underlying_type();
  std::cout << "\nHours: " << std::get<0>(hmsf).as_underlying_type()
            << ", Minutes: " << std::get<1>(hmsf).as_underlying_type()
            << ", Seconds: " << std::setprecision(3) << fractional_seconds;
  std::cout << "\n\tSeconds = "
            << to_sec(std::get<0>(hmsf), std::get<1>(hmsf), std::get<2>(hmsf),
                      std::get<3>(hmsf) / milliseconds::sec_factor<double>());

  hmsf = mcrsec.to_hmsf();
  // get milliseconds
  secnds = std::get<3>(hmsf);
  // milliseconds to fractional seconds
  fractional_seconds = (double)secnds / microseconds::sec_factor<double>();
  // add (integral) seconds
  fractional_seconds += (double)std::get<2>(hmsf).as_underlying_type();
  std::cout << "\nHours: " << std::get<0>(hmsf).as_underlying_type()
            << ", Minutes: " << std::get<1>(hmsf).as_underlying_type()
            << ", Seconds: " << std::setprecision(6) << fractional_seconds;
  std::cout << "\n\tSeconds = "
            << to_sec(std::get<0>(hmsf), std::get<1>(hmsf), std::get<2>(hmsf),
                      std::get<3>(hmsf) / microseconds::sec_factor<double>());

  std::cout << "\n";
  return 0;
}
