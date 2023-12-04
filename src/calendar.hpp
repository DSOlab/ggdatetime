#ifndef __DSO_CALENDAR_GENINC_HPP__
#define __DSO_CALENDAR_GENINC_HPP__

#include "dtdatetime.hpp"
#include "tpdate.hpp"
#include "tpdate2.hpp"

namespace dso {

/** alias for datetime<T> */
template <typename T> using Datetime = datetime<T>;

/** alias for TwoPartDate */
using MjdEpoch = TwoPartDate;

} /* namespace dso */

#endif
