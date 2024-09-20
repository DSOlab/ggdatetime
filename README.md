# C++ Datetime library

## Introduction

ggdatetime is a C++ library to assist date and time related computations in 
Geodesy related fields.

[![clang-format Check](https://github.com/DSOlab/ggdatetime/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/DSOlab/ggdatetime/actions/workflows/clang-format-check.yml)
[![Linux CI build](https://github.com/DSOlab/ggdatetime/actions/workflows/cpp-linux-build.yml/badge.svg)](https://github.com/DSOlab/ggdatetime/actions/workflows/cpp-linux-build.yml)

## Compilation / Installation

Source code is ISO C++17. Compilation should be trivial using any C++ compiler
[supporting the c++17](https://en.wikipedia.org/wiki/C%2B%2B17#Compiler_support) 
standard (option `-std=c++17` in gcc and clang).

**October 2021** Source code can also be compiled using the latest 
[C++ 20 standard](https://en.cppreference.com/w/cpp/20). 

> Since December 2021, the build system has been changed from 
> [GNU Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)
> to [scons](https://scons.org/). 
> Since September 2024 the build system has been changed to [cmake](https://cmake.org/)

### Compile and install

Supposing you are located in the top-level directory:

```
## to build in a folder named "build":
$> cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$> cmake --build build --target all --config Release -- -j4
## (Optional) run tests
$> ctest --test-dir build
## Install, system-wide (needs root)
$> cd build && sudo make install
```

## Examples

Consult the test programs source code in the 
[test folder](https://github.com/xanthospap/libsinex/tree/main/test) 
for examples on how to use the library.

## Developers

Take a look at the [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html) 
and if possible stick to it. 

Use [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
to format you code before commiting; if you try to commit with non-acceptable 
code format, the CI system will fail.

## The Library

### Nomenclature

* *Julian year* 365.25 days
* *Julian century* 36525 days

*Julian date* (JD) is a continuous count of days (JD zero is located about 7000 years ago).
Because of the unwieldy size of Julian Dates and the awkwardness of the half-day 
offset, it is accepted practice to remove the leading ‘24’ and the trailing ‘.5’, 
producing what is called the *Modified Julian Date*, i.e.
$ MJD = JD − 2,400,000.5 $

In the library, this constant is named `ds::MJD0_JD = 2400000.5e0`, i.e. the
*Julian Date of Modified Julian Date zero*.

*Julian epoch* is a convinient way of representing an epoch as fractional years. 
It uses the *Julian year* of exactly 365.25 days, and the *TT* time scale; 
Julian epoch 2000.0 is defined to be 2000 January 1.5 (i.e. 2000 January 1d 12h), 
which is JD 2,451,545.0 (i.e. `dso::J2000_JD`) or MJD 51,544.5 (i.e. `dso::J2000_MJD`). 
Julian epoch is usually denoted as *“J2000.0”*.

### Precision

**Transforming from `datetime<T>` to `TwoPartDate`s**

Transforming from a `datetime<nanoseconds>` instance to an instance of type 
`TwoPartDate` preserves a precision better than 2e-2 nanoseconds. This is verified 
by running the test program [unit_tests/tpdates1.cpp](test/unit_tests/tpdates1.cpp)

Transforming from a `datetime<microseconds>` instance to an instance of type 
`TwoPartDate` preserves a precision better than 2e-5 microseconds. This is verified 
by running the test program [unit_tests/tpdates2.cpp](test/unit_tests/tpdates2.cpp)

Transforming from a `datetime<milliseconds>` instance to an instance of type 
`TwoPartDate` preserves a precision better than 2e-8 milliseconds. This is verified 
by running the test program [unit_tests/tpdates3.cpp](test/unit_tests/tpdates3.cpp)

Transforming from a `datetime<seconds>` instance to an instance of type 
`TwoPartDate` preserves a precision better than 2e-12 seconds. This is verified 
by running the test program [unit_tests/tpdates4.cpp](test/unit_tests/tpdates4.cpp)

**Transforming from/to Julian Epoch**

Transforming from a `TwoPartDate` to a *Julian Epoch* and back, preserves an 
accuracy of `~1e-5` seconds. This is verified by running the test program 
[test/sofa/epj_date.cpp](test/sofa/epj_date.cpp).

## Authors

Dionysos Satellite Observatory,
School of Rural Surveying and Geoinformatics Engineering,
National Technical University of Athens

Send bugs to:
Xanthos Papanikolaou, xanthos@mail.ntua.gr
