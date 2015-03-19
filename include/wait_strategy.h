#ifndef WAIT_STRATEGY_H_
#define WAIT_STRATEGY_H_

#include <cstdint>

namespace magic_bean {

class Sequence;
class SequenceBarrier;

class WaitStrategy{
 public:
  virtual ~WaitStrategy() {};

  virtual int64_t WaitFor(int64_t sequence, Sequence& cursor, Sequence dependent_sequence, SequenceBarrier& barrier) = 0;
  virtual void SignalAllWhenBlocking() = 0;
};

} //end namespace

#endif //WAIT_STRATEGY_H_
