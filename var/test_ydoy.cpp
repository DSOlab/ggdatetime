#include <cstdio>
#include <stdexcept>
#include <vector>
#include <random>
#include "dtfund.hpp"
#include "dtcalendar.hpp"

using dso::day_of_month;
using dso::day_of_year;
using dso::modified_julian_day;
using dso::month;
using dso::year;
using dso::seconds;
  
int main() {

  std::random_device rd; // only used once to initialise (seed) engine
  std::mt19937 rng(rd()); // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni_y(1970, 2025); // guaranteed unbiased
  std::uniform_int_distribution<long> uni_s(0, 86400); // guaranteed unbiased

  for (int n=0; n<1000; n++) {
    year yr (uni_y(rng));
    std::uniform_int_distribution<int> uni_d(1, 365+yr.is_leap());
    day_of_year doy (uni_d(rng));
    seconds sec = seconds(uni_s(rng));
    dso::datetime<seconds, dso::TimeScale::TAI> d {yr, doy, sec};
    auto ydoy = d.as_ydoy();
    assert(ydoy.__doy == doy + day_of_year(sec.as_underlying_type()==86400L));
  }

  printf("Ydoy check ok!\n");

  return 0;
}
