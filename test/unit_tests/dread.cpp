#include "datetime_read.hpp"
#include "datetime_write.hpp"
#include <cstdio>

using namespace dso;

int main() {
  const char *test = "04/11/2023 17:53:14 Abcdefg";
  const char *end;
  char buffer[64] = {'\0'};

  datetime<nanoseconds> d1 =
      from_char<YMDFormat::DDMMYYYY, HMSFormat::HHMMSS, nanoseconds>(test,
                                                                     &end);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  printf("Given date: %s\n", buffer);
  printf("Rest of string was: [%s]\n", end);

  const char *test2 = "04/11/2023 17:53:14.123456789 Abcdefg";
  d1 = from_char<YMDFormat::DDMMYYYY, HMSFormat::HHMMSS, nanoseconds>(test2,
                                                                      &end);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSS>(d1, buffer);
  printf("Given date: %s\n", buffer);
  printf("Rest of string was: [%s]\n", end);

  d1 = from_char<YMDFormat::DDMMYYYY, HMSFormat::HHMMSSF, nanoseconds>(test2,
                                                                       &end);
  to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d1, buffer);
  printf("Given date: %s\n", buffer);
  printf("Rest of string was: [%s]\n", end);
  
  return 0;
}
