/**
 *
 * Pipeline a series of stages from a publisher to ultimate event processor.
 * Each event processor depends on the output of the event processor.
 *
 * +----+    +-----+    +-----+    +-----+
 * | P1 |--->| EP1 |--->| EP2 |--->| EP3 |
 * +----+    +-----+    +-----+    +-----+
 *
 *
 * Disruptor:
 * ==========
 *                           track to prevent wrap
 *              +----------------------------------------------------------------+
 *              |                                                                |
 *              |                                                                v
 * +----+    +====+    +=====+    +-----+    +=====+    +-----+    +=====+    +-----+
 * | P1 |--->| RB |    | SB1 |<---| EP1 |<---| SB2 |<---| EP2 |<---| SB3 |<---| EP3 |
 * +----+    +====+    +=====+    +-----+    +=====+    +-----+    +=====+    +-----+
 *      claim   ^  get    |   waitFor           |   waitFor           |  waitFor
 *              |         |                     |                     |
 *              +---------+---------------------+---------------------+
 *        </pre>
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB1 - SequenceBarrier 1
 * EP1 - EventProcessor 1
 * SB2 - SequenceBarrier 2
 * EP2 - EventProcessor 2
 * SB3 - SequenceBarrier 3
 * EP3 - EventProcessor 3
 *
 */

#include "one_to_three_pipeline_sequenced_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static int64_t CalculateExpected();
static const int BUFFER_SIZE = 1024 * 8;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static const int64_t OPERAND_TWO_INITIAL_VALUE = 777;
static int64_t EXPECTED_RESULT = CalculateExpected();

static int64_t CalculateExpected() {
  int64_t result = 0;
  int64_t operand_two = OPERAND_TWO_INITIAL_VALUE;
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t step_one_result = i + operand_two--;
    int64_t step_two_result = step_one_result + 3;
    if((step_two_result & 4) == 4)
      ++result;
  }
  return result;
}

OneToThreePipelineSequencedThroughputTest::OneToThreePipelineSequencedThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<FunctionEvent>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);

  std::vector<magic_bean::SequencePtr> sequences_to_track;
  step_one_barrier_ = ring_buffer_->NewBarrier(sequences_to_track);
  step_one_handler_ = new FunctionEventHandler(FunctionStep::ONE);
  step_one_batch_processor_ = new magic_bean::BatchEventProcessor<FunctionEvent>(ring_buffer_, step_one_barrier_, step_one_handler_);

  sequences_to_track.push_back(step_one_batch_processor_->GetSequence());
  step_two_barrier_ = ring_buffer_->NewBarrier(sequences_to_track);
  step_two_handler_ = new FunctionEventHandler(FunctionStep::TWO);
  step_two_batch_processor_ = new magic_bean::BatchEventProcessor<FunctionEvent>(ring_buffer_, step_two_barrier_, step_two_handler_);

  sequences_to_track.clear();
  sequences_to_track.push_back(step_two_batch_processor_->GetSequence());
  step_three_barrier_ = ring_buffer_->NewBarrier(sequences_to_track);
  step_three_handler_ = new FunctionEventHandler(FunctionStep::THREE);
  step_three_batch_processor_ = new magic_bean::BatchEventProcessor<FunctionEvent>(ring_buffer_, step_three_barrier_, step_three_handler_);

  gatings_sequences_.push_back(step_three_batch_processor_->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

OneToThreePipelineSequencedThroughputTest::~OneToThreePipelineSequencedThroughputTest() {
  ring_buffer_->RemoveGatingSequence(step_three_batch_processor_->GetSequence());
  gatings_sequences_.clear();
  delete step_three_batch_processor_;
  delete step_three_handler_;
  delete step_three_barrier_;

  delete step_two_batch_processor_;
  delete step_two_handler_;
  delete step_two_barrier_;

  delete step_one_batch_processor_;
  delete step_one_handler_;
  delete step_one_barrier_;

  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t OneToThreePipelineSequencedThroughputTest::RunDisruptorPass() {
  int64_t expected_count = step_three_batch_processor_->GetSequence()->Get() + ITERATIONS;
  step_three_handler_->Reset(expected_count);

  std::thread step_one_thread(std::bind(&magic_bean::BatchEventProcessor<FunctionEvent>::Run,
                                    step_one_batch_processor_));
  std::thread step_two_thread(std::bind(&magic_bean::BatchEventProcessor<FunctionEvent>::Run,
                                    step_two_batch_processor_));
  std::thread step_three_thread(std::bind(&magic_bean::BatchEventProcessor<FunctionEvent>::Run,
                                         step_three_batch_processor_));

  auto start = std::chrono::high_resolution_clock::now();
  int64_t operand_two = OPERAND_TWO_INITIAL_VALUE;
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t sequence = ring_buffer_->Next();
    FunctionEvent* event = ring_buffer_->Get(sequence);
    event->SetOperandOne(i);
    event->SetOperandTwo(operand_two--);
    ring_buffer_->Publish(sequence);
  }

  step_three_handler_->Wait();
  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  step_one_batch_processor_->Halt();
  step_two_batch_processor_->Halt();
  step_three_batch_processor_->Halt();

  step_one_thread.join();
  step_two_thread.join();
  step_three_thread.join();

  PerfTestUtil::FailIfNot(EXPECTED_RESULT, step_three_handler_->GetStepThreeCounter());
  return ops_per_second;
}

/*
int main() {
  OneToThreePipelineSequencedThroughputTest test;
  test.TestImplementations();
  return 0;
}
*/
