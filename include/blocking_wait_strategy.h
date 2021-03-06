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
