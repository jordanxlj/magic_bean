#include "yielding_wait_strategy.h"
#include <thread>
#include <sequence.h>
#include <sequence_barrier.h>

namespace magic_bean {

static const int SPIN_TRIES = 200;

YieldingWaitStrategy::YieldingWaitStrategy() {}

YieldingWaitStrategy::~YieldingWaitStrategy() {}

int64_t YieldingWaitStrategy::WaitFor(int64_t sequence, SequencePtr cursor,
                                      SequencePtr dependent_sequence,
                                      SequenceBarrier* barrier)
  throw (AlertException, TimeoutException) {
  int64_t available_sequence;
  int counter = SPIN_TRIES;

  while((available_sequence = dependent_sequence->Get()) < sequence) {
    counter = ApplyWaitMethod(barrier, counter);
  }
  return available_sequence;
}

void YieldingWaitStrategy::SignalAllWhenBlocking() {}

int YieldingWaitStrategy::ApplyWaitMethod(SequenceBarrier* barrier, int counter)
  throw (AlertException) {
  barrier->CheckAlert();

  if(counter == 0) {
    std::this_thread::yield();
  } else {
    counter--;
  }
  return counter;
}

} //end namespace
