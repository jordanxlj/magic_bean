#include "single_producer_sequencer.h"
#include "multi_producer_sequencer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>
#include <thread>
#include <condition_variable>
#include "blocking_wait_strategy.h"
#include "sequence.h"
#include "sequence_barrier.h"

using namespace magic_bean;

enum ProducerType {
  SINGLE,
  MULTI
};

class MockWaitStrategy : public WaitStrategy {
 public:
  virtual int64_t WaitFor(int64_t sequence, SequencePtr cursor, SequencePtr dependent_sequence,
                          SequenceBarrier* barrier) throw (AlertException, TimeoutException) override {
    return 0;
  }

  MOCK_METHOD0(SignalAllWhenBlocking, void());
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

TEST_P(SequencerTest, should_not_be_available_until_published) {
  int64_t next = sequencer->Next(6);
  for(int i = 0; i <= 5; i++)
    ASSERT_FALSE(sequencer->IsAvailable(i));

  sequencer->Publish(next - (6 - 1), next);
  for(int i = 0; i <= 5; i++)
    ASSERT_TRUE(sequencer->IsAvailable(i));
  ASSERT_FALSE(sequencer->IsAvailable(6));
}

TEST_P(SequencerTest, should_notify_wait_strategy_on_publish) {
  MockWaitStrategy* wait_strategy = new MockWaitStrategy;
  Sequenced* sequencer = NewProducer(producer_type, wait_strategy);

  EXPECT_CALL(*wait_strategy, SignalAllWhenBlocking());
  sequencer->Publish(sequencer->Next());
  delete sequencer;
  delete wait_strategy;
}

TEST_P(SequencerTest, should_notify_wait_strategy_on_publish_batch) {
  MockWaitStrategy* wait_strategy = new MockWaitStrategy;
  Sequenced* sequencer = NewProducer(producer_type, wait_strategy);

  EXPECT_CALL(*wait_strategy, SignalAllWhenBlocking());
  int64_t next = sequencer->Next(4);
  sequencer->Publish(next - (4 - 1), next);
  delete sequencer;
  delete wait_strategy;
}

TEST_P(SequencerTest, should_wait_on_publication) {
  std::vector<SequencePtr> sequences_to_track;
  SequenceBarrier* barrier = sequencer->NewBarrier(sequences_to_track);
  int64_t next = sequencer->Next(10);
  int64_t lo = next - (10 - 1);
  int64_t mid = next - 5;

  for(int64_t l = lo; l < mid; l++)
    sequencer->Publish(l);

  ASSERT_EQ(barrier->WaitFor(-1), mid - 1);

  for(int64_t l = mid; l <= next; l++)
    sequencer->Publish(l);

  ASSERT_EQ(barrier->WaitFor(-1), next);
  delete barrier;
}

TEST_P(SequencerTest, should_try_next) {
  sequencer->AddGatingSequences(gating_sequences);
  for(int i = 0; i < BUFFER_SIZE; i++)
    sequencer->Publish(sequencer->TryNext());

  try {
    sequencer->TryNext();
  } catch(InsufficientCapacityException& ex) {
    SUCCEED();
  }
}

TEST_P(SequencerTest, should_claim_specific_sequence) {
  int64_t sequence = 14;
  sequencer->Claim(sequence);
  sequencer->Publish(sequence);
  ASSERT_EQ(sequencer->Next(), sequence + 1);
}

INSTANTIATE_TEST_CASE_P(AllProducerSequencerTest, SequencerTest, ::testing::Range(0, 2));
