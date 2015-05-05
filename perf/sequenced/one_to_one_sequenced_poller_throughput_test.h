#ifndef ONE_TO_ONE_SEQUENCED_POLLER_THROUGHPUT_TEST_H_
#define ONE_TO_ONE_SEQUENCED_POLLER_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <vector>
#include "magic_types.h"
#include "event_poller.h"
#include "ring_buffer.h"
#include "wait_strategy.h"
#include "support/value_event.h"

class OneToOneSequencedPollerThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToOneSequencedPollerThroughputTest();
  ~OneToOneSequencedPollerThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;
 private:
  void WaitForEventProcessorSequence(int64_t expected_count);

 private:
  magic_bean::RingBuffer<ValueEvent>* ring_buffer_;
  magic_bean::WaitStrategy* wait_strategy_;
  ValueEventFactory event_factory_;
  magic_bean::EventPoller<ValueEvent>* poller_;
  std::vector<magic_bean::SequencePtr> gatings_sequences_;
};

#endif //ONE_TO_ONE_SEQUENCED_POLLER_THROUGHPUT_TEST_H_
