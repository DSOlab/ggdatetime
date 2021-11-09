#include <cstdint>
#include <cstdio>
#include <limits>

template<typename T>
T days_in_nanosec() noexcept {
  return std::numeric_limits<T>::max() / T{1000000000} / T{86400};
}

int main() {
  printf("Program to check standard int sizes for each platform\n");

  printf("type: %12s size:%5ld (bytes) upper limit: %30ld nanoseconds: %30ld\n", "int", sizeof(int), std::numeric_limits<int>::max(), days_in_nanosec<int>());
  printf("type: %12s size:%5ld (bytes) upper limit: %30lu nanoseconds: %30ld\n", "unsigned int", sizeof(unsigned int), std::numeric_limits<unsigned int>::max(), days_in_nanosec<unsigned int>());
  
  printf("type: %12s size:%5ld (bytes) upper limit: %30ld nanoseconds: %30ld\n", "int_fast_16t", sizeof(int_fast16_t), std::numeric_limits<int_fast16_t>::max(), days_in_nanosec<int_fast16_t>());
  printf("type: %12s size:%5ld (bytes) upper limit: %30ld nanoseconds: %30ld\n", "int_fast_32t", sizeof(int_fast32_t), std::numeric_limits<int_fast32_t>::max(), days_in_nanosec<int_fast32_t>());
  printf("type: %12s size:%5ld (bytes) upper limit: %30ld nanoseconds: %30ld\n", "int_fast_64t", sizeof(int_fast64_t), std::numeric_limits<int_fast64_t>::max(), days_in_nanosec<int_fast64_t>());
  
  printf("type: %12s size:%5ld (bytes) upper limit: %30lu nanoseconds: %30lu\n", "uint_fast_16t", sizeof(uint_fast16_t), std::numeric_limits<uint_fast16_t>::max(), days_in_nanosec<uint_fast16_t>());
  printf("type: %12s size:%5ld (bytes) upper limit: %30lu nanoseconds: %30lu\n", "uint_fast_32t", sizeof(uint_fast32_t), std::numeric_limits<uint_fast32_t>::max(), days_in_nanosec<uint_fast32_t>());
  printf("type: %12s size:%5ld (bytes) upper limit: %30lu nanoseconds: %30lu\n", "uint_fast_64t", sizeof(uint_fast64_t), std::numeric_limits<uint_fast64_t>::max(), days_in_nanosec<uint_fast64_t>());
  

  return 0;
}
