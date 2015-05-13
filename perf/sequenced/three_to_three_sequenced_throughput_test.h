#ifndef THREE_TO_THREE_SEQUENCED_THROUGHPUT_TEST_H_
#define THREE_TO_THREE_SEQUENCED_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <vector>
#include "magic_types.h"
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"
#include "support/long_array_event_handler.h"
#include "support/long_array_publisher.h"
#include "support/multi_buffer_batch_event_processor.h"

class LongArrayFactory : public magic_bean::EventFactory<int64_t> {
 public:
  LongArrayFactory() {};
  ~LongArrayFactory() {};

  virtual int64_t* NewInstance() override {
    return new int64_t[3];
  }
};

class ThreeToThreeSequencedThroughputTest : public AbstractPerfTestDisruptor {
 public:
  ThreeToThreeSequencedThroughputTest();
  ~ThreeToThreeSequencedThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;

 private:
  magic_bean::RingBuffer<int64_t>* buffers_[3];
  magic_bean::WaitStrategy* wait_strategies_[3];
  magic_bean::SequenceBarrier* barriers_[3];
  LongArrayFactory event_factory_;
  LongArrayEventHandler handler_;
  MultiBufferBatchEventProcessor<int64_t>* batch_event_processor_;
  LongArrayPublisher* value_publishers_[3];
  std::vector<magic_bean::SequencePtr> gatings_sequences_;
};

#endif //THREE_TO_THREE_SEQUENCED_THROUGHPUT_TEST_H_
