#ifndef ABSTRACT_PERF_TEST_DISRUPTOR_H_
#define ABSTRACT_PERF_TEST_DISRUPTOR_H_

#include <cstdint>

class AbstractPerfTestDisruptor {
 public:
  virtual ~AbstractPerfTestDisruptor() {};

  void TestImplementations();
 protected:
  virtual int64_t RunDisruptorPass() = 0;
 public:
  static const int RUNS = 7;
};

#endif //ABSTRACT_PERF_TEST_DISRUPTOR_H_
