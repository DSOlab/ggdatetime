#include "date_integral_types.hpp"
#include <cstring>
#include <stdexcept>

const char *dso::month::short_name() const {
  if (!this->is_valid()) {
    fprintf(stderr,
            "[ERROR] Invalid month; cannot translate to str (traceback: %s)\n",
            __func__);
    throw std::runtime_error(
        "[ERROR] Invalid month; cannot translate to str\n");
  }
  return short_names[m_month - 1];
}
const char *dso::month::long_name() const {
  if (!this->is_valid()) {
    fprintf(stderr,
            "[ERROR] Invalid month; cannot translate to str (traceback: %s)\n",
            __func__);
    throw std::runtime_error(
        "[ERROR] Invalid month; cannot translate to str\n");
  }
  return long_names[m_month - 1];
}
