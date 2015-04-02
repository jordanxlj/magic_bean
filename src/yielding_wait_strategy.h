#ifndef YIELDING_WAIT_STRATEGY_H_
#define YIELDING_WAIT_STRATEGY_H_

#include <wait_strategy.h>

namespace magic_bean {

class YieldingWaitStrategy : public WaitStrategy {
 public:
  YieldingWaitStrategy();
  ~YieldingWaitStrategy();

  virtual int64_t WaitFor(int64_t sequence, SequencePtr cursor, SequencePtr dependent_sequence,
                          SequenceBarrier* barrier) throw (AlertException, TimeoutException) override;
  virtual void SignalAllWhenBlocking() override;
 private:
  int ApplyWaitMethod(SequenceBarrier* barrier, int counter) throw (AlertException);
};

} //end namespace

#endif //YIELDING_WAIT_STRATEGY_H_
