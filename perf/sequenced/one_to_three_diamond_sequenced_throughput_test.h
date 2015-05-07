#ifndef ONE_TO_THREE_DIAMOND_SEQUENCED_THROUGHPUT_TEST_H_
#define ONE_TO_THREE_DIAMOND_SEQUENCED_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <vector>
#include "batch_event_processor.h"
#include "magic_types.h"
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"
#include "support/fizz_buzz_event.h"
#include "support/fizz_buzz_event_handler.h"

class OneToThreeDiamondSequencedThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToThreeDiamondSequencedThroughputTest();
  ~OneToThreeDiamondSequencedThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;
 private:
  void WaitForEventProcessorSequence(int64_t expected_count);

 private:
  magic_bean::RingBuffer<FizzBuzzEvent>* ring_buffer_;
  magic_bean::WaitStrategy* wait_strategy_;
  magic_bean::SequenceBarrier* barrier_;
  FizzBuzzEventFactory event_factory_;

  FizzBuzzEventHandler* fizz_handler_;
  magic_bean::BatchEventProcessor<FizzBuzzEvent>* batch_processor_fizz_;
  FizzBuzzEventHandler* buzz_handler_;
  magic_bean::BatchEventProcessor<FizzBuzzEvent>* batch_processor_buzz_;
  magic_bean::SequenceBarrier* barrier_fizz_buzz_;
  FizzBuzzEventHandler* fizz_buzz_handler_;
  magic_bean::BatchEventProcessor<FizzBuzzEvent>* batch_processor_fizz_buzz_;

  std::vector<magic_bean::SequencePtr> gatings_sequences_;
};

#endif //ONE_TO_THREE_DIAMOND_SEQUENCED_THROUGHPUT_TEST_H_
