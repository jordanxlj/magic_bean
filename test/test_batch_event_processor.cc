#include "batch_event_processor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ring_buffer.h"
#include "sequence_barrier.h"
#include "support/stub_event.h"

using namespace magic_bean;

class BatchEventProcessorTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    event_factory = new StubEventFactory;
    ring_buffer = RingBuffer<StubEvent>::CreateSingleProducer(event_factory, 16);
  }
  virtual void TearDown() {
    delete ring_buffer;
    delete event_factory;
  }

 protected:
  EventFactory<StubEvent>* event_factory;
  RingBuffer<StubEvent>* ring_buffer;
  SequenceBarrier* sequence_barrier;
  EventHandler<StubEvent>* event_handler;
  BatchEventProcessor<StubEvent>* batch_event_processor;
};
