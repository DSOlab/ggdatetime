# C++ Datetime library

[![Build Status](https://travis-ci.com/xanthospap/ggdatetime.svg?branch=master)](https://travis-ci.com/xanthospap/ggdatetime)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/xanthospap/ggdatetime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/xanthospap/ggdatetime/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/xanthospap/ggdatetime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/xanthospap/ggdatetime/context:cpp)


## Introduction

ggdatetime is a C++ library to assist Geodesy and GNSS-processing related
fields.

## Compilation / Installation

Source code is ISO C++17. Compilation should be trivial using any C++ compiler
[supporting the c++17](https://en.wikipedia.org/wiki/C%2B%2B17#Compiler_support) 
standard (option `-std=c++17` in gcc and clang).

**October 2021** Source code can also be compiled using the latest 
[C++ 20 standard](https://en.cppreference.com/w/cpp/20). To compile against
C++20, set the corresponding flags in the `Makefile.am` files.

> Since December 2021, the build system has been changed from [GNU Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)
> to [scons](https://scons.org/). Users can still build using the Autotools system, but
> we advise changing to scons, as the former will be considered obsolete in the near
> future. 

To compile the library, just follow the basic steps: (*note that the library is still at development phase so users need to configure the project before compiling*)

For the following, `ROOTDIR` will be the root directory of this repository,
aka the directory under which `/src`, `/test` and `/doc` folders live.

### Compile and install
To compile and install, using [scons](https://scons.org/), just type:
```
scons
sudo scons install
```

### Compilation using [Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html) __(deprecated)__

**If you do not need the DEBUG version** (which most probably you don't), create the `Makefile.am` templates. This means that you
should rename [Makefile.am.production](src/Makefile.am.production) and [Makefile.am.production](test/Makefile.am.production) to
`src/Makefile.am` and `test/Makefile.am` respectively, that is:

```bash
mv src/Makefile.am.production src/Makefile.am
mv test/Makefile.am.production test/Makefile.am
```

Then run Autotools and compile:

```bash
autoreconf -if
./configure
make
sudo make install
```

## Verify & Test

After a succesefull installation, users should have:

1. all library header files in `/usr/local/include/datetime/`
2. the library (both a static and shared) in `/usr/local/lib/`

~~To run a validity check, just run: `make check` at the root directory. Hopefully, 
you 'll see all checks passing!~~

**__The following step is optional__**
A validation test script (kind of unit-testing) can be run to check that the library 
works as expected. You do not need to have the library installed on your system to 
perform the check (aka the step `sudo make install` is not needed) you only need to 
have it compiled (aka `make` should be successeful). If you built the library in 
a seperate folder you may need to edit the script, because it assumes that the 
(newly compiled) library is located in `ROOTDIR/src/.libs` witch is the default case.
The validation script can be found is [make_unit_tests.sh](https://github.com/xanthospap/ggdatetime/blob/master/var/make_unit_tests.sh) 
and to run it (assuming you are in `ROOTDIR`):
`cd var && bash make_unit_tests.sh`. The script will use a number of source files 
located in the [ROOT/var](https://github.com/xanthospap/ggdatetime/blob/master/var/) folder 
to check the compile-ibility of the sources and the reported results. At the end 
of the report, you should see the meesage: `Everything appears to have worked as expected!`

Link, include and have fun!

## The Library

Here is a list of the provided utilities:

### How to use the library (TODO)

### Namespaces

The whole of the library is wrapped around the `dso` namespace

### Linking

- static
- dynamic

## Documentation & Library API (TODO)

- build dox with doxygen (or link to dox)

## TODO

## Bugs & Maintanance
Xanthos, xanthos@mail.ntua.gr
Mitsos, danast@mail.ntua.gr


## FAQ
