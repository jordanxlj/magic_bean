#ifndef ONE_TO_ONE_RAW_THROUGHPUT_TEST_H_
#define ONE_TO_ONE_RAW_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include "magic_types.h"
#include "sequencer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"

class OneToOneRawThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToOneRawThroughputTest();
  ~OneToOneRawThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;
 private:
  void Execute(int64_t expected_count);
  void WaitForEventProcessorSequence(int64_t expected_count);

 private:
  magic_bean::Sequencer* sequencer_;
  magic_bean::WaitStrategy* wait_strategy_;
  magic_bean::SequenceBarrier* barrier_;
  magic_bean::SequencePtr sequence_;
  std::vector<magic_bean::SequencePtr> gatings_sequences_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif //ONE_TO_ONE_RAW_THROUGHPUT_TEST_H_
