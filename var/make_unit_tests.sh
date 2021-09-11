#!/usr/bin/env sh

echo "+=====================================================================+"
echo "        Kind-a automated, bash/gcc unit testing for ggdatetime         "
echo "+=====================================================================+"

## We need the freshly-compiled library NOT the system one! Locate the library
## and update LD_LIBRARY_PATH for the linker
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOTDIR=${DIR%\/var}
LNPTH=${ROOTDIR}/src/.libs
if ! test -f ${LNPTH}/libggdatetime.la && ! test -f ${LNPTH}/libggdatetime.so ; then
  echo echo "FAILED; stoping with error" 1>&2
  exit 3
fi
export LD_LIBRARY_PATH=${LNPTH}:$LD_LIBRARY_PATH

## Unit tests, compilable source files
unit_tests=( test_year.cpp test_month.cpp test_gps_week.cpp test_day_of_month.cpp test_modified_julian_day.cpp test_day_of_year.cpp test_seconds.cpp test_hmfs.cpp )
echo " Intial source files:"
for fl in "${unit_tests[@]}" ; do echo "    * $fl" ; done

## Unit tests, non-compilable source files
declare -a errornuous_units_tests

##  For every source file, create (a) new one(s) in which we uncomment the 
##+ lines that contain CMP_ERROR, one line per file. E.g. if the file
##+ foo.cpp contains 5 lines that match "CMP_ERROR" we are going to create
##+ 5 different files; the file foo_error1.cpp will be the same as foo.cpp 
##+ except that the first line that matches "CMP_ERROR" will be uncommented.
##+ foo_error2.cpp will be the same as foo.cpp except that the second line 
##+ that matches "CMP_ERROR" will be uncommented (the first will be commented),
##+ and the same for foo_error3.cpp, foo_error4.cpp, and foo_error5.cpp
##
##  We will replace any line of type:
##+ "//[....]CMP_ERROR" with the part in braces, aka "[....]"
echo " Preparing erronuous (non-compilable) source files"
echo "+---------------------------------------------------------------------+"
for sc in "${unit_tests[@]}" ; do
  ## find have many lines we have, that contain CMP_ERROR
  OCC=$(cat $sc | grep '\(//\)\(.*;\)\( *\)\(CMP_ERROR\)' | wc -l)
  if test "${OCC}" -gt 1 ; then
    for OCC_NR in $(seq 1 $OCC) ; do
      error_file=${sc/.cpp/_error${OCC_NR}.cpp}
      tr '\n' '^' < $sc \
      | sed "s:\(//\)\( *[^;]*\)\(; *CMP_ERROR\):\2;:${OCC_NR}" \
      | tr '^' '\n' \
      > ${error_file}
      echo "    * $sc -> ${OCC_NR}/${OCC} created file: $error_file"
      errornuous_units_tests+=(${error_file})
    done
  elif test "${OCC}" -eq 1 ; then
    error_file=${sc/.cpp/_error.cpp}
    cat $sc | sed 's:\(//\)\(.*;\)\( *\)\(CMP_ERROR\):\2:g' > ${error_file}
    echo "    * $sc -> 1/1 created file: $error_file"
    errornuous_units_tests+=(${error_file})
  else
    echo "    * $sc -> No file(s) to create"
  fi
done

echo " Compilable unit tests    :"
echo "+---------------------------------------------------------------------+"
for nc in "${unit_tests[@]}" ; do echo "    * $nc" ; done
echo " Non-Compilable unit tests: "
echo "+---------------------------------------------------------------------+"
for nc in "${errornuous_units_tests[@]}" ; do echo "    * $nc" ; done
>comp.log
echo " Ready to start compiling; all output directed to comp.log"
echo "+---------------------------------------------------------------------+"

## Compile all compilable source code
echo " Compiling (correct) unit tests ..."
echo "+---------------------------------------------------------------------+"
for nc in "${unit_tests[@]}" ; do
  echo "    g++ -std=c++20 -Wall -I${ROOTDIR}/src -L${LNPTH} ${nc} -o ${nc/.cpp/.o} -lggdatetime"
  if ! g++ -std=c++17 -Wall -I${ROOTDIR}/src -L${LNPTH} ${nc} -o ${nc/.cpp/.o} \
      -lggdatetime 2>>comp.log; then
    echo "FAILED; stoping with error" 1>&2
    exit 1
  fi
done

## If any of the non-compilable source codes compiles, trigger an error and stop
echo " Compiling erronuous unit tests ... (they should fail)"
echo "+---------------------------------------------------------------------+"
for nc in "${errornuous_units_tests[@]}" ; do
  echo -n "    g++ -std=c++20 -Wall -I${ROOTDIR}/src -L${LNPTH} ${nc} -o ${nc/.cpp/.o} -lggdatetime ..."
  if g++ -std=c++17 -Wall -I${ROOTDIR}/src -L${LNPTH} ${nc} -o ${nc/.cpp/.o} \
      -lggdatetime 2>>comp.log ; then
    echo "FAILED; stoping with error" 1>&2
    exit 2
  else
    echo -e "failed"
  fi
done
echo ""

##  Now we need to run the compiled programs and check that they don;t
##+ results in assertion error.
echo "Running compiled programs (hopefully they don't throw/assert) ...."
echo "+---------------------------------------------------------------------+"
for nc in "${unit_tests[@]}" ; do
  prg=${nc/.cpp/.o}
  if ! ./${prg}  ; then
    echo "FAILED; stoping with error" 1>&2
    exit 1
  fi
done

echo " Everything appears to have worked as expected!"
echo "+=====================================================================+"
exit 0
