#! /usr/bin/bash

## Unit tests, compilable source files
unit_tests=( test_year.cpp )

## Unit tests, non-compilable source files
declare -a errornuous_units_tests

##  For every source file, create a new one in which we uncomment the lines
##+ that contain CMP_ERROR
## We will replace any line of type:
## "//[....]CMP_ERROR" with the part in braces, aka "[....]"
for sc in "${unit_tests[@]}" ; do
  error_file=${sc/.cpp/_error.cpp}
  cat $sc | sed 's:\(//\)\(.*;\)\( *\)\(CMP_ERROR\):\2:g' > ${error_file}
  errornuous_units_tests+=(${error_file})
done

echo "Compilable unit tests    : ${unit_tests[@]}"
echo "Non-Compilable unit tests: ${errornuous_units_tests[@]}"

## If any of the non-compilable source codes compiles, trigger an error and stop
CFL="-std=c++17 -Wall -I../src -L../src -lggdatetime"
for nc in "${errornuous_units_tests[@]}" ; do
  g++ "${CFL}" $nc
done
