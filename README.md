# ggdatetime C++ library
==============================================================================

ggdatetime is a C++ library to assist Geodesy and GNSS-processing related
fields.

## Prerequisites
==============================================================================

* C++ standards:
tested under C++14 and C++17

* Compilers
tested under g++

## Installation
==============================================================================

For the following, `ROOTDIR` will be the root directory of this repository,
aka the directory under which `/src`, `/test` and `/doc` folders live.

### Production Version
----------------------
To build the "production" version, follow the "standard procedure", aka
(in the `ROOT_DIR`) type the following commands:
```
$> ./configure ## (run configure placed in ROOTDIR)
$> make all
$> sudo make install
```

The library will be placed under `/usr/local/lib`, as
- libggdatetime.a (dynamic version)
- libggdatetime.so (static version)

Note that you have to update/add the above path to be able to link against the
library. This can be done in a number of ways, e.g. have your system
administrator add `/usr/local/lib` to `/etc/ld.so.conf`

### Debug Version
-----------------
To build the "debug" version, you will need to re-make the `Makefile.in` files
(in `ROOTDIR/src` and `ROOTDIR/test`). To do this, you will need to use the
`Makefile.am.debug` files, under `ROOTDIR/src` and `ROOTDIR/test`.
So, rename these files to `Makefile.am`
```
$> mv ROOTDIR/src/Makefile.am.debug ROOTDIR/src/Makefile.am
$> mv ROOTDIR/test/Makefile.am.debug ROOTDIR/test/Makefile.am
```

Run `autoreconf --install` to produce the respective, new, `Makefile.in` files.
(Note that autoreconf is part of Autotools package; you may have to install this
package)
`$> autoreconf -i`

Now follow the the "standard procedure", aka
(in the `ROOT_DIR`) type the following commands:
```
$> ./configure ## (run configure placed in ROOTDIR)
$> make all
$> sudo make install
```

Note that you only need the "debug" version if you want to mess with the code.

### Verification
----------------
After a successeful installation, you should be able to run the test program
`ROOTDIR/test/testDatetime`

## Implementation Notes
==============================================================================

### Fundamental Types
---------------------
The following classes are defined as datetime "fundamental" classes:
```
class  year;
class  month;
class  gps_week;
class  day_of_month;
class  day_of_year;
class  modified_julian_day;
class  hours;
class  minutes;
class  seconds;
class  milliseconds;
class  microseconds;
```
Implementation-wise, this means that they only have one member variable, which
is of integral type (actually either an `int` or a `long`).
For these fundamental types, a kinda reflection is used and the following 
operators are defined:
  * `bool`ean operators between instances of the same type: '==', '!=', '<', '<=', '>', '>='
     E.g. `year y1(2012), y2(2013); y1 == y2;`
  * increase/decrease where the right-hand-side is any integral type:
    E.g. `year y1(2012); y1 += 15;// now year is 2027`
  * increase/decrease where the right-hand-side is of the same type as the
    calling instance; E.g. `year y1(2012), y2(10); y1 += y2;// now year1 is 2022`
  * increment/decrement pre- & post-fix operators, aka '++' ans '--' 
    E.g. `year y1(2012); y1++;// now year is 2013 --y1;// now year is 2012`
  * assignment operator ('=') from any integral type:
    E.g. `year y1(2012); y1 = 2015;`

### Integer Division
--------------------
In general, use '/' and '%' operators (not e.g. a loop or std::div). Normally,
if the code is optimized it will make no difference, but (at least for my computer,
x86_64 GNU/Linux) out of the three implementations:
```
class A {
public:
  static constexpr long maxi = 3600L;
  int  i;
  long l;

  void normalize1() noexcept
  {
    while (l>=maxi) {
      ++i;
      l-=maxi;
    }
    while (l<=-maxi) {
      --i;
      l+=maxi;
    }
  }
  void normalize2() noexcept
  {
    std::ldiv_t dv {std::div(l, maxi)};
    i+=dv.quot;
    l=dv.rem;
  }
  void normalize3() noexcept
  {
    i+= l / maxi;
    l = l % maxi;
  }
  ...
  ...
};
```
the third one is the fastest (if no optimizations are allowed).
