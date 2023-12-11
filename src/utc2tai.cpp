#include "tpdate.hpp"

dso::TwoPartDate dso::TwoPartDateUTC::utc2tai() const noexcept {
  FDOUBLE taisec;
  int taimjd = this->utc2tai(taisec);
  return dso::TwoPartDate(taimjd, dso::FractionalSeconds{taisec});
}

dso::TwoPartDate dso::TwoPartDateUTC::utc2tt() const noexcept {
  FDOUBLE ttsec;
  int ttmjd = this->utc2tai(ttsec);
  return dso::TwoPartDate(ttmjd, dso::FractionalSeconds{ttsec});
}
