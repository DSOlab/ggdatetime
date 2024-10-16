#include "calendar.hpp"
#include "datetime_write.hpp"
#include <cstdio>
#include <exception>
#include <cstring>
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#endif

constexpr const int MAX_ERRORS_ALLOWED = 10;

/* help message */
void prhelp() {
  printf(
      "jd2ymd: Transform a date from Modified Julian Day to calendar date, "
      "i.e.\n\"YYYYdMMdDD\". The program expects the read a Modified Julian "
      "Day string\n(actually an integral value) from STDIN (or multiple MJDs, "
      "seperated by\nnewlines) and will print results on STDOUT. The MJD string" 
      " can be followed by\nany number of remaining characters that will be "
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
  char buf[64];
  int mjd;
  char c;
  int error = 0, cer = 0;
  int max_errors_allowed = MAX_ERRORS_ALLOWED;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
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

  while (fgets(line, sizeof(line), stdin) && (error<max_errors_allowed)) {
    if (1 == sscanf(line, "%d", &mjd)) {
      dso::modified_julian_day m(mjd);
      const auto ymd = m.to_ymd();
      try {
        if (dso::SpitDate<dso::YMDFormat::YYYYMMDD>::spit(ymd, buf) !=
            dso::SpitDate<dso::YMDFormat::YYYYMMDD>::numChars) {
          ++error;
        } else {
          printf("%s\n", buf);
        }
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

  if (error>=max_errors_allowed) {
    fprintf(stderr, "Too many errors, giving up!\n");
    return 1;
  }

  return 0;
}
