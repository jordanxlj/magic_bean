/**
 * Produce an event replicated to two event processors and fold back to a single third event processor.
 *
 *           +-----+
 *    +----->| EP1 |------+
 *    |      +-----+      |
 *    |                   v
 * +----+              +-----+
 * | P1 |              | EP3 |
 * +----+              +-----+
 *    |                   ^
 *    |      +-----+      |
 *    +----->| EP2 |------+
 *           +-----+
 *
 * Disruptor:
 * ==========
 *                    track to prevent wrap
 *              +-------------------------------+
 *              |                               |
 *              |                               v
 * +----+    +====+               +=====+    +-----+
 * | P1 |--->| RB |<--------------| SB2 |<---| EP3 |
 * +----+    +====+               +=====+    +-----+
 *      claim   ^  get               |   waitFor
 *              |                    |
 *           +=====+    +-----+      |
 *           | SB1 |<---| EP1 |<-----+
 *           +=====+    +-----+      |
 *              ^                    |
 *              |       +-----+      |
 *              +-------| EP2 |<-----+
 *             waitFor  +-----+
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB1 - SequenceBarrier 1
 * EP1 - EventProcessor 1
 * EP2 - EventProcessor 2
 * SB2 - SequenceBarrier 2
 * EP3 - EventProcessor 3
 *
 */

#include "one_to_three_diamond_sequenced_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 8;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static int64_t EXPECTED_RESULT = 0;

OneToThreeDiamondSequencedThroughputTest::OneToThreeDiamondSequencedThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<FizzBuzzEvent>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);
  std::vector<magic_bean::SequencePtr> sequences_to_track;
  barrier_ = ring_buffer_->NewBarrier(sequences_to_track);

  fizz_handler_ = new FizzBuzzEventHandler(FizzBuzzStep::FIZZ);
  batch_processor_fizz_ = new magic_bean::BatchEventProcessor<FizzBuzzEvent>(ring_buffer_, barrier_, fizz_handler_);

  buzz_handler_ = new FizzBuzzEventHandler(FizzBuzzStep::BUZZ);
  batch_processor_buzz_ = new magic_bean::BatchEventProcessor<FizzBuzzEvent>(ring_buffer_, barrier_, buzz_handler_);

  sequences_to_track.push_back(batch_processor_fizz_->GetSequence());
  sequences_to_track.push_back(batch_processor_buzz_->GetSequence());
  barrier_fizz_buzz_ = ring_buffer_->NewBarrier(sequences_to_track);

  fizz_buzz_handler_ = new FizzBuzzEventHandler(FizzBuzzStep::FIZZ_BUZZ);
  batch_processor_fizz_buzz_ = new magic_bean::BatchEventProcessor<FizzBuzzEvent>(ring_buffer_, barrier_fizz_buzz_, fizz_buzz_handler_);

  gatings_sequences_.push_back(batch_processor_fizz_buzz_->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

OneToThreeDiamondSequencedThroughputTest::~OneToThreeDiamondSequencedThroughputTest() {
  ring_buffer_->RemoveGatingSequence(batch_processor_fizz_buzz_->GetSequence());
  gatings_sequences_.clear();
  delete batch_processor_fizz_buzz_;
  delete fizz_buzz_handler_;
  delete barrier_fizz_buzz_;
  delete batch_processor_buzz_;
  delete buzz_handler_;
  delete batch_processor_fizz_;
  delete fizz_handler_;
  delete barrier_;
  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t OneToThreeDiamondSequencedThroughputTest::RunDisruptorPass() {
  /*
  int batch_size = 10;
  int64_t expected_count = batch_event_processor_->GetSequence()->Get() + (ITERATIONS * batch_size);
  handler_.Reset(expected_count);

  std::thread thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run, batch_event_processor_));
  auto start = std::chrono::high_resolution_clock::now();
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t hi = ring_buffer_->Next(batch_size);
    int64_t lo = hi - (batch_size - 1);
    for(int64_t l = lo; l <= hi; l++)
      ring_buffer_->Get(l)->SetValue(i);
    ring_buffer_->Publish(lo, hi);
  }

  handler_.Wait();
  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000 * batch_size) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  WaitForEventProcessorSequence(expected_count);
  batch_event_processor_->Halt();
  thread.join();
  PerfTestUtil::FailIfNot(EXPECTED_RESULT, handler_.GetValue());
  return ops_per_second;
  */
  return 0;
}

void OneToThreeDiamondSequencedThroughputTest::WaitForEventProcessorSequence(int64_t expected_count) {
  /*
  std::chrono::milliseconds duration(1000);
  while(batch_event_processor_->GetSequence()->Get() != expected_count) {
    std::this_thread::sleep_for(duration);
  }
  */
}

int main() {
  OneToThreeDiamondSequencedThroughputTest test;
  test.TestImplementations();
  return 0;
}
