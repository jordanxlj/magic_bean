#include "single_producer_sequencer.h"
#include "multi_producer_sequencer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "blocking_wait_strategy.h"
#include "sequence.h"

using namespace magic_bean;
using namespace ::testing;

enum ProducerType {
  SINGLE,
  MULTI
};

class SequencerTest : public TestWithParam<int> {
 protected:
  SequencerTest() {
    producer_type = GetParam();
  }
  virtual ~SequencerTest() {};
 protected:
  virtual void SetUp() {
    wait_strategy = new BlockingWaitStrategy();
    sequencer = NewProducer(producer_type, wait_strategy);
    gating_sequence = SequencePtr(new Sequence());
  }
  virtual void TearDown() {
    gating_sequence.reset();
    delete sequencer;
    delete wait_strategy;
  }

 private:
  Sequencer* NewProducer(int producer_type, WaitStrategy* strategy) {
    if(producer_type == ProducerType::SINGLE)
      return new SingleProducerSequencer(BUFFER_SIZE, strategy);
    else if(producer_type == ProducerType::MULTI)
      return new MultiProducerSequencer(BUFFER_SIZE, strategy);
    return nullptr;
  }
 protected:
  int producer_type;
  Sequencer* sequencer;
  SequencePtr gating_sequence;
  WaitStrategy* wait_strategy;
 private:
  static const int BUFFER_SIZE = 16;
};

INSTANTIATE_TEST_CASE_P(AllProducerSequencerTest, SequencerTest, Range(1, 2));
