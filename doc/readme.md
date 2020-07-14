# ggdatetime C++ library                                           {#mainpage}

## Implementation Notes

### Fundamental Types

The following classes are defined as datetime "fundamental" classes:

```cpp
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

A simple usage example follows:

```cpp
  ngpt::year y(2019), y1(2018), y2(2020), y3(2019);
  assert( y == y3 );
  assert( y >= y1 );
  assert( y <= y2 );
  assert( y != y1 );
  assert( (y += 1) == y2 );
  assert( (y -= 2) == y1 );
  assert( (y += ngpt::year{2}) == y2 );
  assert( (y -= ngpt::year{2}) == y1 );
  assert( ++y == ngpt::year{2019} );
  y++;
  assert( y == y2 );
  y--;
  assert( y == y3 );
  assert( --y == y1 );
  assert( (y = 2020) == y2 );
```

For a detailed example, see the file `test/test_operators`. To make sure that
these operators only work for the types they are supposed to, use the script
`script/test_source.sh`; this performs a kina unit-testing, using erronuous
source code.

### Integer Division

In general, use '/' and '%' operators (not e.g. a loop or std::div). Normally,
if the code is optimized it will make no difference, but (at least for my computer,
x86_64 GNU/Linux) out of the three implementations:

```cpp
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


## Time Scales

* TAI (International Atomic Time): the official timekeeping standard.
* UTC (Coordinated Universal Time): the basis of civil time.
* UT1 (Universal Time): based on Earth rotation.
* TT (Terrestrial Time): used for solar system ephemeris look-up. 2
* TCG (Geocentric Coordinate Time): used for calculations centered on the Earth in space.
* TCB (Barycentric Coordinate Time): used for calculations beyond Earth orbit.
* TDB (Barycentric Dynamical Time): a scaled form of TCB that keeps in step with TT
  on the average.

### ΔUT1 = UT1 - UTC

To obtain UT1 starting from UTC, it is necessary to look up the value of ∆UT1 = UT1−UTC for
the date concerned in tables published by the International Earth Rotation and Reference Sys-
tems Service (IERS); this is then added to the UTC. The quantity UT1−UTC, which typically
changes by 1-2 ms per day, can be obtained only by observation, principally very long baseline
interferometry (VLBI) using extragalactic radio sources, though seasonal effects are present and
the IERS listings are able to predict some way into the future with adequate accuracy for most
applications.
The value of \Delta UT1 can be obtained from 
[Bulletin D](https://datacenter.iers.org/productMetadata.php?id=17) 
published by IERS

### ΔAT = TAI - UTC

### ΔT = TT - UT1

The difference between UT1 and TT (formerly ET) is called ∆T, and in the present era can be
written out as
∆T = TT − UT1 = 32.184(sec) + ∆AT - ∆UT1
∆T is important for interpreting historical observations of solar system phenomena, where mod-
ern ephemerides provide the time of the event accurately but its appearance at a specified
geographical location depends on knowing the Earth orientation.
