
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

#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

#include <cstdint>

namespace magic_bean {

template<typename T>
class EventHandler {
 public:
  virtual ~EventHandler() {};
  virtual void OnEvent(T* event, int64_t sequence, bool end_of_batch) = 0;
};

} //end namespace

#endif //EVENT_HANDLER_H_
