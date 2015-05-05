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

#include "one_to_one_sequenced_poller_throughput_test.h"
#include <chrono>
#include <functional>
#include <thread>
#include "support/padded_long.h"
#include "support/perf_test_util.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"
#include <iostream>

static const int BATCH_SIZE = 10;
static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 100;
static const int64_t EXPECTED_RESULT = PerfTestUtil::AccumulateAddition(ITERATIONS);

class PollRunnable : public magic_bean::Handler<ValueEvent> {
 public:
  PollRunnable(magic_bean::EventPoller<ValueEvent>* poller, int64_t expected_count)
    : poller_(poller)
    , running_(true)
    , count_(expected_count) {}

  void Run() {
    try {
      while(running_) {
        if(magic_bean::PollState::PROCESSING != poller_->Poll(this))
          std::this_thread::yield();
      }
    } catch(...) {
      std::cout << "catch poller exception" << std::endl;
    }
  }

  virtual bool OnEvent(ValueEvent* event, int64_t sequence, bool end_of_batch) override {
    value_.Set(value_.Get() + event->GetValue());
    if(count_ == sequence) {
      std::unique_lock<std::mutex> lock(mutex_);
      cond_.notify_all();
    }
    return true;
  }

  void Halt() {
    running_ = false;
  }

  int64_t GetValue() {
    return value_.Get();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock);
  }

 private:
  magic_bean::EventPoller<ValueEvent>* poller_;
  volatile bool running_;
  PaddedLong value_;
  std::mutex mutex_;
  std::condition_variable cond_;
  int64_t count_;
};

OneToOneSequencedPollerThroughputTest::OneToOneSequencedPollerThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  ring_buffer_ = magic_bean::RingBuffer<ValueEvent>::CreateSingleProducer(&event_factory_, BUFFER_SIZE, wait_strategy_);

  poller_ = ring_buffer_->NewPoller(gatings_sequences_);

  gatings_sequences_.push_back(poller_->GetSequence());
  ring_buffer_->AddGatingSequences(gatings_sequences_);
}

OneToOneSequencedPollerThroughputTest::~OneToOneSequencedPollerThroughputTest() {
  ring_buffer_->RemoveGatingSequence(poller_->GetSequence());
  gatings_sequences_.clear();
  delete poller_;
  delete ring_buffer_;
  delete wait_strategy_;
}

int64_t OneToOneSequencedPollerThroughputTest::RunDisruptorPass() {
  int batch_size = 10;
  int64_t expected_count = poller_->GetSequence()->Get() + ITERATIONS;
  PollRunnable poll_runnable(poller_, expected_count);

  std::thread thread(std::bind(&PollRunnable::Run, &poll_runnable));
  auto start = std::chrono::high_resolution_clock::now();
  for(int64_t i = 0; i < ITERATIONS; i++) {
    int64_t next = ring_buffer_->Next();
    ring_buffer_->Get(next)->SetValue(i);
    ring_buffer_->Publish(next);
  }

  poll_runnable.Wait();
  auto end = std::chrono::high_resolution_clock::now();

  int64_t ops_per_second = (ITERATIONS * 1000 * batch_size) / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  WaitForEventProcessorSequence(expected_count);
  poll_runnable.Halt();
  thread.join();
  PerfTestUtil::FailIfNot(EXPECTED_RESULT, poll_runnable.GetValue());
  return ops_per_second;
}

void OneToOneSequencedPollerThroughputTest::WaitForEventProcessorSequence(int64_t expected_count) {
  std::chrono::milliseconds duration(1000);
  while(poller_->GetSequence()->Get() != expected_count) {
    std::this_thread::sleep_for(duration);
  }
}

int main() {
  OneToOneSequencedPollerThroughputTest test;
  test.TestImplementations();
  return 0;
}
