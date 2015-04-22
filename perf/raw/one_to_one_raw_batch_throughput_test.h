#ifndef ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_
#define ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <mutex>
#include <condition_variable>
#include "sequence.h"
#include "sequencer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"

class OneToOneRawBatchThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToOneRawBatchThroughputTest();
  ~OneToOneRawBatchThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;
 private:
  void Execute(magic_bean::Sequence* sequence, int64_t expected_count);
  void WaitForEventProcessorSequence(magic_bean::Sequence* sequence, int64_t expected_count);

 private:
  magic_bean::Sequencer* sequencer_;
  magic_bean::WaitStrategy* wait_strategy_;
  magic_bean::SequenceBarrier* barrier_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif //ONE_TO_ONE_RAW_BATCH_THROUGHPUT_TEST_H_
