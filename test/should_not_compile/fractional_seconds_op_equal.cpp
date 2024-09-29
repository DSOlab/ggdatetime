#include "dtfund.hpp"

using namespace dso;

int main() {

  FractionalSeconds fsec(32.1);
  if (fsec == 32e0) return 1;
  return 2;

}
