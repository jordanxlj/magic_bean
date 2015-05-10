/**
 *
 * Sequence a series of events from multiple publishers going to one event processor.
 *
 * +----+
 * | P1 |------+
 * +----+      |
 *             v
 * +----+    +-----+
 * | P1 |--->| EP1 |
 * +----+    +-----+
 *             ^
 * +----+      |
 * | P3 |------+
 * +----+
 *
 * Disruptor:
 * ==========
 *             track to prevent wrap
 *             +--------------------+
 *             |                    |
 *             |                    v
 * +----+    +====+    +====+    +-----+
 * | P1 |--->| RB |<---| SB |    | EP1 |
 * +----+    +====+    +====+    +-----+
 *             ^   get    ^         |
 * +----+      |          |         |
 * | P2 |------+          +---------+
 * +----+      |            waitFor
 *             |
 * +----+      |
 * | P3 |------+
 * +----+
 *
 * P1  - Publisher 1
 * P2  - Publisher 2
 * P3  - Publisher 3
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 *
 */

#include "three_to_one_sequenced_batch_throughput_test.h"
#include <chrono>
#include <future>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "busy_spin_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static const int NUM_PUBLISHERS = 3;

ThreeToOneSequencedBatchThroughputTest::ThreeToOneSequencedBatchThroughputTest() {
  wait_strategy_ =  new magic_bean::BusySpinWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<ValueEvent>::CreateMultiProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);
  std::vector<magic_bean::SequencePtr> sequences_to_track;
  barrier_ = ring_buffer_->NewBarrier(sequences_to_track);

  batch_processor_ = new magic_bean::BatchEventProcessor<ValueEvent>(ring_buffer_, barrier_, &handler_);
  for(int i = 0; i < NUM_PUBLISHERS; i++)
    value_publishers_[i] = new ValueBatchPublisher(ring_buffer_, ITERATIONS/NUM_PUBLISHERS, 10);

  gatings_sequences_.push_back(batch_processor_->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

ThreeToOneSequencedBatchThroughputTest::~ThreeToOneSequencedBatchThroughputTest() {
  ring_buffer_->RemoveGatingSequence(batch_processor_->GetSequence());
  gatings_sequences_.clear();
  for(int i = 0; i < NUM_PUBLISHERS; i++)
    delete value_publishers_[i];

  delete batch_processor_;
  delete barrier_;
  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t ThreeToOneSequencedBatchThroughputTest::RunDisruptorPass() {
  int64_t expected_count = batch_processor_->GetSequence()->Get() + (ITERATIONS / NUM_PUBLISHERS) * NUM_PUBLISHERS;
  handler_.Reset(expected_count);

  std::future<void> futures[NUM_PUBLISHERS];
  for(int i = 0; i < NUM_PUBLISHERS; i++)
    futures[i] = std::async(std::bind(&ValueBatchPublisher::Run, value_publishers_[i]));
  std::thread thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run,
                               batch_processor_));

  auto start = std::chrono::high_resolution_clock::now();

  for(int i = 0; i < NUM_PUBLISHERS; i++)
    value_publishers_[i]->SignalAll();

  for(int i = 0; i < NUM_PUBLISHERS; i++)
    futures[i].get();

  handler_.Wait();

  auto end = std::chrono::high_resolution_clock::now();
  int64_t ops_per_second = (ITERATIONS * 1000) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  batch_processor_->Halt();
  thread.join();

  return ops_per_second;
}

int main() {
  ThreeToOneSequencedBatchThroughputTest test;
  test.TestImplementations();
  return 0;
}
