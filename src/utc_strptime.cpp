#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include "utcdates.hpp"
#include <charconv>
#include <cstring>

namespace {

typedef dso::nanoseconds::underlying_type SecIntType;
/*constexpr const SecIntType MAXSDAY = dso::nanoseconds::max_in_day;
constexpr const SecIntType FACTOR =
    dso::nanoseconds::template sec_factor<SecIntType>();*/

// skip any of the following characters:
// ' ', '/', '-', ':', 'T'
inline const char *skipni(const char *line) noexcept {
  const char *c = line;
  while (*c && (*c == ' ' || *c == '/' || *c == '-' || *c == ':' || *c == 'T'))
    ++c;
  return c;
}

} // unnamed namespace

double dso::utc_strptime_ymd_hms(const char *str,
                                 dso::modified_julian_day &utc_mjd,
                                 const char **stop) {
  int ints[5];
  double secs;
  const char *start = str;
  int sz = std::strlen(str);
  int error = 0;

  for (int i = 0; (i < 5) && (!error); ++i) {
    auto cerr = std::from_chars(skipni(start), str + sz, ints[i]);
    if (cerr.ec != std::errc{})
      ++error;
    start = cerr.ptr;
  }

  auto cerr = std::from_chars(skipni(start), str + sz, secs);
  if (cerr.ec != std::errc{})
    ++error;

  if (stop)
    *stop = cerr.ptr;

  // double seconds to integral
  double ndsec = secs * dso::nanoseconds::template sec_factor<double>();
  SecIntType nsec = static_cast<SecIntType>(ndsec);

  // resolve UTC date, assign utc_mjd
  dso::nanoseconds leap;
  const double utc_fday =
      dso::dtf2d(dso::year(ints[0]), dso::month(ints[1]),
                 dso::day_of_month(ints[2]), dso::hours(ints[3]),
                 dso::minutes(ints[4]), dso::nanoseconds(nsec), utc_mjd, leap);

  return utc_fday;
}

dso::TwoPartDate dso::utc_strptime_ymd_hms(const char *str, const char **stop) {
  dso::modified_julian_day mjd;
  const double fd = utc_strptime_ymd_hms(str, mjd, stop);
  return dso::TwoPartDate((double)mjd.as_underlying_type(), fd);
}
