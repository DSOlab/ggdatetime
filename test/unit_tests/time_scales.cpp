#include "calendar.hpp"
#include <cmath>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

using namespace dso;
constexpr const int num_tests = 10000;
constexpr const double TOLERANCE = 1e-6; /* [sec] */
constexpr const double CTOLERANCE = 1e-9; /* [sec] */

int sign(FractionalSeconds sec) {
  const auto x = sec.seconds();
  return (x == 0) ? 0 : std::copysign(1.0, x);
}

int main() {

  for (int i=0; i<num_tests; i++) {
    /* random date */
    datetime<nanoseconds> d1 = datetime<nanoseconds>::random();
    TwoPartDate d2(d1);

    {
    /* TAI to TT */
    auto tmp1 = d1.tai2tt();
    auto tmp2 = d2.tai2tt();
    auto diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    auto diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* TT to TAI */
    tmp1 = d1.tt2tai();
    tmp2 = d2.tt2tai();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* check closure */
    tmp1 = d1.tai2tt().tt2tai();
    tmp2 = d2.tai2tt().tt2tai();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds())<CTOLERANCE);
    assert(std::abs(diff2.seconds())<CTOLERANCE);
    }
    
    {
    /* TAI to GPS */
    auto tmp1 = d1.tai2gps();
    auto tmp2 = d2.tai2gps();
    auto diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    auto diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* GPS to TAI */
    tmp1 = d1.gps2tai();
    tmp2 = d2.gps2tai();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* check closure */
    tmp1 = d1.tai2gps().gps2tai();
    tmp2 = d2.tai2gps().gps2tai();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    // printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds())<CTOLERANCE);
    assert(std::abs(diff2.seconds())<CTOLERANCE);
    }
    
    {
    /* TT to GPS */
    auto tmp1 = d1.tt2gps();
    auto tmp2 = d2.tt2gps();
    auto diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    auto diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    //printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* GPS to TAI */
    tmp1 = d1.gps2tt();
    tmp2 = d2.gps2tt();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    //printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds() - diff2.seconds())<TOLERANCE);
    assert(sign(diff1) == sign(diff2));

    /* check closure */
    tmp1 = d1.tt2gps().gps2tt();
    tmp2 = d2.tt2gps().gps2tt();
    diff1 = tmp1.diff<DateTimeDifferenceType::FractionalSeconds>(d1);
    diff2 = tmp2.diff<DateTimeDifferenceType::FractionalSeconds>(d2);
    //printf("%+12.9f %+12.9f\n", diff1.seconds(), diff2.seconds());
    assert(std::abs(diff1.seconds())<CTOLERANCE);
    assert(std::abs(diff2.seconds())<CTOLERANCE);
    }
  }


  return 0;
}
