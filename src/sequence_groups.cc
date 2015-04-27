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

#include "sequence_groups.h"
#include "cursored.h"
#include "sequence.h"
#include <algorithm>

namespace magic_bean {

SequenceGroups::SequenceGroups() {}

SequenceGroups::~SequenceGroups() {
  gating_sequences_.clear();
}

void SequenceGroups::AddSequences(Cursored* cursor,
                                  const std::vector<SequencePtr>& sequences_to_add) {
  for(auto sequence_to_add : sequences_to_add)
    AddSequence(cursor, sequence_to_add);
}

void SequenceGroups::AddSequence(Cursored* cursor, SequencePtr sequence) {
  int64_t cursor_sequence = cursor->GetCursor();
  sequence->Set(cursor_sequence);
  gating_sequences_.push_back(sequence);
}

bool SequenceGroups::RemoveSequence(SequencePtr sequence) {
  auto for_removing = std::remove_if(gating_sequences_.begin(), gating_sequences_.end(),
                                     [&](SequencePtr gating_sequence) {
                                       return sequence == gating_sequence;
                                     });
  if(for_removing != gating_sequences_.end()) {
    gating_sequences_.erase(for_removing, gating_sequences_.end());
    return true;
  } else
    return false;
}

int64_t SequenceGroups::GetMinimumSequence(int64_t minimum) {
  int64_t minimum_value = minimum;
  for(auto gating_sequence : gating_sequences_) {
    if(gating_sequence->Get() < minimum_value)
      minimum_value = gating_sequence->Get();
  }

  return minimum_value;
}

} //end namespace
