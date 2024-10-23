#include "calendar.hpp"
#include <cstdio>
#include <cstring>
#include <exception>
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

constexpr const int MAX_ERRORS_ALLOWED = 10;

/* help message */
void prhelp() {
  printf(
      "ydoy2mjd: Transform a date from Year, Day-Of-Year to Modified Julian "
      "Day.\n"
      "The program expects the read a date compliant to the format "
      "\n\"YYYYdDDD\" "
      "where \"d\" is any non-numeric character from STDIN (or multiple "
      "\ndates, "
      "seperated by newlines) and will print results on STDOUT. \nThe date "
      "string"
      " can be followed by any number of remaining characters that \nwill be "
      "ignored.\n\nOptions:\n[-h] "
      "help message\n\tprint (this) message and exit.\n[-e] "
      "MAX_ERRORS_ALLOWED\n\tMaximum number of errors allowed (i.e. date "
      "strings that where not\n\tparsed correctly). Default values is %d\n\n"
      "\n\nWarnings:\n\t* Command line options are only available on POSIX "
      "systems.\n\nDionysos Satellite Observatory\nNational Technical "
      "University of "
      "Athens\nhttps://github.com/DSOlab/ggdatetime\n",
      MAX_ERRORS_ALLOWED);
  return;
}

int main(int argc, char *argv[]) {
  char line[124];
  int yr, dy;
  char c;
  int error = 0, cer = 0;
  int max_errors_allowed = MAX_ERRORS_ALLOWED;

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  /* parse command line arguments */
  while ((c = getopt(argc, argv, "he:")) != -1) {
    switch (c) {
    case 'h':
      prhelp();
      return 0;
    case 'e':
      max_errors_allowed = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s [-e MAX_ERRORS_ALLOWED]\n", argv[0]);
      return 1;
    } /* switch c */
  }
#endif

  while (fgets(line, sizeof(line), stdin) && (error < max_errors_allowed)) {
    if (3 == sscanf(line, "%d%c%d", &yr, &c, &dy)) {
      try {
        const dso::modified_julian_day m{dso::year(yr), dso::day_of_year(dy)};
        printf("%d\n", m.as_underlying_type());
      } catch (std::exception &) {
        ++error;
      }
    } else {
      ++error;
    }

    if (error > cer) {
      fprintf(stderr, "ERROR. Failed parsing/transforming line: %s\n", line);
      ++cer;
    }
  }

  if (error >= max_errors_allowed) {
    fprintf(stderr, "Too many errors, giving up!\n");
    return 1;
  }

  return 0;
}
