# ggdatetime C++ library                                           {#mainpage}

ggdatetime is a C++ library to assist Geodesy and GNSS-processing related
fields.

## Prerequisites

* C++ standards:
tested under C++14 and C++17

* Compilers
tested under g++

## Installation

For the following, `ROOTDIR` will be the root directory of this repository,
aka the directory under which `/src`, `/test` and `/doc` folders live.

### Production Version

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

After a successeful installation, you should be able to run the test program
`ROOTDIR/test/testDatetime`

## Classes

| Class                  | Header File     | Description                       | Link to Doc               |
| :--------------------- | :-------------- | :-------------------------------- | :------------------------ |
|year                    | dtfund.hpp      | any int                           | ngpt::year                |
|month                   | dtfund.hpp      | any int; valid months are [1,12]  | ngpt::month               |
|day_of_month            | dtfund.hpp      | any int                           | ngpt::day_of_month        |
|day_of_year             | dtfund.hpp      | any int                           | ngpt::day_of_year         |
|modified_julian_day     | dtfund.hpp      | any int; extended operations      | ngpt::modified_julian_day |
|hours                   | dtfund.hpp      | any int                           | ngpt::hours               |
|minutes                 | dtfund.hpp      | any int                           | ngpt::minutes             |
|seconds                 | dtfund.hpp      | any long int; extended operations | ngpt::seconds             |
|milliseconds            | dtfund.hpp      | any long int; extended operations | ngpt::milliseconds        |
|microseconds            | dtfund.hpp      | any long int; extended operations | ngpt::microseconds        |
|datetime_interval<s>    | dtcalendar.hpp  | holds a datetime interval/period  | ngpt::datetime_interval   |
