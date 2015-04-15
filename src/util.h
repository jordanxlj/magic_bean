
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

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include "magic_types.h"

namespace magic_bean {

class Util {
 public:
  static int64_t GetMinimumSequence(const std::vector<SequencePtr>& sequences, int64_t minimum);
  static int Log2(int i);
};

} //end namespace

#endif //UTIL_H_
