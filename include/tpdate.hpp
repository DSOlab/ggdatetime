/** @file
 *
 * Define classes TwoPartDate and TwoPartDateUTC. These are classes to
 * represent time-points, i.e. epochs or instances in the datetime domain. The
 * difference between the two, is that the first (TwoPartDate) is meant to
 * work on a 'continuous time basis' e.g. in time scales such as TAI, or TT.
 * The latter, i.e. TwoPartDateUTC is meant to be used solely to represent
 * isntances in the UTC time-scale. This needs special treatment cause of the
 * leap seconds that are introduced, making the time-scale non-continuous.
 *
 * Both classes store the datetime instances as a mixture of (integral) MJDay
 * and (floating point) seconds of day.
 */

#ifndef __DSO_DATETIME_TWOPARTDATES_HPP__
#define __DSO_DATETIME_TWOPARTDATES_HPP__

#include "datetime_utc.hpp"
#include <random>

namespace dso {

  /** forward decleration */
  class TwoPartDate;

  /** A datetime class to represent epochs in UTC time system.
   *
   * A TwoPartDate instance conviniently splits a datetime into two numeric
   * values:
   * - the Modified Julian Day (i.e. an integral value), and
   * - the time of day, which is stored in fractional seconds of day
   *
   * The methods of the class, including constructors, take special care to
   * always keep the seconds as seconds of day, i.e. in the range [0,86400) and
   * correspondingly increase/decrease the day count.
   */
  class TwoPartDateUTC {
    private:
      using FDOUBLE = /*long*/ double;
      /** MJDay */
      int _mjd;
      /** fractional seconds of day. */
      FDOUBLE _fsec;

      /** @brief Return 1 or  0 depending on leap seconds.
       *
       * Check the integral (MJD) part of the date; if we are on a leap
       * insertation day, return 1, otherwise return 0
       */
      constexpr int extra_seconds_in_day() const noexcept {
        // dat(modified_julian_day(_mjd), extra);
        return modified_julian_day(_mjd).is_leap_insertion_day();
      }

      /** @brief Transform a UTC date to a TAI date.
       *
       * The TAI date is returned in two parts:
       * 1. The MJD part (integral), and
       * 2. The seconds of day part (fractional)
       * Note that the fractional seconds part may be over 86400.
       */
      int utc2tai(FDOUBLE &taisec) const noexcept {
        taisec = _fsec + dat(modified_julian_day(_mjd));
        return _mjd;
      }

      /** @brief Transform a UTC date to a TT date.
       *
       * The TT date is returned in two parts:
       * 1. The MJD part (integral), and
       * 2. The seconds of day part (fractional)
       * Note that the fractional seconds part may be over 86400.
       */
      int utc2tt(FDOUBLE &taisec) const noexcept {
        constexpr const FDOUBLE dtat = TT_MINUS_TAI;
        taisec = _fsec + (dat(modified_julian_day(_mjd)) + dtat);
        return _mjd;
      }

      /** @brief Constructor from MJDay (integral) and seconds of day. */
      explicit TwoPartDateUTC(int mjd, FDOUBLE secday) noexcept
        : _mjd(mjd), _fsec(secday) {
          normalize();
        }

      /** @brief Constructor from MJDay (integral) and seconds of day.
       *
       * @warning This constructor will not call normalize! Be sure that the
       * input arguments are correct, i.e. secday is indeed seconds of day.
       * */
      constexpr explicit TwoPartDateUTC(int mjd, FDOUBLE secday,
          [[maybe_unused]] char c) noexcept
        : _mjd(mjd), _fsec(secday) {}

      /** @brief Add seconds to instance, taking into account leap seconds.
      */
      void add_seconds_inplace(FDOUBLE sec) noexcept {
        _fsec += sec;
        this->normalize();
      }

    public:
      /** Constructor from datetime<T> */
#if __cplusplus >= 202002L
      template <gconcepts::is_sec_dt T>
#else
        template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
        constexpr TwoPartDateUTC(const datetime<T> &d) noexcept
        : _mjd(d.imjd().as_underlying_type()),
        _fsec(to_fractional_seconds<T>(d.sec()).seconds()) {
        }

