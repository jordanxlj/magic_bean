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

#include "one_to_one_raw_batch_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "sequence.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 200;

OneToOneRawBatchThroughputTest::OneToOneRawBatchThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  sequencer_ = new magic_bean::SingleProducerSequencer(BUFFER_SIZE, wait_strategy_);
  std::vector<magic_bean::SequencePtr> sequences_to_track;
  barrier_ = sequencer_->NewBarrier(sequences_to_track);

  sequence_ = magic_bean::SequencePtr(new magic_bean::Sequence);
  gatings_sequences_.push_back(sequence_);

  sequencer_->AddGatingSequences(gatings_sequences_);
}

OneToOneRawBatchThroughputTest::~OneToOneRawBatchThroughputTest() {
  sequencer_->RemoveGatingSequence(sequence_);
  gatings_sequences_.clear();
  sequence_.reset();
  delete barrier_;
  delete sequencer_;
  delete wait_strategy_;
}

int64_t OneToOneRawBatchThroughputTest::RunDisruptorPass() {
  int batch_size = 10;
  std::unique_lock<std::mutex> lock(mutex_);
  int64_t expected_count = sequence_->Get() + (ITERATIONS * batch_size);

  std::thread thread(std::bind(&OneToOneRawBatchThroughputTest::Execute, this, expected_count));
  auto start = std::chrono::high_resolution_clock::now();
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t next = sequencer_->Next(batch_size);
    sequencer_->Publish((next - (batch_size - 1)), next);
  }

  cond_.wait(lock);
  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000 * batch_size) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  WaitForEventProcessorSequence(expected_count);
  thread.join();
  return ops_per_second;
}

void OneToOneRawBatchThroughputTest::Execute(int64_t expected_count) {
  int64_t expected = expected_count;
  int64_t processed = -1;

  do {
    processed = barrier_->WaitFor(sequence_->Get() + 1);
    sequence_->Set(processed);
  } while(processed < expected);

  {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.notify_all();
  }
  sequence_->Set(processed);
}

void OneToOneRawBatchThroughputTest::WaitForEventProcessorSequence(int64_t expected_count) {
  std::chrono::milliseconds duration(1000);
  while(sequence_->Get() != expected_count) {
    std::this_thread::sleep_for(duration);
  }
}

int main() {
  OneToOneRawBatchThroughputTest test;
  test.TestImplementations();
  return 0;
}
