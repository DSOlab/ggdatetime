#include "dtfund.hpp"
#include <cstring>
#ifdef _WIN32
#include <string.h>
#define strcasecmp _stricmp
#else /* assuming POSIX or BSD compliant system */
#include <strings.h>
#endif

dso::month::month(const char *str) : m_month(0) {
  if (std::strlen(str) == 3) {
    for (int i = 0; i < SHORT_NAMES_LEN; i++) {
      if (!strcasecmp(str, short_names[i])) {
        m_month = i + 1;
        break;
      }
    }
  } else if (std::strlen(str) > 3) {
    for (int i = 0; i < LONG_NAMES_LEN; ++i) {
      if (!strcasecmp(str, long_names[i])) {
        m_month = i + 1;
        break;
      }
    }
  }

  if (!m_month) {
    throw std::invalid_argument("Failed to set month from string \"" +
                                std::string(str) + "\"");
  }
}
