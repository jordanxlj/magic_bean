/**
 * UniCast a series of items between 1 publisher and 1 event processor.
 *
 * +----+    +-----+
 * | P1 |--->| EP1 |
 * +----+    +-----+
 *
 *
 * Queue Based:
 * ============
 *
 *        put      take
 * +----+    +====+    +-----+
 * | P1 |--->| Q1 |<---| EP1 |
 * +----+    +====+    +-----+
 *
 * P1  - Publisher 1
 * Q1  - Queue 1
 * EP1 - EventProcessor 1
 *
 *
 * Disruptor:
 * ==========
 *              track to prevent wrap
 *              +------------------+
 *              |                  |
 *              |                  v
 * +----+    +====+    +====+   +-----+
 * | P1 |--->| RB |<---| SB |   | EP1 |
 * +----+    +====+    +====+   +-----+
 *      claim      get    ^        |
 *                        |        |
 *                        +--------+
 *                          waitFor
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 *
 */

#include "one_to_one_sequenced_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BATCH_SIZE = 10;
static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static const int64_t EXPECTED_RESULT = PerfTestUtil::AccumulateAddition(ITERATIONS);

OneToOneSequencedThroughputTest::OneToOneSequencedThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<ValueEvent>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);
  std::vector<magic_bean::SequencePtr> sequences_to_track;
  barrier_ = ring_buffer_->NewBarrier(sequences_to_track);

  batch_event_processor_ = new magic_bean::BatchEventProcessor<ValueEvent>(ring_buffer_, barrier_, &handler_);
  gatings_sequences_.push_back(batch_event_processor_->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

OneToOneSequencedThroughputTest::~OneToOneSequencedThroughputTest() {
  ring_buffer_->RemoveGatingSequence(batch_event_processor_->GetSequence());
  gatings_sequences_.clear();
  delete batch_event_processor_;
  delete barrier_;
  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t OneToOneSequencedThroughputTest::RunDisruptorPass() {
  int batch_size = 10;
  int64_t expected_count = batch_event_processor_->GetSequence()->Get() + ITERATIONS;
  handler_.Reset(expected_count);

  std::thread thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run, batch_event_processor_));
  auto start = std::chrono::high_resolution_clock::now();
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t next = ring_buffer_->Next();
    ring_buffer_->Get(next)->SetValue(i);
    ring_buffer_->Publish(next);
  }

  handler_.Wait();
  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000 * batch_size) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  WaitForEventProcessorSequence(expected_count);
  batch_event_processor_->Halt();
  thread.join();
  PerfTestUtil::FailIfNot(EXPECTED_RESULT, handler_.GetValue());
  return ops_per_second;
}

void OneToOneSequencedThroughputTest::WaitForEventProcessorSequence(int64_t expected_count) {
  std::chrono::milliseconds duration(1000);
  while(batch_event_processor_->GetSequence()->Get() != expected_count) {
    std::this_thread::sleep_for(duration);
  }
}

int main() {
  OneToOneSequencedThroughputTest test;
  test.TestImplementations();
  return 0;
}
