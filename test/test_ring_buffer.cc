#include "ring_buffer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>
#include <thread>
#include <condition_variable>
#include "insufficient_capacity_exception.h"
#include "no_op_event_processor.h"
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

    event_processor = new NoOpEventProcessor(ring_buffer);
    gating_sequences.push_back(event_processor->GetSequence());
    ring_buffer->AddGatingSequences(gating_sequences);
  }
  virtual void TearDown() {
    ring_buffer->RemoveGatingSequence(event_processor->GetSequence());
    gating_sequences.clear();
    delete event_processor;
    delete sequence_barrier;
    delete ring_buffer;
    delete event_factory;
  }

 protected:
  EventFactory<StubEvent>* event_factory;
  RingBuffer<StubEvent>* ring_buffer;
  SequenceBarrier* sequence_barrier;

  EventProcessor* event_processor;
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

TEST_F(RingBufferTest, should_wrap) {
  StubEventTranslator translator;
  int num_messages = ring_buffer->GetBufferSize();
  int offset = 1000;
  for(int i = 0; i < num_messages + offset; i++) {
    ring_buffer->PublishEvent(&translator, i);
  }

  int64_t expected_sequence = num_messages + offset - 1;
  int64_t available = sequence_barrier->WaitFor(expected_sequence);
  ASSERT_EQ(expected_sequence, available);

  for(int i = offset; i < num_messages + offset; i++) {
    ASSERT_EQ(ring_buffer->Get(i)->GetValue(), i);
  }
}

TEST(RingBufferTestSuite, should_prevent_wrapping) {
  StubEventFactory factory;
  RingBuffer<StubEvent>* rb = RingBuffer<StubEvent>::CreateMultiProducer(&factory, 4);

  SequencePtr sequence = SequencePtr(new Sequence);
  std::vector<SequencePtr> gating_sequences;
  gating_sequences.push_back(sequence);
  rb->AddGatingSequences(gating_sequences);

  StubEventTranslator translator;
  for(int i = 0; i < 4; i++)
    rb->PublishEvent(&translator, i);

  ASSERT_FALSE(rb->TryPublishEvent(&translator, 3));
  sequence.reset();
  delete rb;
}

TEST(RingBufferTestSuite, should_throw_exception_if_buffer_is_full) {
  StubEventFactory factory;
  RingBuffer<StubEvent>* rb = RingBuffer<StubEvent>::CreateMultiProducer(&factory, 32);

  SequencePtr sequence = SequencePtr(new Sequence(rb->GetBufferSize()));
  std::vector<SequencePtr> gating_sequences;
  gating_sequences.push_back(sequence);
  rb->AddGatingSequences(gating_sequences);

  try {
    for(int i = 0; i < rb->GetBufferSize(); i++)
      rb->Publish(rb->TryNext());
  } catch(...) {
    FAIL() << "should not of thrown exception";
  }

  try {
    rb->TryNext();
    FAIL() << "Exception should have been thrown";
  } catch(InsufficientCapacityException& ex) {
    SUCCEED();
  }

  delete rb;
}
