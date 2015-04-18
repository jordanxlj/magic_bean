#include "ring_buffer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>
#include <thread>
#include <condition_variable>
#include "sequence.h"
#include "sequence_barrier.h"
#include "support/stub_event.h"

using namespace magic_bean;

class RingBufferTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    event_factory = new StubEventFactory;
    ring_buffer = RingBuffer<StubEvent>::CreateMultiProducer(event_factory, 32);
    std::vector<SequencePtr> sequences_to_track;
    sequence_barrier = ring_buffer->NewBarrier(sequences_to_track);

    std::vector<SequencePtr> gating_sequences;
    gating_sequence = SequencePtr(new Sequence());
    gating_sequences.push_back(gating_sequence);
    ring_buffer->AddGatingSequences(gating_sequences);
  }
  virtual void TearDown() {
    gating_sequences.clear();
    gating_sequence.reset();
    delete sequence_barrier;
    delete ring_buffer;
    delete event_factory;
  }

 protected:
  EventFactory<StubEvent>* event_factory;
  RingBuffer<StubEvent>* ring_buffer;
  SequenceBarrier* sequence_barrier;

  SequencePtr gating_sequence;
  std::vector<SequencePtr> gating_sequences;

  static const int64_t INITIAL_CURSOR_VALUE = -1;
};

TEST_F(RingBufferTest, should_claim_and_get) {
  ASSERT_EQ(ring_buffer->GetCursor(), INITIAL_CURSOR_VALUE);
  StubEvent expected_event(2701);
  StubEventTranslator translator;
  ring_buffer->PublishEvent(&translator, expected_event.GetValue());

  int64_t sequence = sequence_barrier->WaitFor(0);
  ASSERT_EQ(sequence, 0);

  StubEvent* event = ring_buffer->Get(sequence);
  ASSERT_EQ(event->GetValue(), expected_event.GetValue());
  ASSERT_EQ(ring_buffer->GetCursor(), 0);
}

TEST_F(RingBufferTest, should_claim_and_get_multiple_messages) {
  StubEventTranslator translator;
  int num_messages = ring_buffer->GetBufferSize();
  for(int i = 0; i < num_messages; i++)
    ring_buffer->PublishEvent(&translator, i);

  int64_t expected_sequence = num_messages - 1;
  int64_t available = sequence_barrier->WaitFor(expected_sequence);
  ASSERT_EQ(expected_sequence, available);

  for(int i = 0; i < num_messages; i++)
    ASSERT_EQ(ring_buffer->Get(i)->GetValue(), i);
}
