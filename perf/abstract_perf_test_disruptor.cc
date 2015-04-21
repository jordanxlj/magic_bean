#include "abstract_perf_test_disruptor.h"
#include <iostream>

void AbstractPerfTestDisruptor::TestImplementations() {
  int64_t* disruptor_ops = new int64_t[RUNS];

  std::cout << "Starting Disruptor Tests." << std::endl;
  for(int i = 0; i < RUNS; i++) {
    disruptor_ops[i] = RunDisruptorPass();
    std::cout << "Run " << i << ", Disruptor=" << disruptor_ops[i] << ", ops/sec." << std::endl;
  }
}
