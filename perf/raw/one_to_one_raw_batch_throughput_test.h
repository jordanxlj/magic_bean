#ifndef ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_
#define ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <mutex>
#include <condition_variable>
#include "sequencer.h"

class OneToOneRawBatchThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToOneRawBatchThroughputTest() {};
  ~OneToOneRawBatchThroughputTest() {};
 protected:
  virtual int64_t RunDisruptorPass() override;
 private:
  Sequencer* sequencer_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif //ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_
