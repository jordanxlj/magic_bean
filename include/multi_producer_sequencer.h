
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

#ifndef MULTI_PRODUCER_SEQUENCER_H_
#define MULTI_PRODUCER_SEQUENCER_H_

#include "abstract_sequencer.h"

namespace magic_bean {

class InsufficientCapacityException;

class MultiProducerSequencer : public AbstractSequencer {
 public:
  explicit MultiProducerSequencer(int buffer_size, WaitStrategy* wait_strategy);
  ~MultiProducerSequencer();

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
 private:
  bool HasAvailableCapacity(int required_capacity, int64_t cursor_value);
  void InitializeAvailableBuffer();
  void SetAvailable(int64_t sequence);
  void SetAvailableBufferValue(int index, int flag);
  int CalculateAvailabilityFlag(int64_t sequence);
  int CalculateIndex(int64_t sequence);

 private:
  SequencePtr gating_sequence_cache_;
  int* available_buffer_;
  int index_mask_;
  int index_shift_;
};

} //end namespace

#endif //MULTI_PRODUCER_SEQUENCER_H_
