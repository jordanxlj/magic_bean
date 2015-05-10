#ifndef THREE_TO_ONE_SEQUENCED_THROUGHPUT_TEST_H_
#define THREE_TO_ONE_SEQUENCED_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <vector>
#include "batch_event_processor.h"
#include "magic_types.h"
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"
#include "support/value_publisher.h"
#include "support/value_event.h"
#include "support/value_addition_event_handler.h"

class ThreeToOneSequencedThroughputTest : public AbstractPerfTestDisruptor {
 public:
  ThreeToOneSequencedThroughputTest();
  ~ThreeToOneSequencedThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;

 private:
  magic_bean::RingBuffer<ValueEvent>* ring_buffer_;
  magic_bean::WaitStrategy* wait_strategy_;
  magic_bean::SequenceBarrier* barrier_;
  ValueEventFactory event_factory_;
  ValueAdditionEventHandler handler_;
  magic_bean::BatchEventProcessor<ValueEvent>* batch_processor_;
  ValuePublisher* value_publishers_[3];
  std::vector<magic_bean::SequencePtr> gatings_sequences_;
};

#endif //THREE_TO_ONE_SEQUENCED_THROUGHPUT_TEST_H_
