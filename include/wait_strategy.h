
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

  virtual int64_t WaitFor(int64_t sequence, SequencePtr cursor, SequencePtr dependent_sequence,
                          SequenceBarrier* barrier) throw (AlertException, TimeoutException) = 0;
  virtual void SignalAllWhenBlocking() = 0;
};

} //end namespace

#endif //WAIT_STRATEGY_H_
