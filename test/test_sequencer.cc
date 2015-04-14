#include "single_producer_sequencer.h"
#include "multi_producer_sequencer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "blocking_wait_strategy.h"
#include "sequence.h"

using namespace magic_bean;

enum ProducerType {
  SINGLE,
  MULTI
};

class SequencerTest : public ::testing::TestWithParam<int> {
 protected:
  SequencerTest() {
    producer_type = GetParam();
  }
  virtual ~SequencerTest() {};

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
  WaitStrategy* wait_strategy;
  SequencePtr gating_sequence;
  static const int BUFFER_SIZE = 16;
};

TEST_P(SequencerTest, should_start_with_initial_value) {
  ASSERT_EQ(0, sequencer->Next());
}

TEST_P(SequencerTest, should_batch_claim) {
  ASSERT_EQ(3, sequencer->Next(4));
}

TEST_P(SequencerTest, should_indicate_has_available_capacity) {
  std::vector<SequencePtr> gating_sequences;
  gating_sequences.push_back(gating_sequence);
  sequencer->AddGatingSequences(gating_sequences);

  ASSERT_TRUE(sequencer->HasAvailableCapacity(1));
  ASSERT_TRUE(sequencer->HasAvailableCapacity(BUFFER_SIZE));
  ASSERT_FALSE(sequencer->HasAvailableCapacity(BUFFER_SIZE + 1));

  sequencer->Publish(sequencer->Next());
  ASSERT_TRUE(sequencer->HasAvailableCapacity(BUFFER_SIZE - 1));
  ASSERT_FALSE(sequencer->HasAvailableCapacity(BUFFER_SIZE));
}

INSTANTIATE_TEST_CASE_P(AllProducerSequencerTest, SequencerTest, ::testing::Range(0, 2));
