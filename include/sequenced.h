
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

#ifndef SEQUENCED_H_
#define SEQUENCED_H_

#include <cstdint>
#include "insufficient_capacity_exception.h"

namespace magic_bean {

class Sequenced {
 public:
  virtual ~Sequenced() {};

  virtual int GetBufferSize() const = 0;
  virtual bool HasAvailableCapacity(int required_capacity) = 0;
  virtual int64_t RemainingCapacity() const = 0;

  virtual int64_t Next() = 0;
  virtual int64_t Next(int n) = 0;

  virtual int64_t TryNext() throw (InsufficientCapacityException) = 0;
  virtual int64_t TryNext(int n) throw (InsufficientCapacityException) = 0;

  virtual void Publish(int64_t sequence) = 0;
  virtual void Publish(int64_t lo, int64_t hi) = 0;
};

} //end namespace

#endif //SEQUENCED_H_
