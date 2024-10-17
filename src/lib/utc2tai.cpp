#include "tpdate.hpp"

dso::TwoPartDate dso::TwoPartDateUTC::utc2tai() const noexcept {
  FDOUBLE taisec = 0e0;
  int taimjd = this->utc2tai(taisec);
  return dso::TwoPartDate(taimjd, dso::FractionalSeconds{taisec});
}

dso::TwoPartDate dso::TwoPartDateUTC::utc2tt() const noexcept {
  FDOUBLE ttsec = 0e0;
  int ttmjd = this->utc2tai(ttsec);
  return dso::TwoPartDate(ttmjd, dso::FractionalSeconds{ttsec});
}
