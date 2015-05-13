/**
 *
 * Sequence a series of events from multiple publishers going to one event processor.
 *
 * Disruptor:
 * ==========
 *             track to prevent wrap
 *             +--------------------+
 *             |                    |
 *             |                    |
 * +----+    +====+    +====+       |
 * | P1 |--->| RB |--->| SB |--+    |
 * +----+    +====+    +====+  |    |
 *                             |    v
 * +----+    +====+    +====+  | +----+
 * | P2 |--->| RB |--->| SB |--+>| EP |
 * +----+    +====+    +====+  | +----+
 *                             |
 * +----+    +====+    +====+  |
 * | P3 |--->| RB |--->| SB |--+
 * +----+    +====+    +====+
 *
 * P1 - Publisher 1
 * P2 - Publisher 2
 * P3 - Publisher 3
 * RB - RingBuffer
 * SB - SequenceBarrier
 * EP - EventProcessor
 *
 */

#include "three_to_three_sequenced_throughput_test.h"
#include <chrono>
#include <future>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 180;
static const int NUM_PUBLISHERS = 3;

ThreeToThreeSequencedThroughputTest::ThreeToThreeSequencedThroughputTest() {
  for(int i = 0; i < NUM_PUBLISHERS; i++) {
    wait_strategies_[i] =  new magic_bean::YieldingWaitStrategy;
    buffers_[i] = magic_bean::RingBuffer<int64_t>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategies_[i]);
    std::vector<magic_bean::SequencePtr> sequences_to_track;
    barriers_[i] = buffers_[i]->NewBarrier(sequences_to_track);
    value_publishers_[i] = new LongArrayPublisher(buffers_[i], ITERATIONS / NUM_PUBLISHERS);
  }

  batch_event_processor_ = new MultiBufferBatchEventProcessor<int64_t>(buffers_, barriers_, &handler_, NUM_PUBLISHERS);

  for(int i = 0; i < NUM_PUBLISHERS; i++) {
    std::vector<magic_bean::SequencePtr> gatings_sequences;
    gatings_sequences.push_back(batch_event_processor_->GetSequences()[i]);
    buffers_[i]->AddGatingSequences(gatings_sequences);
  }
}

ThreeToThreeSequencedThroughputTest::~ThreeToThreeSequencedThroughputTest() {
  for(int i = 0; i < NUM_PUBLISHERS; i++)
    buffers_[i]->RemoveGatingSequence(batch_event_processor_->GetSequences()[i]);

  delete batch_event_processor_;

  for(int i = 0; i < NUM_PUBLISHERS; i++) {
    delete value_publishers_[i];
    delete barriers_[i];
    delete buffers_[i];
    delete wait_strategies_[i];
  }
}

int64_t ThreeToThreeSequencedThroughputTest::RunDisruptorPass() {
  handler_.Reset(ITERATIONS);

  std::future<void> futures[NUM_PUBLISHERS];
  for(int i = 0; i < NUM_PUBLISHERS; i++)
    futures[i] = std::async(std::bind(&LongArrayPublisher::Run, value_publishers_[i]));
  std::thread thread(std::bind(&MultiBufferBatchEventProcessor<int64_t>::Run,
                               batch_event_processor_));

  auto start = std::chrono::high_resolution_clock::now();

  for(int i = 0; i < NUM_PUBLISHERS; i++)
    value_publishers_[i]->SignalAll();

  for(int i = 0; i < NUM_PUBLISHERS; i++)
    futures[i].get();

  handler_.Wait();

  auto end = std::chrono::high_resolution_clock::now();
  int64_t ops_per_second = (ITERATIONS * 1000 * 3) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  batch_event_processor_->Halt();
  thread.join();

  return ops_per_second;
}

int main() {
  ThreeToThreeSequencedThroughputTest test;
  test.TestImplementations();
  return 0;
}
