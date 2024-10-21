#include "calendar.hpp"

int main() {

  dso::modified_julian_day mjd(123);
  // only allowed with ALLOW_DT_INTEGRAL_MATH
  mjd += 1;
  assert(mjd.as_underlying_type() == 124);
  return 0;
}
