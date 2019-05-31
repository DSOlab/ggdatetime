#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "sofa.h"
#include "sofam.h"

double 
julian_date(short int year, short int month, short int day)
{
   long int jd12h;
   double tjd;

   jd12h = (long) day - 32075L + 1461L * ((long) year + 4800L
      + ((long) month - 14L) / 12L) / 4L
      + 367L * ((long) month - 2L - ((long) month - 14L) / 12L * 12L)
      / 12L - 3L * (((long) year + 4900L + ((long) month - 14L) / 12L)
      / 100L) / 4L;
   tjd = (double) jd12h - 0.5; // + hour / 24.0;

   return (tjd);
}

int iauJd2cal2(double dj1, double dj2,
              int *iy, int *im, int *id, double *fd)
{
/* Minimum and maximum allowed JD */
   const double DJMIN = -68569.5;
   const double DJMAX = 1e9;

   long jd, l, n, i, k;
   double dj, d1, d2, f1, f2, f, d;


/* Verify date is acceptable. */
   dj = dj1 + dj2;
   if (dj < DJMIN || dj > DJMAX) return -1;

/* Copy the date, big then small, and re-align to midnight. */
   if (dj1 >= dj2) {
      d1 = dj1;
      d2 = dj2;
   } else {
      d1 = dj2;
      d2 = dj1;
   }
   d2 -= 0.5;

/* Separate day and fraction. */
   f1 = fmod(d1, 1.0);
   f2 = fmod(d2, 1.0);
   f = fmod(f1 + f2, 1.0);
   if (f < 0.0) f += 1.0;
   d = dnint(d1-f1) + dnint(d2-f2) + dnint(f1+f2-f);
   jd = (long) dnint(d) + 1L;
  /*printf("+++++++iauCal2jd input jd=%15.3ff l=%12ld\n", dj1+dj2, jd);*/

/* Express day in Gregorian calendar. */
   l = jd + 68569L;
  /*printf("+++++++iauCal2jd jd=%12ld l=%12ld\n", jd, l);*/
   n = (4L * l) / 146097L;
   l -= (146097L * n + 3L) / 4L;
   i = (4000L * (l + 1L)) / 1461001L;
   l -= (1461L * i) / 4L - 31L;
   k = (80L * l) / 2447L;
   *id = (int) (l - (2447L * k) / 80L);
   l = k / 11L;
   *im = (int) (k + 2L - 12L * l);
   *iy = (int) (100L * (n - 49L) + i + l);
   *fd = f;

   return 0;
}


int jd2cal(int mjd, int *iy, int *im, int *id)
{
  long l, n, i, k;

  /* Express day in Gregorian calendar. */
  l = mjd + (68569L + 2400000L + 1);
  /*printf("+++++++iauCal2jd mjd=%12d l=%12ld\n", mjd, l);*/
  n = (4L * l) / 146097L;
  l -= (146097L * n + 3L) / 4L;
  i = (4000L * (l + 1L)) / 1461001L;
  l -= (1461L * i) / 4L - 31L;
  k = (80L * l) / 2447L;
  *id = (int) (l - (2447L * k) / 80L);
  l = k / 11L;
  *im = (int) (k + 2L - 12L * l);
  *iy = (int) (100L * (n - 49L) + i + l);

  return 0;
}

int main()
{

  int status;
  double djm0, djm, jn, fd;
  int iy, im, id;
  int iy2, im2, id2;
  int DJM_;

  status = iauCal2jd(-4713, 1, 1, &djm0, &djm);
  jn = julian_date(-4713, 1, 1);
  printf("-4713/01/01 -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(-4712, 1, 1, &djm0, &djm);
  jn = julian_date(-4712, 1, 1);
  printf("-4712/01/01 -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(-4711, 1, 1, &djm0, &djm);
  jn = julian_date(-4711, 1, 1);
  printf("-4711/01/01  -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(-1, 1, 1, &djm0, &djm);
  jn = julian_date(-1, 1, 1);
  printf("-0001/01/01 -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(3, 1, 1, &djm0, &djm);
  jn = julian_date(1, 1, 1);
  printf("0000/01/01  -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(1, 1, 1, &djm0, &djm);
  jn = julian_date(1, 1, 1);
  printf("0001/01/01  -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);
  
  status = iauCal2jd(-4712, 1, 1, &djm0, &djm);
  jn = julian_date(-4713, 11, 24);
  printf("-4712/01/01 -> %12.3f or %12.3f\n", djm0 + djm, jn);
  status = iauJd2cal2(djm0, djm, &iy, &im, &id, &fd);
  status = jd2cal((int)djm, &iy2, &im2, &id2);
  printf("\t%7d/%7d/%7d\n", iy2, im2, id2);
  printf("\t%7d/%7d/%7d\n", iy, im, id);

  srand(time(NULL));
  for (int i=0; i<1000; i++) {
    DJM_ = rand();
    status = iauJd2cal2(djm0, DJM, &iy, &im, &id, &fd);
    status = jd2cal((int)DJM, &iy2, &im2, &id2);
    if (iy!=iy2 || (im!=im2 || id!=id2)) {
      printf("!!!!!!!!!!!! ERROR !!!!!!!!!!!!!!!!!!!!\n");
    }
  }

  return 0;
}
