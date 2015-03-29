#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "event_poller.h"
#include "event_handler.h"
#include "sequence.h"

using namespace magic_bean;

using ::testing::Return;

class MockDataProvider : public DataProvider<int64_t> {
 public:
  MOCK_METHOD1(Get, int64_t*(int64_t));
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

}
