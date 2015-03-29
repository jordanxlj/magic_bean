#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "event_poller.h"
#include "event_handler.h"
#include "sequence.h"

using namespace magic_bean;

class MockDataProvider : public DataProvider<int64_t> {
 public:
  MOCK_METHOD1(Get, int64_t*(int64_t));
};

TEST(test_event_poller, should_poll_for_events) {
}
