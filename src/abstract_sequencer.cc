
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

#include "abstract_sequencer.h"
#include <wait_strategy.h>
#include "processing_sequence_barrier.h"
#include "sequence_groups.h"

namespace magic_bean {

AbstractSequencer::AbstractSequencer(int buffer_size, WaitStrategy* wait_strategy)
  : buffer_size_(buffer_size)
  , wait_strategy_(wait_strategy)
  , cursor_(new Sequence)
  , sequence_groups_(new SequenceGroups) {}

AbstractSequencer::~AbstractSequencer() {
  cursor_.reset();
}

int64_t AbstractSequencer::GetCursor() {
  return cursor_->Get();
}

int AbstractSequencer::GetBufferSize() const {
  return buffer_size_;
}

void AbstractSequencer::AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) {
  sequence_groups_->AddSequences(this, gating_sequences);
}

bool AbstractSequencer::RemoveGatingSequence(SequencePtr gating_sequence) {
  return sequence_groups_->RemoveSequence(gating_sequence);
}

SequenceBarrier* AbstractSequencer::NewBarrier(const std::vector<SequencePtr>& sequences_to_track) {
  return new ProcessingSequenceBarrier(this, wait_strategy_, cursor_, sequences_to_track);
}

int64_t AbstractSequencer::GetMinimumSequence() const {
  return sequence_groups_->GetMinimumSequence(cursor_->Get());
}

int64_t AbstractSequencer::GetMinimumSequence(int64_t minimum) const {
  return sequence_groups_->GetMinimumSequence(minimum);
}

template<typename T>
EventPoller<T> AbstractSequencer::NewPoller(DataProvider<T>* provider,
                                            const std::vector<SequencePtr>& gating_sequences) {
  return EventPoller<T>::NewInstance(provider, this, new Sequence, cursor_, gating_sequences);
}

} //end namespace
