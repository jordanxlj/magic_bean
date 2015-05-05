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

#ifndef ABSTRACT_SEQUENCER_H_
#define ABSTRACT_SEQUENCER_H_

#include <vector>
#include "event_poller.h"
#include "magic_types.h"
#include "sequencer.h"

namespace magic_bean {

class WaitStrategy;
class SequenceGroups;

class AbstractSequencer : public Sequencer {
 public:
  explicit AbstractSequencer(int buffer_size, WaitStrategy* wait_strategy);
  virtual ~AbstractSequencer();

  virtual int64_t GetCursor() override;
  virtual int GetBufferSize() const override;

  virtual void AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) override;
  virtual bool RemoveGatingSequence(SequencePtr gating_sequence) override;
  virtual SequenceBarrier* NewBarrier(const std::vector<SequencePtr>& sequences_to_track) override;

  virtual int64_t GetMinimumSequence() const override;
  int64_t GetMinimumSequence(int64_t minimum) const;

  template<typename T>
    EventPoller<T>* NewPoller(DataProvider<T>* provider,
                              const std::vector<SequencePtr>& gating_sequences);
 protected:
  int buffer_size_;
  WaitStrategy* wait_strategy_;
  SequencePtr cursor_;
  SequenceGroups* sequence_groups_;
};

template<typename T>
  EventPoller<T>* AbstractSequencer::NewPoller(DataProvider<T>* provider, const std::vector<SequencePtr>& gating_sequences) {
  return EventPoller<T>::NewInstance(provider, static_cast<Sequencer*>(this),
                                     SequencePtr(new Sequence), cursor_, gating_sequences);
}

} //end namespace

#endif //ABSTRACT_SEQUENCER_H_
