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

#include "fixed_sequence_group.h"
#include <limits>
#include <stdexcept>
#include "util.h"

namespace magic_bean {

FixedSequenceGroup::FixedSequenceGroup(const std::vector<SequencePtr>& sequences)
  : sequences_(sequences) {}

FixedSequenceGroup::~FixedSequenceGroup() {}

int64_t FixedSequenceGroup::Get() const {
  int64_t minimum = std::numeric_limits<int64_t>::max();
  return Util::GetMinimumSequence(sequences_, minimum);
}

void FixedSequenceGroup::Set(int64_t value) {
  throw std::runtime_error("not implemented");
}

bool FixedSequenceGroup::CompareAndSet(int64_t expected_value, int64_t new_value) {
  throw std::runtime_error("not implemented");
}

int64_t FixedSequenceGroup::IncrementAndGet() {
  throw std::runtime_error("not implemented");
}

int64_t FixedSequenceGroup::AddAndGet(int64_t increment) {
  throw std::runtime_error("not implemented");
}

} //end namespace
