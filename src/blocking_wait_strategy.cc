#include "blocking_wait_strategy.h"
#include "sequence.h"
#include "sequence_barrier.h"

namespace magic_bean {

BlockingWaitStrategy::BlockingWaitStrategy() {}

BlockingWaitStrategy::~BlockingWaitStrategy() {}

int64_t BlockingWaitStrategy::WaitFor(int64_t sequence, SequencePtr cursor_sequence,
                                      SequencePtr dependent_sequence, SequenceBarrier* barrier)
  throw (AlertException, TimeoutException) {
  int64_t available_sequence;
  if((available_sequence = cursor_sequence->Get()) < sequence) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    while((available_sequence = cursor_sequence->Get()) < sequence) {
      barrier->CheckAlert();
      cond_.wait(lock);
    }
  }

  while((available_sequence = dependent_sequence->Get()) < sequence)
    barrier->CheckAlert();

  return available_sequence;
}

void BlockingWaitStrategy::SignalAllWhenBlocking() {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  cond_.notify_all();
}

} //end namespace
