/**
 *
 * MultiCast a series of items between 1 publisher and 3 event processors.
 *
 *           +-----+
 *    +----->| EP1 |
 *    |      +-----+
 *    |
 * +----+    +-----+
 * | P1 |--->| EP2 |
 * +----+    +-----+
 *    |
 *    |      +-----+
 *    +----->| EP3 |
 *           +-----+
 *
 * Disruptor:
 * ==========
 *                             track to prevent wrap
 *             +--------------------+----------+----------+
 *             |                    |          |          |
 *             |                    v          v          v
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 * | P1 |--->| RB |<---| SB |    | EP1 |    | EP2 |    | EP3 |
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 *      claim      get    ^         |          |          |
 *                        |         |          |          |
 *                        +---------+----------+----------+
 *                                      waitFor
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 * EP2 - EventProcessor 2
 * EP3 - EventProcessor 3
 *
 */

#include "one_to_three_sequenced_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "support/perf_test_util.h"
#include "sequence_barrier.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 8;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static int64_t expected_result[3];

static void CalculateExpected() {
  for(int64_t i = 0; i < ITERATIONS; i++) {
    expected_result[0] += i;
    expected_result[1] -= i;
    expected_result[2] &= i;
  }
}

OneToThreeSequencedThroughputTest::OneToThreeSequencedThroughputTest() {
  CalculateExpected();

  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<ValueEvent>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);
  std::vector<magic_bean::SequencePtr> sequences_to_track;
  barrier_ = ring_buffer_->NewBarrier(sequences_to_track);

  handlers_[0] = new ValueMutationEventHandler(Operation::ADDITION);
  handlers_[1] = new ValueMutationEventHandler(Operation::SUBSTRACTION);
  handlers_[2] = new ValueMutationEventHandler(Operation::AND);

  batch_processors_[0] = new magic_bean::BatchEventProcessor<ValueEvent>(ring_buffer_, barrier_, handlers_[0]);
  batch_processors_[1] = new magic_bean::BatchEventProcessor<ValueEvent>(ring_buffer_, barrier_, handlers_[1]);
  batch_processors_[2] = new magic_bean::BatchEventProcessor<ValueEvent>(ring_buffer_, barrier_, handlers_[2]);

  gatings_sequences_.push_back(batch_processors_[0]->GetSequence());
  gatings_sequences_.push_back(batch_processors_[1]->GetSequence());
  gatings_sequences_.push_back(batch_processors_[2]->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

OneToThreeSequencedThroughputTest::~OneToThreeSequencedThroughputTest() {
  ring_buffer_->RemoveGatingSequence(batch_processors_[2]->GetSequence());
  ring_buffer_->RemoveGatingSequence(batch_processors_[1]->GetSequence());
  ring_buffer_->RemoveGatingSequence(batch_processors_[0]->GetSequence());

  gatings_sequences_.clear();
  delete batch_processors_[2];
  delete batch_processors_[1];
  delete batch_processors_[0];

  delete handlers_[2];
  delete handlers_[1];
  delete handlers_[0];
  delete barrier_;
  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t OneToThreeSequencedThroughputTest::RunDisruptorPass() {
  for(int i = 0; i < 3; i++) {
    int64_t expected_count = batch_processors_[i]->GetSequence()->Get() + ITERATIONS;
    handlers_[i]->Reset(expected_count);
  }

  std::thread first_thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run,
                                     batch_processors_[0]));
  std::thread second_thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run,
                                      batch_processors_[1]));
  std::thread third_thread(std::bind(&magic_bean::BatchEventProcessor<ValueEvent>::Run,
                                     batch_processors_[2]));

  auto start = std::chrono::high_resolution_clock::now();
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t sequence = ring_buffer_->Next();
    ring_buffer_->Get(sequence)->SetValue(i);
    ring_buffer_->Publish(sequence);
  }

  handlers_[0]->Wait();
  handlers_[1]->Wait();
  handlers_[2]->Wait();

  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  for(int i = 0; i < 3; i++) {
    batch_processors_[i]->Halt();
    PerfTestUtil::FailIfNot(expected_result[i], handlers_[i]->GetValue());
  }

  first_thread.join();
  second_thread.join();
  third_thread.join();

  return ops_per_second;
}

/*
int main() {
  OneToThreeSequencedThroughputTest test;
  test.TestImplementations();
  return 0;
}
*/
