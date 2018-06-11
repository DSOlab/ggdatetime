#include <iostream>
#include "dtfund.hpp"

using ngpt::seconds;
using ngpt::milliseconds;
using ngpt::microseconds;

int main()
{
    seconds sec {1123};
    milliseconds mllsec {1234567};
    microseconds mcrsec {1234567890};

    std::cout<<"\nThis is how many seconds:      "<<sec.as_underlying_type();
    std::cout<<"\nThis is how many milliseconds: "<<mllsec.as_underlying_type();
    std::cout<<"\nThis is how many microseconds: "<<mcrsec.as_underlying_type();

    auto hmsf = sec.to_hmsf();
    std::cout<<"\nHours: "<<std::get<0>(hmsf).as_underlying_type() 
        <<", Minutes: "<<std::get<1>(hmsf).as_underlying_type()
        <<", Seconds: "<<std::get<2>(hmsf).as_underlying_type();

    std::cout<<"\n";
    return 0;
}
