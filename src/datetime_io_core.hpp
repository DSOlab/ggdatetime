/** @file
 * Datetime input/output (i.e. read/write) core utilities and definitions.
 */

#ifndef __DSO_DATETIME_IO_CORE_HPP__
#define __DSO_DATETIME_IO_CORE_HPP__

namespace dso {
enum class YMDFormat { YYYYMMDD, DDMMYYYY, YYYYDDD, YYMMDD, DDMMYY };
enum class HMSFormat { HHMMSS, HHMMSSF };

namespace datetime_io_core {
int get_two_ints(const char *str, int *ints, int max_chars,
                 const char **end) noexcept;

int get_three_ints(const char *str, int *ints, int max_chars,
                   const char **end) noexcept;

int get_three_ints(const char *str, long *ints, int max_chars,
                   const char **end) noexcept;

int get_two_ints_double(const char *str, int *ints, double &flt, int max_chars,
                        const char **end) noexcept;
} /* namespace datetime_io_core */

} /* namespace dso */

#endif
