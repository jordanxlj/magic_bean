
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

#ifndef EVENT_TRANSLATOR_ONE_ARG_H_
#define EVENT_TRANSLATOR_ONE_ARG_H_

namespace magic_bean {

template<typename T, typename A>
class EventTranslatorOneArg {
 public:
  virtual ~EventTranslatorOneArg() {};
  virtual void TranslateTo(T* event, int64_t sequence, A arg0) = 0;
};

} //end namespace

#endif //EVENT_TRANSLATOR_ONE_ARG_H_
