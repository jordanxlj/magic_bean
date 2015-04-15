
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

#ifndef PROCESSING_SEQUENCE_BARRIER_H_
#define PROCESSING_SEQUENCE_BARRIER_H_

#include <vector>
#include <magic_types.h>
#include <sequence_barrier.h>

namespace magic_bean {

class Sequencer;
class WaitStrategy;

class ProcessingSequenceBarrier : public SequenceBarrier {
 public:
  explicit ProcessingSequenceBarrier(Sequencer* sequencer, WaitStrategy* wait_strategy,
                                     SequencePtr cursor_sequence,
                                     const std::vector<SequencePtr>& dependent_sequences);
  ~ProcessingSequenceBarrier();

  virtual int64_t WaitFor(int64_t sequence) throw (AlertException, TimeoutException) override;
  virtual int64_t GetCursor() const override;

  virtual bool IsAlerted() const override;
  virtual void Alert() override;
  virtual void ClearAlert() override;
  virtual void CheckAlert() throw (AlertException) override;
 private:
  Sequencer* sequencer_;
  WaitStrategy* wait_strategy_;
  SequencePtr cursor_sequence_;
  SequencePtr dependent_sequence_;
  volatile bool alerted_;
};

} //end namespace

#endif //PROCESSING_SEQUENCE_BARRIER_H_
