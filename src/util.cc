
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

#include "util.h"
#include "sequence.h"

namespace magic_bean {

int64_t Util::GetMinimumSequence(const std::vector<SequencePtr>& sequences, int64_t minimum) {
  for(size_t i = 0; i < sequences.size(); i++) {
    int64_t value = sequences[i]->Get();
    minimum = value < minimum ? value : minimum;
  }
  return minimum;
}

int Util::Log2(int i) {
  int r = 0;
  while((i >>= 1) != 0)
    ++r;
  return r;
}

} //end namespace
