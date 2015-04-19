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

#ifndef NO_OP_EVENT_PROCESSOR_H_
#define NO_OP_EVENT_PROCESSOR_H_

#include <atomic>
#include "magic_types.h"
#include "event_processor.h"
#include "ring_buffer.h"

namespace magic_bean {

template<typename T>
class SequencerFollowingSequence : public Sequence {
 public:
  SequencerFollowingSequence(RingBuffer<T>* sequencer);
  virtual int64_t Get() const override;
 private:
  RingBuffer<T>* sequencer_;
};

template<typename T>
  SequencerFollowingSequence<T>::SequencerFollowingSequence(RingBuffer<T>* sequencer)
    : sequencer_(sequencer) {
}

template<typename T>
  int64_t SequencerFollowingSequence<T>::Get() const {
  return sequencer_->GetCursor();
}

class NoOpEventProcessor : public EventProcessor {
 public:
  template<typename T>
    NoOpEventProcessor(RingBuffer<T>* ring_buffer);
  ~NoOpEventProcessor();

  virtual SequencePtr GetSequence() override;
  virtual void Halt() override;
  virtual bool IsRunning() override;
  virtual void Run() override;
 private:
  SequencePtr sequence_;
  std::atomic<bool> running_;
};

template<typename T>
  NoOpEventProcessor::NoOpEventProcessor(RingBuffer<T>* ring_buffer)
    : running_(false) {
  sequence_ = SequencePtr(new SequencerFollowingSequence<T>(ring_buffer));
}

} //end namespace

#endif //NO_OP_EVENT_PROCESSOR_H_
