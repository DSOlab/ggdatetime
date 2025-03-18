#include "dtdatetime.hpp"
#include <cassert>
#include <random>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;

using nsec = dso::nanoseconds;
typedef nsec::underlying_type SecIntType;
constexpr const SecIntType TOSEC = nsec::sec_factor<SecIntType>();

int main() {

  const auto i1 = datetime_interval<nsec>(0, nsec(-1));
  assert(i1.days() == 0);
  assert(i1.sec() == nsec(1));
  assert(i1.sign() == -1);

  const auto i2 = datetime_interval<nsec>(0, nsec(1));
  assert(i2.days() == 0);
  assert(i2.sec() == nsec(1));
  assert(i2.sign() == 1);

  const auto i3 = datetime_interval<nsec>(1, nsec(1));
  assert(i3.days() == 1);
  assert(i3.sec() == nsec(1));
  assert(i3.sign() == 1);

  const auto i4 = datetime_interval<nsec>(-1, nsec(1));
  assert(i4.days() == 1);
  assert(i4.sec() == nsec(1));
  assert(i4.sign() == -1);

  const auto i5 = datetime_interval<nsec>(nsec(-(86400 * TOSEC + 1)));
  assert(i5.days() == 1);
  assert(i5.sec() == nsec(1));
  assert(i5.sign() == -1);

  const auto i6 = datetime_interval<nsec>(-1, nsec(-1));
  assert(i6.days() == 1);
  assert(i6.sec() == nsec(1));
  assert(i6.sign() == -1);

  return 0;
}
