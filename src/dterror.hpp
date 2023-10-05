#ifndef __ERROR_BASE_STATUS_ERROR_HPP__
#define __ERROR_BASE_STATUS_ERROR_HPP__

namespace dso {

/* @brief Error status to be returned in various functions
 * In general, anything other than 0 denotes some kind of status.
 * Such an instance should be returned when a function is declared as
 * 'nodiscard'.
 * See https://en.cppreference.com/w/cpp/language/attributes/nodiscard
 * To be used with C++ >= 17
 */
class [[nodiscard]] iStatus {

private:
  /* status flag */
  int status;

public:
  /* @brief Constructor */
  constexpr iStatus(int i) noexcept : status(i){};
  /* @brief Cast to an integers value */
  explicit constexpr operator int() const noexcept { return status; }
  /* @brief Check if return status is ok (aka == 0) */
  static constexpr iStatus ok() noexcept { return iStatus(0); }
  /* @brief equality operator */
  constexpr bool operator==(const iStatus &other) const noexcept {
    return status == other.status;
  }
  /* @brief Inequality operator */
  constexpr bool operator!=(const iStatus &other) const noexcept {
    return !(*this == other);
  }
};

} // namespace dso

#endif
