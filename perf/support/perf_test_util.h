#ifndef PERF_TEST_UTIL_H_
#define PERF_TEST_UTIL_H_

#include <cstdint>
#include <iostream>
#include <stdexcept>

class PerfTestUtil {
 public:
  static int64_t AccumulateAddition(int64_t iterations) {
    int64_t result = 0;
    for(int64_t i = 0; i < iterations; i++)
      result += i;
    return result;
  }
  static void FailIfNot(int64_t a, int64_t b) {
    if(a != b) {
      std::cout << "a : " << a << ", b : " << b << std::endl;
      throw std::runtime_error("Failed");
    }
  }
};

#endif //PERF_TEST_UTIL_H_
