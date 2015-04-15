
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

#ifndef EVENT_PROCESSOR_H_
#define EVENT_PROCESSOR_H_

#include "magic_types.h"

namespace magic_bean {

class EventProcessor {
 public:
  virtual ~EventProcessor() {};

  virtual SequencePtr GetSequence() = 0;
  virtual void Halt() = 0;
  virtual bool IsRunning() = 0;
  virtual void Run() = 0;
};

} //end namespace

#endif //EVENT_PROCESSOR_H_
