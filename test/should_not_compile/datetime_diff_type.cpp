#include "calendar.hpp"

using namespace dso;

int main() {

  auto d1 = datetime<nanoseconds>(year(2023), month(1), day_of_month(31));
  auto d2 = datetime<nanoseconds>(year(2023), month(2), day_of_month(1));

  // should not compile because 1e0 is not of type FractionalDays
  if (d2.diff<DateTimeDifferenceType::FractionalDays>(d1) > 1e0)
    return 1;
  return 2;
}
