# Testing Failed Build

In this folder, we place source code that should not compile. Trying to build 
these files, should result in a **compile-time** error.

This means, that "testing" in this case, means **"verify that the source code 
does not get built, and the compiler spits an error message that makes sense"**.

The tests here are tested with `ctest` (once built) and if they pass, this 
means that indeed the compilation failed, triggering an error which we expected. 
Expected errors are given as regex expression for each source code file, e.g. 
`"no match for ‘operator==’"`.

See the CMakeLists file for more info.
