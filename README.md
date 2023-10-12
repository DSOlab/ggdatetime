# C++ Datetime library

## Introduction

ggdatetime is a C++ library to assist date and time related computations in 
Geodesy related fields.

## Compilation / Installation

Source code is ISO C++17. Compilation should be trivial using any C++ compiler
[supporting the c++17](https://en.wikipedia.org/wiki/C%2B%2B17#Compiler_support) 
standard (option `-std=c++17` in gcc and clang).

**October 2021** Source code can also be compiled using the latest 
[C++ 20 standard](https://en.cppreference.com/w/cpp/20). 

> Since December 2021, the build system has been changed from 
> [GNU Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)
> to [scons](https://scons.org/). 

### Compile and install

To compile and install, using [scons](https://scons.org/), just type:
```
# compile
scons
# install (system-wide)
sudo scons install
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

### Testing

Test source code is found in [test folder](test). 
To automatically run all tests, use the Python script 
[run_test_suite.py](run_test_suite.py). 
Note that to be able to run the scipt, you will have to compile using the 
`test=1` option, i.e.
```
## build project and tests
scons test=1
## run tests
./run_test_suite.py --progs-dir=test
```
