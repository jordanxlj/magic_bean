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
};

TEST_F(RingBufferTest, should_claim_and_get) {
}
