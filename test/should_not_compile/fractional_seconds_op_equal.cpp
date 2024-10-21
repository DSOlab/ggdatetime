#include "calendar.hpp"

using namespace dso;

int main() {

  FractionalSeconds fsec(32.1);
  // should not compile: 32e0 is not of type FractionalSeconds
  if (fsec == 32e0)
    return 1;
  return 2;
}
