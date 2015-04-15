
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

#ifndef SINGLE_PRODUCER_SEQUENCER_H_
#define SINGLE_PRODUCER_SEQUENCER_H_

#include "abstract_sequencer.h"

namespace magic_bean {

class InsufficientCapacityException;

class SingleProducerSequencerPad : public AbstractSequencer {
 public:
  SingleProducerSequencerPad(int buffer_size, WaitStrategy* wait_strategy);
  virtual ~SingleProducerSequencerPad() {};

 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

class SingleProducerSequencerFields : public SingleProducerSequencerPad {
 public:
  SingleProducerSequencerFields(int buffer_size, WaitStrategy* wait_strategy);
 protected:
  int64_t next_value_;
  int64_t cached_value_;
};

class SingleProducerSequencer : public SingleProducerSequencerFields {
 public:
  explicit SingleProducerSequencer(int buffer_size, WaitStrategy* wait_strategy);
  ~SingleProducerSequencer();

  virtual bool HasAvailableCapacity(int required_capacity) override;

  virtual int64_t Next() override;
  virtual int64_t Next(int n) override;

  virtual int64_t TryNext() throw (InsufficientCapacityException) override;
  virtual int64_t TryNext(int n) throw (InsufficientCapacityException) override;

  virtual int64_t RemainingCapacity() const override;

  virtual void Claim(int64_t sequence) override;
  virtual void Publish(int64_t sequence) override;
  virtual void Publish(int64_t lo, int64_t hi) override;

  virtual bool IsAvailable(int64_t sequence) override;
  virtual int64_t GetHighestPublishedSequence(int64_t next_sequence,
                                              int64_t available_sequence) override;
 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

} //end namespace

#endif //SINGLE_PRODUCER_SEQUENCER_H_
