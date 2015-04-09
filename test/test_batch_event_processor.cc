#include "batch_event_processor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>
#include <thread>
#include <condition_variable>
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "support/stub_event.h"
#include <stdio.h>

using namespace magic_bean;
using namespace ::testing;

class MockEventHandler : public EventHandler<StubEvent> {
 public:
  MOCK_METHOD3(OnEvent, void(StubEvent*, int64_t, bool));
};

class BatchEventProcessorTest : public Test {
 public:
  void SignalAllWhenBlocking(StubEvent*, int64_t, bool) {
    cond.notify_all();
  }

 protected:
  virtual void SetUp() {
    event_factory = new StubEventFactory;
    ring_buffer = RingBuffer<StubEvent>::CreateSingleProducer(event_factory, 16);
    std::vector<SequencePtr> sequences_to_track;
    sequence_barrier = ring_buffer->NewBarrier(sequences_to_track);
    event_handler = new MockEventHandler;
    batch_event_processor = new BatchEventProcessor<StubEvent>(ring_buffer, sequence_barrier,
                                                               event_handler);
    std::vector<SequencePtr> gating_sequences;
    gating_sequences.push_back(batch_event_processor->GetSequence());
    ring_buffer->AddGatingSequences(gating_sequences);
  }

  virtual void TearDown() {
    delete batch_event_processor;
    delete event_handler;
    delete sequence_barrier;
    delete ring_buffer;
    delete event_factory;
  }

 protected:
  EventFactory<StubEvent>* event_factory;
  RingBuffer<StubEvent>* ring_buffer;
  SequenceBarrier* sequence_barrier;
  MockEventHandler* event_handler;
  BatchEventProcessor<StubEvent>* batch_event_processor;
  std::condition_variable cond;
  std::mutex mutex;
};


TEST_F(BatchEventProcessorTest, should_call_methods_in_lifecycle_order) {
  EXPECT_CALL(*event_handler, OnEvent(ring_buffer->Get(0), 0, true))
    .WillOnce(Invoke(this, &BatchEventProcessorTest::SignalAllWhenBlocking));

  std::unique_lock<std::mutex> lock(mutex);
  std::thread thread(std::bind(&BatchEventProcessor<StubEvent>::Run, batch_event_processor));
  ASSERT_EQ(-1, batch_event_processor->GetSequence()->Get());
  ring_buffer->Publish(ring_buffer->Next());
  cond.wait(lock);
  batch_event_processor->Halt();
  thread.join();
}