      /** @brief Constructor from MJDays (integral) and seconds of day. */
      explicit TwoPartDateUTC(int b = 0,
          FractionalSeconds s = FractionalSeconds{0}) noexcept
        : _mjd(b), _fsec(s.seconds()) {
          /* do not normalize for default constructor! */
          if (b != 0 && s.seconds() != 0e0)
            this->normalize();
        }
      
      /** @brief Constructor from calendar date. */
      explicit TwoPartDateUTC(const ymd_date &ymd) noexcept
      : _mjd(modified_julian_day(ymd.yr(), ymd.mn(), ymd.dm()).as_underlying_type()), 
      _fsec(0e0) {}

      /** @brief Reference epoch (J2000.0), as a Modified Julian Date. */
      static constexpr TwoPartDateUTC j2000_mjd() noexcept {
        return TwoPartDateUTC(51544, 86400e0 / 2e0, 'y');
      }

      /** @brief Min date. This is the same as datetime<T>::min(). */
      static constexpr TwoPartDateUTC min() noexcept {
        return TwoPartDateUTC(datetime<nanoseconds>::min());
      }

      /** @brief Max date. This is the same as datetime<T>::max(). */
      static constexpr TwoPartDateUTC max() noexcept {
        return TwoPartDateUTC(datetime<nanoseconds>::max());
      }

      /** @brief Get the MJD as an intgral number, i.e. no fractional part */
      int imjd() const noexcept { return _mjd; }

      /** @brief Get the (fractional) seconds of the MJD. Always in [0, 86400). */
      FractionalSeconds seconds() const noexcept {
        return FractionalSeconds(_fsec);
      }

      /** @brief Get the seconds of the MJD as fractional day. Always in [0,1) */
      FractionalDays fractional_days() const noexcept {
        return FractionalDays(_fsec / (86400e0 + this->extra_seconds_in_day()));
      }

      /** @brief Get the fractional seconds of day as some multiple of seconds.
       *
       * 'Some multiple of seconds' can be any type defined with a member
       * is_of_sec_type set to true. In practical terms, this means that you can
       * use seconds, milliseconds, microseconds or nanoseconds.
       * So, in essence this function will give you the seconds of day, or the
       * milliseconds of day, or ...
       * The function will return the [whatever]seconds of day as a floating point
       * number.
       *
       * @return Fractional day as fractional T-type second multiples. I.e.
       *         fractional seconds of day, or milliseconds of day, or
       *         microseconds of day, etc ...
       */
#if __cplusplus >= 202002L
      template <gconcepts::is_sec_dt T>
#else
        template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
        constexpr FDOUBLE sec_of_day() const noexcept {
          // return TwoPartDate(_mjd, _fsec, 'c').sec_of_day<T>();
          return seconds().seconds() * T::template sec_factor<FDOUBLE>();
        }

      /** @brief Transform the (integral part of the) date to Year Month Day */
      ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

      /** @brief Add seconds to instance, taking into account leap seconds. */
      void add_seconds_inplace(FractionalSeconds fsec) noexcept {
        this->add_seconds_inplace(fsec.seconds());
      }
      
      [[nodiscard]] TwoPartDateUTC add_seconds(FractionalSeconds fsec) const noexcept {
        TwoPartDateUTC cpy(*this);
        cpy.add_seconds_inplace(fsec.seconds());
        return cpy;
      }

      /** Add seconds to instance and return the "Kahan summation" error.
       *
       * This function implements a "Kahan summation" scheme to iteratively add
       * seconds (to the instance). Especially in the case of adding multiple
       * times, small multiples of second (i.e. nanoseconds and/or microseconds),
       * it gives better precision than the simple add_seconds method.
       * Example usage:
       * d = TwoPartDateUTC(...);
       * double err = 0;
       * for (long i = 0; i < 1'000'000'000; i++) {
       *    d.add_seconds(1e-9,err);
       * }
       * This version will give better results that using
       * d = TwoPartDateUTC(...);
       * double err = 0;
       * for (long i = 0; i < 1'000'000'000; i++) {
       *    d.add_seconds(1e-9);
       * }
       * For accuracy/precision results, see test/precision/tpadd.cpp
       *
       * @param[in] sec  Floating-point seconds to add to instance
       * @param[out] err Previous summation error at input; updated at output to
       *                 be used at next iteration. If this is the first call,
       *                 set err to 0e0.
       */
      void add_seconds(FractionalSeconds sec, FDOUBLE &err) noexcept {
        FDOUBLE a = _fsec;
        FDOUBLE b = sec.seconds();
        FDOUBLE y = b - err;
        b = a + y;
        err = (b - a) - y;
        _fsec = b;
        this->normalize();
      }

