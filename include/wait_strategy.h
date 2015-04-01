#ifndef WAIT_STRATEGY_H_
#define WAIT_STRATEGY_H_

#include "alert_exception.h"
#include "magic_types.h"
#include "timeout_exception.h"

namespace magic_bean {

class SequenceBarrier;

class WaitStrategy{
 public:
  virtual ~WaitStrategy() {};

  virtual int64_t WaitFor(int64_t sequence, SequencePtr cursor, Sequence dependent_sequence,
                          SequenceBarrier* barrier) throw (AlertException, TimeoutException) = 0;
  virtual void SignalAllWhenBlocking() = 0;
};

} //end namespace

#endif //WAIT_STRATEGY_H_
