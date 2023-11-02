#include "dtcalendar.hpp"
#include "utcdates.hpp"
/*
namespace {
typedef dso::nanoseconds::underlying_type SecIntType;
constexpr const SecIntType MAXSDAY = dso::nanoseconds::max_in_day;
constexpr const SecIntType FACTOR =
    dso::nanoseconds::template sec_factor<SecIntType>();
} // unnamed namespace

dso::TwoPartDate dso::dtf2d(dso::year yr, dso::month mt, dso::day_of_month dm,
                            dso::hours hr, dso::minutes mn,
                            dso::nanoseconds _sec,
                            dso::nanoseconds &leap) noexcept {
  dso::modified_julian_day big;
  const double fday = dso::dtf2d(yr, mt, dm, hr, mn, _sec, big, leap);
  return dso::TwoPartDate{big.as_underlying_type(), fday}.normalized();
}

double dso::dtf2d(dso::year yr, dso::month mt, dso::day_of_month dm,
                  dso::hours hr, dso::minutes mn, dso::nanoseconds _sec,
                  dso::modified_julian_day &mjd,
                  dso::nanoseconds &leap) noexcept {

  // calendar date to MJD
  mjd = dso::modified_julian_day(yr, mt, dm);

  // the time in seconds of day
  dso::nanoseconds sinday(hr, mn, _sec);

  // max *seconds in day, in S resolution
  dso::nanoseconds maxsday(MAXSDAY);

  // deal with the UTC leap second
  int extra_seconds_in_day;
  [[maybe_unused]] int leap_sec = dso::dat(mjd, extra_seconds_in_day);

  // leap seconds to S (for user)
  leap = dso::nanoseconds(leap_sec * FACTOR);

  // do we have extra seconds ?
  if (extra_seconds_in_day) {
    maxsday += dso::nanoseconds(extra_seconds_in_day * FACTOR);
  }

  // validate input time
  assert(hr.as_underlying_type() < 24 && mn.as_underlying_type() < 60 &&
         _sec < maxsday);

// do we have an extra day (in S) ? what id we do ?
#ifdef DEBUG
  if (sinday > maxsday) {
    fprintf(stderr, "Computed overflowing seconds in day! -> %ld\n",
            sinday.as_underlying_type());
  }
  assert(sinday <= maxsday);
#endif

  const double dsec = static_cast<double>(sinday.as_underlying_type());
  const double msec = static_cast<double>(maxsday.as_underlying_type());
  return dsec / msec;
}
*/
