
/*
 * Copyright 2015 jordanxlj
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
