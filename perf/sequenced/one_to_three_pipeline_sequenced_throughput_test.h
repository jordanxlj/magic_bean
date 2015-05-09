#ifndef ONE_TO_THREE_PIPELINE_SEQUENCED_THROUGHPUT_TEST_H_
#define ONE_TO_THREE_PIPELINE_SEQUENCED_THROUGHPUT_TEST_H_

#include "abstract_perf_test_disruptor.h"
#include <vector>
#include "batch_event_processor.h"
#include "magic_types.h"
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "wait_strategy.h"
#include "support/function_event.h"
#include "support/function_event_handler.h"

class OneToThreePipelineSequencedThroughputTest : public AbstractPerfTestDisruptor {
 public:
  OneToThreePipelineSequencedThroughputTest();
  ~OneToThreePipelineSequencedThroughputTest();
 protected:
  virtual int64_t RunDisruptorPass() override;

 private:
  magic_bean::RingBuffer<FunctionEvent>* ring_buffer_;
  magic_bean::WaitStrategy* wait_strategy_;
  FunctionEventFactory event_factory_;

  magic_bean::SequenceBarrier* step_one_barrier_;
  FunctionEventHandler* step_one_handler_;
  magic_bean::BatchEventProcessor<FunctionEvent>* step_one_batch_processor_;

  magic_bean::SequenceBarrier* step_two_barrier_;
  FunctionEventHandler* step_two_handler_;
  magic_bean::BatchEventProcessor<FunctionEvent>* step_two_batch_processor_;

  magic_bean::SequenceBarrier* step_three_barrier_;
  FunctionEventHandler* step_three_handler_;
  magic_bean::BatchEventProcessor<FunctionEvent>* step_three_batch_processor_;

  std::vector<magic_bean::SequencePtr> gatings_sequences_;
};

#endif //ONE_TO_THREE_PIPELINE_SEQUENCED_THROUGHPUT_TEST_H_
