#include "calendar.hpp"
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
      "ymd2mjd: Transform a date given as  \"YYYYdMMdDD\" to Modified Julian "
      "Day. The\ncharacter \"d\" in the date string can be any character you "
      "want,\nexcept from a numeric value. The program expects the read a date "
      "string\nfrom STDIN (or multuple date strings, seperated by newlines) "
      "and "
      "will\nprint results on STDOUT.\nThe date string can be followed by any "
      "numberof remaining characters\nthat will be ignored.\n\nExample "
      "Usage:\n$>cat "
      "dates\n2014:01:09\n2014:01:9\n2014:1:09\n2014:01:08\n2014:01:07\n2014:"
      "01:0 // ERROR\n2014:01:1\n2014T01:1\n2014TT01:1 //ERROR\n2014:01:1with "
      "some string\n2014/01/1with some string\n$>cat dates | "
      "ymd2mjd\n56666\n56666\n56666\n56665\n56664\nERROR. Failed "
      "parsing/transforming line: 2014:01:0\n\n56658\n56658\nERROR. Failed "
      "parsing/transforming line: 2014TT01:1\n\n56658\n56658\n\nOptions:\n[-h] "
      "help message\n\tprint (this) message and exit.\n[-e] "
      "MAX_ERRORS_ALLOWED\n\tMaximum number of errors allowed (i.e. date "
      "strings that where not\n\tparsed correctly). Default values is %d\n\n"
      "\n\nWarnings:\n\t* Command line options are only available on POSIX "
      "systems.\n\nDionysos Satellite "
      "Observatory\nNational Technical University of "
      "Athens\nhttps://github.com/DSOlab/ggdatetime\n",
      MAX_ERRORS_ALLOWED);
  return;
}

int main(int argc, char *argv[]) {
  char line[124];
  int yr, mn, dm;
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
    if (5 == sscanf(line, "%d%c%d%c%d", &yr, &c, &mn, &c, &dm)) {
      try {
      printf("%d\n", dso::modified_julian_day(dso::year(yr), dso::month(mn),
                                              dso::day_of_month(dm))
                         .as_underlying_type());
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
