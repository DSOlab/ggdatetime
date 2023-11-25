# Two-Part Date Precision Tests

## Introduction

Precision tests contains C++ scripts that estimate the accuracy 

## Testing the precision while adding nanoseconds to tpd results

### Script 1 : tpd-add-nanosec.cpp -- Dividing one MJD in 8 parts (every 10800seconds) and running the test to figure out how the precision changes

Results: 

Using Normal TwoPartDate::add_seconds()
Starting Date: 60224 0.000000000000000
Test 0:  Date: 60224 0.999999992539933           Dsec=7.46007e-09          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 1e0)
Test 1:  Date: 60224 10801.000444171950221       Dsec=4.44172e-04          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 10800e0)          
Test 2:  Date: 60224 21601.000444171950221       Dsec=4.44172e-04          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 21600e0)
Test 3:  Date: 60224 32401.000444171950221       Dsec=4.44172e-04          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 32400e0)
Test 4:  Date: 60224 43200.996806193143129       Dsec=3.19381e-03          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 43200e0)
Test 5:  Date: 60224 54000.996806193143129       Dsec=3.19381e-03          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 54000e0)
Test 6:  Date: 60224 64800.996806193143129       Dsec=3.19381e-03          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 64800e0)
Test 7:  Date: 60224 75601.004082150757313       Dsec=4.08215e-03          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 75600e0)
Test 8:  Date: 60225 0.004065554552766           Dsec=4.06555e-03          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 86399e0) -- Encountering MJD change
Test 9:  Date: 60224 2.000000082740371           Dsec=8.27404e-08          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 2e0)
Test 10: Date: 60224 91.000003635454050          Dsec=3.63545e-06          -- Adding 1 second in nanoseconds at TwoPartDate(60224, 90e0)

Conclusions: 
 - The results of tests 9,10 show that the accuracy decreases rapidly.
 - By dividing the day into 8 parts and running the tests, a staggered decrease in accuracy is odserved.

### Script 2 : tpd-add-lnanosec.cpp -- Running longer tests to check how quickly the accuracy is decreased

Results:

Using Normal TwoPartDate::add_seconds()
Starting Date: 60224 0.000000000000000
Test 1:  Date: 60224 10.000000737203271         Dsec=7.37203e-07            -- Adding 10 seconds in nanonseconds at TwoPartDate(60224, 1e0)
Test 2:  Date: 60224 2.000000075280303          Dsec=7.52803e-08            -- Adding 2 seconds in nanonseconds at TwoPartDate(60224, 1e0)          
Test 3:  Date: 60224 1.500000033910118          Dsec=3.39101e-08            -- Adding 1.5 seconds in nanonseconds at TwoPartDate(60224, 1e0)
Test 4:  Date: 60224 0.500000006680899          Dsec=6.68090e-09            -- Adding 0.5 seconds in nanonseconds at TwoPartDate(60224, 1e0)

Conclusions:
 - As expected, while the addition time increases, the accuracy decreases.
 - The precisions decreases much slower

### Script 3 : tpd-sub-nanosec.cpp -- Executing the same tests as the first script using a subtractiong instead of addition

Results: 

