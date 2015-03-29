#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "event_poller.h"
#include "sequence.h"

using namespace magic_bean;

using ::testing::Return;

class MockDataProvider : public DataProvider<int64_t> {
 public:
  MOCK_METHOD1(Get, int64_t*(int64_t));
};

class MockHandler : public Handler<int64_t> {
 public:
  MOCK_METHOD3(OnEvent, bool(int64_t*, int64_t, bool));
};

class MockSequencer : public Sequencer {
 public:
  MockSequencer(){};

  MOCK_METHOD0(GetCursor, int64_t());
  MOCK_CONST_METHOD0(GetBufferSize, int());
  MOCK_CONST_METHOD1(HasAvailableCapacity, bool(int));
  MOCK_CONST_METHOD0(RemainingCapacity, int64_t());

  MOCK_METHOD0(Next, int64_t());
  MOCK_METHOD1(Next, int64_t(int));

  virtual int64_t TryNext() throw (InsufficientCapacityException) override {
    return 0;
  }

  virtual int64_t TryNext(int) throw (InsufficientCapacityException) override {
    return 0;
  }

  MOCK_METHOD1(Publish, void(int64_t));
  MOCK_METHOD2(Publish, void(int64_t, int64_t));
  MOCK_METHOD1(Claim, void(int64_t));
  MOCK_METHOD1(IsAvailable, bool(int64_t));

  MOCK_METHOD1(AddGatingSequences, void(const std::vector<SequencePtr>&));
  MOCK_METHOD1(RemoveGatingSequence, bool(SequencePtr));
  MOCK_CONST_METHOD0(GetMinimumSequence, int64_t());
  MOCK_METHOD2(GetHighestPublishedSequence, int64_t(int64_t, int64_t));
};

TEST(test_event_poller, should_poll_for_events) {
  MockSequencer* sequencer = new MockSequencer();
  EXPECT_CALL(*sequencer, GetCursor())
    .Times(3)
    .WillOnce(Return(-1))
    .WillOnce(Return(0))
    .WillOnce(Return(0));

  EXPECT_CALL(*sequencer, GetHighestPublishedSequence(0, -1))
    .WillOnce(Return(-1));

  EXPECT_CALL(*sequencer, GetHighestPublishedSequence(0, 0))
    .WillOnce(Return(0));

  int64_t* event = new int64_t(100);
  MockDataProvider* provider = new MockDataProvider();
  EXPECT_CALL(*provider, Get(0))
    .WillOnce(Return(event));

  SequencePtr poll_sequence = SequencePtr(new Sequence());
  SequencePtr buffer_sequence = SequencePtr(new Sequence());
  SequencePtr gating_sequence = SequencePtr(new Sequence());
  std::vector<SequencePtr> gating_sequences;
  gating_sequences.push_back(gating_sequence);
  EventPoller<int64_t>* poller = EventPoller<int64_t>::NewInstance(provider, sequencer,
                                                                  poll_sequence, buffer_sequence,
                                                                  gating_sequences);

  MockHandler* handler = new MockHandler();
  //Initial State - nothing published
  ASSERT_EQ(poller->Poll(handler), PollState::IDLE);

  //Publish Event
  buffer_sequence->IncrementAndGet();
  ASSERT_EQ(poller->Poll(handler), PollState::GATING);

  gating_sequence->IncrementAndGet();
  ASSERT_EQ(poller->Poll(handler), PollState::PROCESSING);
}