      /** @brief Differencce betweeen two UTC dates
       *
       * The difference is returned in two parts,
       * 1. An integral part, holding whole days of 86400[sec], and
       * 2. remaining seconds
       * This function will take into consideration the leap seconds between the
       * two days.
       * Note that the whole days returned are days of 86400[sec] aka **NOT**
       * UTC days. This is because this (i.e. the returned instance) is actually
       * a time interval, not a datetime!
       *
       * Example:
       * a leap insertion date
       * d1 = TwoPartDateUTC(ymd_date{year(2016), month(12), day_of_month(31))
       * d2 = TwoPartDateUTC(ymd_date{year(2017), month(1), day_of_month(1))
       * auto diff = d1 - d2
       * now: diff.imjd() == 1 and diff.seconds() == 1
       */
      TwoPartDate operator-(const TwoPartDateUTC &d) const noexcept;

      /** @brief Normalize an instance.
       *
       * Normalize here is meant in the sense that the fractional seconds of day
       * (\p _fsec) are always in the range [0,86400+leap_insertion_secs), while
       * the integral part (\p _mjd) is always a valid integer.
       */
      void normalize() noexcept {
        if (_fsec >= 0e0 && _fsec < 86400e0)
          return;
        int extra_sec_in_day;
        dat(modified_julian_day(_mjd), extra_sec_in_day);
        /* for each MJD, remove integral days. Each MJD may have a different
         * number of seconds, since we are in UTC time scale. Hence, iteratively
         * remove whole days using the number of seconds for each day.
         */
        while (_fsec >= 86400e0 + extra_sec_in_day) {
          _fsec -= (86400e0 + extra_sec_in_day);
          ++_mjd;
          dat(modified_julian_day(_mjd), extra_sec_in_day);
        }
        while (_fsec < 0e0) {
          --_mjd;
          _fsec += 86400e0;
          dat(modified_julian_day(_mjd), extra_sec_in_day);
          _fsec += 1e0 * extra_sec_in_day;
        }
#ifdef DEBUG
        if (_mjd)
          assert(_fsec >= 0e0 && _fsec < 86400e0 + extra_sec_in_day);
        else
          assert(_fsec < 0e0 && _fsec > -86400e0);
#endif
        /* all done */
        return;
      }

      /** @brief Transform a UTC date to a TAI date, via TAI = UTC + ΔAT. */
      TwoPartDate utc2tai() const noexcept;

      /** @brief Transform a UTC date to a TT date */
      TwoPartDate utc2tt() const noexcept;

