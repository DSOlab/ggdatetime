#include "tpdate.hpp"

dso::TwoPartDate
dso::TwoPartDateUTC::operator-(const dso::TwoPartDateUTC &d) const noexcept {
  const int dmjd = this->imjd() - d.imjd();
  const double dsec = this->seconds() - d.seconds();
  double dleap = 0e0;
  if (dmjd) {
    /* not on same day */
    int dat1 = dso::dat(modified_julian_day(this->imjd()));
    int dat2 = dso::dat(modified_julian_day(d.imjd()));
    dleap = static_cast<double>(dat1 - dat2);
  }
  return TwoPartDate(dmjd, dsec + dleap);
}
