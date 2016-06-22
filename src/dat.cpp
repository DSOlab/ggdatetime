#include "dtfund.hpp"

/// \brief For a given UTC date, calculate delta(AT) = TAI-UTC.
///
/// If the specified date is for a day which ends with a leap second,
/// the UTC-TAI value returned is for the period leading up to the
/// leap second.  If the date is for a day which begins as a leap
/// second ends, the UTC-TAI returned is for the period following the
/// leap second.
///
/// \warning
///         - This version only works for post-1972 dates! For a more complete
///           version, see the iauDat.c routine from IAU's SOFA.
///         - No checks are performed for the validity of the input date.
///
/// \see IAU SOFA (iau-dat.c)
double
ngpt::dat(ngpt::year iy, ngpt::month im, ngpt::day_of_month dom)
noexcept
{

    // Dates and Delta(AT)s
    static constexpr struct {
        int iyear, month, delat;
    } changes[] = {
        { 1972,  1, 10 },
        { 1972,  7, 11 },
        { 1973,  1, 12 },
        { 1974,  1, 13 },
        { 1975,  1, 14 },
        { 1976,  1, 15 },
        { 1977,  1, 16 },
        { 1978,  1, 17 },
        { 1979,  1, 18 },
        { 1980,  1, 19 },
        { 1981,  7, 20 },
        { 1982,  7, 21 },
        { 1983,  7, 22 },
        { 1985,  7, 23 },
        { 1988,  1, 24 },
        { 1990,  1, 25 },
        { 1991,  1, 26 },
        { 1992,  7, 27 },
        { 1993,  7, 28 },
        { 1994,  7, 29 },
        { 1996,  1, 30 },
        { 1997,  7, 31 },
        { 1999,  1, 32 },
        { 2006,  1, 33 },
        { 2009,  1, 34 },
        { 2012,  7, 35 },
        { 2015,  7, 36 }
    };

    // Number of Delta(AT) changes
    constexpr int NDAT { (int) (sizeof changes / sizeof changes[0]) };

    // Combine year and month to form a date-ordered integer...
    int m = 12*iy + im;
    // ...and use it to find the preceding table entry.
    int idx = NDAT-1;
    for (; i >=0; i--) {
        if (m >= (12 * changes[i].iyear + changes[i].month)) break;
    }

    // Get the Delta(AT).
    return changes[i].delat;
}