      /** @brief Overload the '>' operator. */
      bool operator>(const TwoPartDateUTC &d) const noexcept {
        return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec > d._fsec));
      }

      /** @brief Overload the '>=' operator. */
      bool operator>=(const TwoPartDateUTC &d) const noexcept {
        return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec >= d._fsec));
      }

      /** @brief Overload the '<' operator. */
      bool operator<(const TwoPartDateUTC &d) const noexcept {
        return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec < d._fsec));
      }

      /** @brief Overload the '<=' operator. */
      bool operator<=(const TwoPartDateUTC &d) const noexcept {
        return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec <= d._fsec));
      }

      /** @brief Overload equality operator. */
      bool operator==(const TwoPartDateUTC &d) const noexcept {
        return (_mjd == d._mjd) && (_fsec == d._fsec);
      }

      /** @brief Overload in-equality operator */
      bool operator!=(const TwoPartDateUTC &d) const noexcept {
        return !(this->operator==(d));
      }

  }; /* class TwoPartDateUTC */

  /** A datetime class to represent epochs in any continuous system.
   *
   * A TwoPartDate instance conviniently splits a datetime into two numeric
   * values:
   * - the Modified Julian Day (i.e. an integral value), and
   * - the time of day, which is stored in fractional seconds of day
   *
   * The methods of the class, including constructors, take special care to
   * always keep the seconds as seconds of day, i.e. in the range [0,86400) and
   * correspondingly increase/decrease the day count.
   */
  class TwoPartDate {
    private:
      using FDOUBLE = /*long*/ double;
      int _mjd;      /** Mjd */
      FDOUBLE _fsec; /** fractional seconds of day in [0, 86400) */

      /** @brief Construct from MJD and fractional seconds.
       *
       * This is only private and should be used in rare cases. Normal users,
       * should explicitely cast the second argument to FractionalSeconds to
       * avoid misconceptions (i.e. avoid ambiguous parameters, fractional seconds
       * or fractional days, or ...).
       */
      TwoPartDate(int mjd, FDOUBLE secday) noexcept : _mjd(mjd), _fsec(secday) {
        normalize();
      }

      /** @brief Add seconds to instance.
       *
       * This version is private; users should use the
       * add_seconds(FractionalSeconds ...) version, which enforces type safety.
       */
      void add_seconds_inplace(FDOUBLE sec) noexcept {
        _fsec += sec;
        this->normalize();
      }

    protected:
      /** @brief Construct from MJD and fractional seconds **of day**.
       *
       * A constexpr constructor that will not check arguments, and will NOT
       * normalize the date. Be very carefull with this one!
       * The char parameter actually does nothing, but is there to discriminate
       * this constructor from the 'normalizing' ones.
       *
       * @wanrning Will not call nomralize, given secday should be in range
       *           [0. 86400)
       */
      constexpr TwoPartDate(int mjd, FDOUBLE secday,
          [[maybe_unused]] char c) noexcept
        : _mjd(mjd), _fsec(secday) {}

    public:
      /** @brief Constructor from datetime<T>.
       *
       * Note that we are not (explicitly) normalizing the instance here, because
       * the parameter \p d is already considered to hold a 'normalized' datetime.
       */
#if __cplusplus >= 202002L
      template <gconcepts::is_sec_dt T>
#else
        template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
        constexpr explicit TwoPartDate(const datetime<T> &d) noexcept
        : _mjd(d.imjd().as_underlying_type()),
        _fsec(to_fractional_seconds<T>(d.sec()).seconds()) {
        }

      /** @brief Constructor from calendar date. */
      explicit TwoPartDate(const ymd_date &ymd) noexcept
      : _mjd(modified_julian_day(ymd.yr(), ymd.mn(), ymd.dm()).as_underlying_type()), 
      _fsec(0e0) {}

      /** @brief Reference epoch (J2000.0), as a Modified Julian Date. */
      static constexpr TwoPartDate j2000_mjd() noexcept {
        return TwoPartDate(51544, 86400e0 / 2e0, 'y');
      }

      /** @brief Random Date within some MJD limits
       * @todo transfer this into a .cpp file
       */
      static TwoPartDate
        random(modified_julian_day from = modified_julian_day::min(),
            modified_julian_day to = modified_julian_day::max()) noexcept {
          int istart = (int)from.as_underlying_type();
          int istop = (int)to.as_underlying_type();
          std::random_device rd;
          std::mt19937 gen(rd());
          std::uniform_int_distribution<int> distr(istart, istop);
          std::uniform_real_distribution<double> unif(0, 86400e0);
          return TwoPartDate(distr(gen), unif(gen), 'y');
        }

      /** @brief Min date. This is the same as datetime<T>::min(). */
      static constexpr TwoPartDate min() noexcept {
        return TwoPartDate(datetime<nanoseconds>::min());
      }

      /** @brief Max date. This is the same as datetime<T>::max(). */
      static constexpr TwoPartDate max() noexcept {
        return TwoPartDate(datetime<nanoseconds>::max());
      }

      /** @brief Constructor from MJDay and FractionalSeconds.  */
      explicit TwoPartDate(int b = 0,
          FractionalSeconds s = FractionalSeconds{0}) noexcept
        : _mjd(b), _fsec(s.seconds()) {
          this->normalize();
        }

      /** @brief Constructor from MJDay; FractionalSeconds are set to 0.  */
      constexpr explicit TwoPartDate(modified_julian_day mjd) noexcept
        : _mjd(mjd.as_underlying_type()), _fsec(0) {};

      /** @brief Constructor from calendar date. */
      explicit TwoPartDate(year y, month m, day_of_month d, double sec_of_day = 0e0)
        : _mjd(modified_julian_day(y, m, d).as_underlying_type()),
        _fsec(sec_of_day) {
          this->normalize();
        }

      /** @brief Constructor from year, day of year and time of day. */
      explicit TwoPartDate(year y, day_of_year d, double sec_of_day = 0e0)
        : _mjd(modified_julian_day(y, d).as_underlying_type()),
        _fsec(sec_of_day) {
          this->normalize();
        }

      /** @brief Constructor from CNES Julian Date.
       * see https://en.wikipedia.org/wiki/Julian_day
       */
      static TwoPartDate
        from_cnes_jd(double cnes_jd) noexcept {
          double ipart;
          const double fpart = std::modf(cnes_jd, &ipart);
          return TwoPartDate((int)ipart+(int)MJD_MINUS_CNESJD, FractionalSeconds(fpart*86400e0));
        }

      /** @brief Get the MJD as an intgral number, i.e. no fractional part. */
      int imjd() const noexcept { return _mjd; }

      /** @brief Get the (fractional) seconds of the MJD. Always in [0, 86400). */
      FractionalSeconds seconds() const noexcept {
        return FractionalSeconds(_fsec);
      }

      /** @brief Get the seconds of the MJD as fractional day. Always in [0,1). */
      FractionalDays fractional_days() const noexcept {
        return FractionalDays(_fsec / 86400e0);
      }

      /** @brief Get the fractional seconds of day as some multiple of seconds.
       *
       * 'Some multiple of seconds' can be any type defined with a member
       * is_of_sec_type set to true. In practical terms, this means that you can
       * use seconds, milliseconds, microseconds or nanoseconds.
       * So, in essence this function will give you the seconds of day, or the
       * milliseconds of day, or ...
       * The function will return the [whatever]seconds of day as a floating point
       * number.
       *
       * @return Fractional day as fractional T-type second multiples. I.e.
       *         fractional seconds of day, or milliseconds of day, or
       *         microseconds of day, etc ...
       */
#if __cplusplus >= 202002L
      template <gconcepts::is_sec_dt T>
#else
        template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
        FDOUBLE sec_of_day() const noexcept {
          return seconds().seconds() * T::template sec_factor<FDOUBLE>();
        }

      /** @brief Transform the (integral part of the) date to Year Month Day */
      ymd_date to_ymd() const noexcept { return core::mjd2ymd((long)_mjd); }

      /** @brief Transform the (integral part of the) date to Year Day-Of-Year */
      ydoy_date to_ydoy() const noexcept {
        const modified_julian_day mjd(_mjd);
        return mjd.to_ydoy();
      }

      /** @brief Add seconds to instance. */
      void add_seconds_inplace(FractionalSeconds fsec) noexcept {
        this->add_seconds_inplace(fsec.seconds());
      }

      [[nodiscard]] TwoPartDate add_seconds(FractionalSeconds fsec) const noexcept {
        TwoPartDate cpy(*this);
        cpy.add_seconds_inplace(fsec.seconds());
        return cpy;
      }


      /** @brief Add seconds to instance and return the "Kahan summation" error.
       *
       * This function implements a "Kahan summation" scheme to iteratively add
       * seconds (to the instance). Especially in the case of adding multiple
       * times, small multiples of second (i.e. nanoseconds and/or microseconds),
       * it gives better precision than the simple add_seconds method.
       * Example usage:
       * d = TwoPartDate(...);
       * double err = 0;
       * for (long i = 0; i < 1'000'000'000; i++) {
       *    d.add_seconds(1e-9,err);
       * }
       * This version will give better results that using
       * d = TwoPartDate(...);
       * double err = 0;
       * for (long i = 0; i < 1'000'000'000; i++) {
       *    d.add_seconds(1e-9);
       * }
       * For accuracy/precision results, see test/precision/tpadd.cpp
       *
       * @param[in] sec  Floating-point seconds to add to instance
       * @param[out] err Previous summation error at input; updated at output to
       *                 be used at next iteration. If this is the first call,
       *                 set err to 0e0.
       */
      void add_seconds_kahan(FractionalSeconds sec, FDOUBLE &err) noexcept {
        FDOUBLE a = _fsec;
        FDOUBLE b = sec.seconds();
        FDOUBLE y = b - err;
        b = a + y;
        err = (b - a) - y;
        _fsec = b;
        this->normalize();
      }

      /** @brief Difference between two dates as integral number of days and
       * seconds of day.
       *
       * This is not an 'actual date' but rather a datetime interval, but can be
       * represented by a TwoPartDate instance. If the calling instance is prior to
       * the operand (i.e. d1-d2 with d2>d1) the interval is signed as 'negative'.
       * This means that the number of days can be negative, but the fractional
       * day will always be positive
       */
      TwoPartDate operator-(const TwoPartDate &d) const noexcept {
        return TwoPartDate(_mjd - d._mjd, _fsec - d._fsec);
      }

      /** @brief Remove (subtract) whole days. */
      TwoPartDate operator-(const modified_julian_day days) const noexcept {
        return TwoPartDate(_mjd - days.as_underlying_type(), _fsec);
      }

      /** @brief Add integral days. */
      TwoPartDate operator+(const modified_julian_day days) const noexcept {
        return TwoPartDate(_mjd + days.as_underlying_type(), _fsec);
      }

      /** @brief Add two instances.
       *
       * In this case, the second instance can be though of as an interval
       * (rather than an actual datetime instance). The right operand can be a
       * negative interval, which means that we are going backwards in time.
       */
      TwoPartDate operator+(const TwoPartDate &d) const noexcept {
        return TwoPartDate(_mjd + d._mjd, _fsec + d._fsec);
      }

      /** @brief Get the difference between two datetime instances as a floating
       * point value.
       *
       * The difference can be obtained as a fractional days or fractional seconds,
       * depending on the template parameter \p DT.
       * If called as diff(d1,d2), the computation is d1-d2; the difference can be
       * negative if d2>d1.
       *
       * @warning Does not take into account leap seconds.
       */
      template <DateTimeDifferenceType DT>
        typename DateTimeDifferenceTypeTraits<DT>::dif_type
        diff(const TwoPartDate &d) const noexcept {
          if constexpr (DT == DateTimeDifferenceType::FractionalDays) {
            /* difference as fractional days */
            return FractionalDays{(_mjd - d._mjd) + (_fsec - d._fsec) / SEC_PER_DAY};
          } else if constexpr (DT == DateTimeDifferenceType::FractionalSeconds) {
            /* difference as fractional seconds */
            return FractionalSeconds{(_fsec - d._fsec) +
              (_mjd - d._mjd) * SEC_PER_DAY};
          } else {
            /* difference as fractional (julian) years */
            return FractionalYears{
              this->diff<DateTimeDifferenceType::FractionalDays>(d).days() /
                DAYS_IN_JULIAN_YEAR};
          }
        }

      /** @brief Get the date as (fractional) Julian Date. */
      FDOUBLE julian_date() const noexcept {
        return _fsec / SEC_PER_DAY + (_mjd + dso::MJD0_JD);
      }

      /** @brief Transform instance to TT, assuming it is in TAI.
       *
       * The two time scales are connected by the formula:
       * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
       */
      TwoPartDate tai2tt() const noexcept {
        constexpr const FDOUBLE dtat = TT_MINUS_TAI;
        return TwoPartDate(_mjd, _fsec + dtat);
      }

      /** @brief Transform an instance to TAI assuming it is in TT.
       *
       * The two time scales are connected by the formula:
       * \f$ TT = TAI + ΔT \$ where \f$ ΔT = TT - TAI = 32.184 [sec] \f$
       */
      TwoPartDate tt2tai() const noexcept {
        constexpr const FDOUBLE dtat = TT_MINUS_TAI;
        return TwoPartDate(_mjd, _fsec - dtat);
      }

      /** @brief Transform an instance to GPS Time assuming it is in TAI.
       *
       * The two time scales are connected by the formula:
       * \f$ TAI = GPSTime + 19 [sec] \f$
       */
      TwoPartDate tai2gps() const noexcept {
        return TwoPartDate(_mjd, _fsec - TAI_MINUS_GPS);
      }

      /** @brief Transform an instance to GPS Time assuming it is in TT.
      */
      TwoPartDate tt2gps() const noexcept {
        return tt2tai().tai2gps();
      }

      /** @brief Transform an instance to TAI Time assuming it is in GPS Time. */
      TwoPartDate gps2tai() const noexcept {
        return TwoPartDate(_mjd, _fsec + TAI_MINUS_GPS);
      }

      /** @brief Transform an instance to TT Time assuming it is in GPS Time. */
      TwoPartDate gps2tt() const noexcept {
        return gps2tai().tai2tt();
      }

      /** @brief Transform an instance to UTC assuming it is in TAI. */
      TwoPartDateUTC tai2utc() const noexcept {
        FDOUBLE utcsec = _fsec - (double)dat(modified_julian_day(_mjd));
        int utcmjd = _mjd;
        /* let the TwoPartDateUTC constructor normalize the instance */
        return TwoPartDateUTC(utcmjd, FractionalSeconds{utcsec});
      }

      /** @brief Transform an instance to UTC assuming it is in GPS Time. */
      TwoPartDateUTC gps2utc() const noexcept { return gps2tai().tai2utc(); }

      /** @brief Transform an instance to UTC assuming it is in TT */
      TwoPartDateUTC tt2utc() const noexcept {
        const auto tai = this->tt2tai();
        return tai.tai2utc();
      }

      /** @brief TT to UT1 MJD.
       *
       * Note that because the Earth’s rotation is slowing due to tidal friction,
       * and the rotation rate decreases approximately linearly with time, ∆T
       * increases quadratically.
       * @param[in] dut1 ΔUT1 in [sec]. This value should be looked up using e.g.
       *                 IERS products
       * @return The corresponding UT1 MJD, computed using:
       *         ∆T = TT − UT1 = 32.184[sec] + ∆AT − ∆UT1
       */
      TwoPartDate tt2ut1(FDOUBLE dut1) const noexcept {
        /* note that ΔUT1 = UT1 − UTC hence UT1 = ΔUT1 + UTC
         * UT1 = TT - 32.184[sec] - ΔAT + ΔUT1
         *     = TAI - ΔAT + ΔUT1
         */
        //const auto utc = this->tt2utc();
        //TwoPartDate ut1(utc.imjd(), utc.seconds());
        //ut1.add_seconds(dut1);
        //return ut1;
        const auto utc = this->tt2utc();
        return TwoPartDate(utc.imjd(), utc.seconds()).add_seconds(FractionalSeconds(dut1));
      }

      /** @brief TAI to UT1 MJD. */
      TwoPartDate tai2ut1(FDOUBLE dut1) const noexcept {
        /* UT1 = TAI + ΔUT1 - ΔAT
         *     = UTC + ΔUT1
         */
        //const auto utc = this->tai2utc();
        //TwoPartDate ut1(utc.imjd(), utc.seconds());
        //ut1.add_seconds(dut1);
        //return ut1;
        const auto utc = this->tai2utc();
        return TwoPartDate(utc.imjd(), utc.seconds()).add_seconds(FractionalSeconds(dut1));
      }

      /** @brief Return instance as fractional MJD. */
      FDOUBLE as_mjd() const noexcept { return _fsec / SEC_PER_DAY + _mjd; }

      /** @brief Return Julian Centuries since J2000.0 */
      FDOUBLE jcenturies_sinceJ2000() const noexcept {
        return ((static_cast<FDOUBLE>(_mjd) - J2000_MJD) + _fsec / SEC_PER_DAY) /
          DAYS_IN_JULIAN_CENT;
      }

      /** @brief Convert to Julian Epoch, assuming the TT time-scale. */
      FDOUBLE epj() const noexcept {
        return core::mjd2epj((double)imjd(), seconds().seconds() / SEC_PER_DAY);
      }

      /** @brief Overload the '>' operator. */
      bool operator>(const TwoPartDate &d) const noexcept {
        return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec > d._fsec));
      }

      /** @brief Overload the '>=' operator. */
      bool operator>=(const TwoPartDate &d) const noexcept {
        return (_mjd > d._mjd) || ((_mjd == d._mjd) && (_fsec >= d._fsec));
      }

      /** @brief Overload the '<' operator. */
      bool operator<(const TwoPartDate &d) const noexcept {
        return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec < d._fsec));
      }

      /** @brief Overload the '<=' operator. */
      bool operator<=(const TwoPartDate &d) const noexcept {
        return (_mjd < d._mjd) || ((_mjd == d._mjd) && (_fsec <= d._fsec));
      }

      /** @brief Normalize an instance.
       *
       * Normalize here is meant in the sense that the fractional seconds of day
       * (\p _fsec) are always in the range [0,86400), while the integral part
       * (\p _mjd) is always a valid integer.
       * The only case where negative seconds are allowed, is when the MJD part is
       * zero. In this case, the seconds of day are allowed to be negative, so
       * that they can hold the sign.
       */
      void normalize() noexcept {
        if (_fsec >= 0e0 && _fsec < 86400e0)
          return;
        /* remove whole days from seconds and compute signed remainder */
        auto srem = std::fmod(_fsec, seconds::max_in_day);
        int extradays = _fsec / seconds::max_in_day;
        /* only allow negative seconds if whole days are zero */
        if ((srem < 0e0) && (_mjd + extradays)) {
          --extradays;
          srem += seconds::max_in_day;
        }
        _fsec = srem;
        _mjd += extradays;
#ifdef DEBUG
        if (_mjd) {
          assert(_fsec >= 0e0 && _fsec < 86400e0);
        }
        if (_fsec < 0e0) {
          assert(_mjd == 0);
          assert(_fsec > -86400e0);
        } else {
          assert(_fsec >= 0e0 && _fsec < 86400e0);
        }
#endif
        /* all done */
        return;
      }

      /** @brief Overload equality operator. */
      bool operator==(const TwoPartDate &d) const noexcept {
        return (_mjd == d._mjd) && (_fsec == d._fsec);
      }

      /** @brief Overload in-equality operator */
      bool operator!=(const TwoPartDate &d) const noexcept {
        return !(this->operator==(d));
      }

      friend class TwoPartDateUTC;
  }; /* class TwoPartDate */

  /** @brief Julian Epoch to two-part Modified Julian Date (TT).
   *
   * The function assume the TT time-scale.
   *
   * @param[in] epj The Julian Epoch to convert
   * @return The corresponding MJD as a TwoPartDate
   */
  inline TwoPartDate epj2tpd(double epj) noexcept {
    double fday;
    const double mjd = core::epj2mjd(epj, fday);
    return TwoPartDate(mjd, FractionalSeconds{fday * SEC_PER_DAY});
  }

  /** Cast a TwoPartDate instance to an instance of type datetime<T>
   * TODO needs testing!
   */
#if __cplusplus >= 202002L
  template <gconcepts::is_sec_dt T>
#else
    template <typename T, typename = std::enable_if_t<T::is_of_sec_type>>
#endif
    inline datetime<T> from_mjdepoch(const TwoPartDate &t) noexcept {
      dso::picoseconds psec(static_cast<picoseconds::underlying_type>(
            t.seconds().seconds() * picoseconds::sec_factor<double>()));
      return datetime<T>(modified_julian_day(t.imjd()),
          cast_to<picoseconds, T>(psec));
    }

  } /* namespace dso */

#endif
