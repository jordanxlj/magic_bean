
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

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include <vector>
#include "cursored.h"
#include "magic_types.h"
#include "sequence.h"
#include "sequenced.h"

namespace magic_bean {

class SequenceBarrier;

class Sequencer : public Cursored, public Sequenced {
 public:
  virtual ~Sequencer() {};

  virtual void Claim(int64_t sequence) = 0;
  virtual bool IsAvailable(int64_t sequence) = 0;

  virtual void AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) = 0;
  virtual bool RemoveGatingSequence(SequencePtr gating_sequence) = 0;

  virtual SequenceBarrier* NewBarrier(const std::vector<SequencePtr>& sequences_to_track) = 0;
  virtual int64_t GetMinimumSequence() const = 0;
  virtual int64_t GetHighestPublishedSequence(int64_t next_sequence, int64_t available_sequence) = 0;
};

} //end namespace

#endif //SEQUENCER_H_
