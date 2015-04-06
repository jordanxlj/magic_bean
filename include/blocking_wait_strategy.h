#ifndef BLOCKING_WAIT_STRATEGY_H_
#define BLOCKING_WAIT_STRATEGY_H_

#include <mutex>
#include <condition_variable>
#include "magic_types.h"
#include "wait_strategy.h"

namespace magic_bean {

class BlockingWaitStrategy : public WaitStrategy {
 public:
  BlockingWaitStrategy();
  ~BlockingWaitStrategy();

  virtual int64_t WaitFor(int64_t sequence, SequencePtr cursor,
                          SequencePtr dependent_sequence, SequenceBarrier* barrier)
    throw (AlertException, TimeoutException) override;
  virtual void SignalAllWhenBlocking() override;
 private:
  std::recursive_mutex mutex_;
  std::condition_variable_any cond_;
};

} //end namespace

#endif //BLOCKING_WAIT_STRATEGY_H_
