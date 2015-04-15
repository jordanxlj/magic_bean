#include "single_producer_sequencer.h"
#include "multi_producer_sequencer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>
#include <thread>
#include <condition_variable>
#include "blocking_wait_strategy.h"
#include "sequence.h"

using namespace magic_bean;

enum ProducerType {
  SINGLE,
  MULTI
};

class SequencerTest : public ::testing::TestWithParam<int> {
 public:
  void Execute() {
    {
      std::unique_lock<std::mutex> waiting_lock(waiting_mutex);
      waiting_cond.notify_all();
    }
    int64_t next = sequencer->Next();
    sequencer->Publish(next);
    {
      std::unique_lock<std::mutex> done_lock(done_mutex);
      done_cond.notify_all();
    }
  }
 protected:
  SequencerTest() {
    producer_type = GetParam();
  }
  virtual ~SequencerTest() {};

  virtual void SetUp() {
    wait_strategy = new BlockingWaitStrategy();
    sequencer = NewProducer(producer_type, wait_strategy);
    gating_sequence = SequencePtr(new Sequence());
    gating_sequences.push_back(gating_sequence);
  }
  virtual void TearDown() {
    gating_sequences.clear();
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
  std::vector<SequencePtr> gating_sequences;

  std::condition_variable waiting_cond;
  std::mutex waiting_mutex;
  std::condition_variable done_cond;
  std::mutex done_mutex;

  static const int BUFFER_SIZE = 16;
  static const int64_t INITIAL_CURSOR_VALUE = -1;
};

TEST_P(SequencerTest, should_start_with_initial_value) {
  ASSERT_EQ(0, sequencer->Next());
}

TEST_P(SequencerTest, should_batch_claim) {
  ASSERT_EQ(3, sequencer->Next(4));
}

TEST_P(SequencerTest, should_indicate_has_available_capacity) {
  sequencer->AddGatingSequences(gating_sequences);

  ASSERT_TRUE(sequencer->HasAvailableCapacity(1));
  ASSERT_TRUE(sequencer->HasAvailableCapacity(BUFFER_SIZE));
  ASSERT_FALSE(sequencer->HasAvailableCapacity(BUFFER_SIZE + 1));

  sequencer->Publish(sequencer->Next());
  ASSERT_TRUE(sequencer->HasAvailableCapacity(BUFFER_SIZE - 1));
  ASSERT_FALSE(sequencer->HasAvailableCapacity(BUFFER_SIZE));
}

TEST_P(SequencerTest, should_indicate_no_available_capacity) {
  sequencer->AddGatingSequences(gating_sequences);

  int64_t sequence = sequencer->Next(16);
  sequencer->Publish(sequence - (BUFFER_SIZE - 1), sequence);
  ASSERT_FALSE(sequencer->HasAvailableCapacity(1));
}

TEST_P(SequencerTest, should_holdup_publisher_when_buffer_is_full) {
  sequencer->AddGatingSequences(gating_sequences);

  int64_t sequence = sequencer->Next(BUFFER_SIZE);
  sequencer->Publish(sequence - (BUFFER_SIZE - 1), sequence);

  std::unique_lock<std::mutex> waiting_lock(waiting_mutex);
  std::unique_lock<std::mutex> done_lock(done_mutex);

  int64_t expected_full_sequence = INITIAL_CURSOR_VALUE + sequencer->GetBufferSize();
  ASSERT_EQ(sequencer->GetCursor(), expected_full_sequence);

  std::thread thread(std::bind(&SequencerTest::Execute, this));

  waiting_cond.wait(waiting_lock);
  ASSERT_EQ(sequencer->GetCursor(), expected_full_sequence);
  gating_sequence->Set(INITIAL_CURSOR_VALUE + 1);
  done_cond.wait(done_lock);

  ASSERT_EQ(sequencer->GetCursor(), expected_full_sequence + 1);
  thread.join();
}

TEST_P(SequencerTest, should_throw_insufficient_capacity_exception_when_sequencer_is_full) {
  sequencer->AddGatingSequences(gating_sequences);

  for(int i = 0; i < BUFFER_SIZE; i++) {
    sequencer->Next();
  }

  try {
    sequencer->TryNext();
  } catch(InsufficientCapacityException& ex) {
    SUCCEED();
  } catch(...) {
    FAIL() << "Unexpected exception";
  }
}

TEST_P(SequencerTest, should_calculate_remaining_capacity) {
  sequencer->AddGatingSequences(gating_sequences);
  ASSERT_EQ(sequencer->RemainingCapacity(), int64_t(BUFFER_SIZE));

  for(int i = 1; i < BUFFER_SIZE; i++) {
    sequencer->Next();
    ASSERT_EQ(sequencer->RemainingCapacity(), BUFFER_SIZE - i);
  }
}

INSTANTIATE_TEST_CASE_P(AllProducerSequencerTest, SequencerTest, ::testing::Range(0, 2));
