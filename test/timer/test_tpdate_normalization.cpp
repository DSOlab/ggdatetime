#include "dtcalendar.hpp"
#include "tpdate.hpp"
#include <cassert>
#include <random>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std::chrono;

constexpr const long num_tests = 1'000'000;
using nsec = dso::nanoseconds;
using dso::datetime;
typedef nsec::underlying_type SecIntType;
using dso::TwoPartDate;
using dso::SEC_PER_DAY;
//constexpr const SecIntType TOSEC = nsec::sec_factor<SecIntType>();
//constexpr const double SECINDAY = nsec::max_in_day;

int main() {
  /* Generators for random numbers ... */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> ydstr(1972, 2050); /* range for years */
  std::uniform_int_distribution<> mdstr(1, 12);      /* range for months */
  std::uniform_int_distribution<> ddstr(1, 31); /* range for day of month */
  std::uniform_int_distribution<SecIntType> nsdstr(
      0, nsec::max_in_day); /* range for day of month */
  std::uniform_real_distribution<double> rnds(-86400e0, 86400e0);

  for (int Y=0; Y<5; Y++) {
  auto start = high_resolution_clock::now();
  int testnr = 0, ok, donotoptimize=0;
  datetime<nsec> d1;
  TwoPartDate tpd;
  while (testnr < num_tests) {
    /* do we have a valid date ? */
    try {
      d1 = datetime<nsec>{dso::year(ydstr(gen)), dso::month(mdstr(gen)),
                          dso::day_of_month(ddstr(gen)), nsec(nsdstr(gen))};
      ok = 1;
    } catch (std::exception &) {
      ok = 0;
    }
    if (ok) {
      std::vector<double> dv;
      /* construct a TwoPartDate from a datetime */
      TwoPartDate tpd1(d1);
      for (int i = 0; i < 10; i++) {
        double s = rnds(gen);
        /* do smthng studip to empty cache */
        for (int j = 0; j < 10; j++) {
          dv.push_back(s + j);
        }
        tpd1.add_seconds(s / SEC_PER_DAY);
        for (int j = 0; j < 10; j++) {
          if (dv[j] < 0e0) ++ok;
        }
        donotoptimize += ok - 100;
        if (donotoptimize>1000) tpd = tpd1;
      }
    }
    ++testnr;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  std::cout << "Time spent on fucntion: " << duration.count() << "microsec\n";
  printf("Here is smthng irrelevant, dummy=%d, mjd=%.1f\n", donotoptimize,
         tpd.imjd());
  }

  return 0;
}
