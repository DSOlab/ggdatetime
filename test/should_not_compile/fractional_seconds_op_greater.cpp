#include "calendar.hpp"

using namespace dso;

int main() {

  FractionalSeconds fsec(32.1);
  // should not compile because 32e0 is not a FractionalSeconds instance
  if (fsec > 32e0)
    return 1;
  return 2;
}
