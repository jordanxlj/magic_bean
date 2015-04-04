#ifndef EVENT_POLL_H_
#define EVENT_POLL_H_

#include "data_provider.h"
#include "event_handler.h"
#include "magic_types.h"
#include "sequence.h"
#include "sequencer.h"

namespace magic_bean {

enum class PollState {
  PROCESSING = 0,
  GATING,
  IDLE
};

template<typename T>
class Handler {
 public:
  virtual ~Handler() {};
  virtual bool OnEvent(T* event, int64_t sequence, bool end_of_batch) = 0;
};

template<typename T>
class EventPoller {
 public:
  explicit EventPoller(DataProvider<T>* data_provider, Sequencer* sequencer,
                       SequencePtr sequence, SequencePtr gating_sequence);
  ~EventPoller() {};

  PollState Poll(Handler<T>* handler);
  static EventPoller<T>* NewInstance(DataProvider<T>* data_provider, Sequencer* sequencer,
                                     SequencePtr sequence, SequencePtr cursor_sequence,
                                     const std::vector<SequencePtr>& gating_sequences);
  SequencePtr GetSequence();

 private:
  DataProvider<T>* data_provider_;
  Sequencer* sequencer_;
  SequencePtr sequence_;
  SequencePtr gating_sequence_;
};

template<typename T>
  EventPoller<T>::EventPoller(DataProvider<T>* data_provider, Sequencer* sequencer,
                              SequencePtr sequence, SequencePtr gating_sequence)
  : data_provider_(data_provider)
  , sequencer_(sequencer)
  , sequence_(sequence)
  , gating_sequence_(gating_sequence) {}

template<typename T>
  PollState EventPoller<T>::Poll(Handler<T>* handler) {
  int64_t current_sequence = sequence_->Get();
  int64_t next_sequence = current_sequence + 1;
  int64_t available_sequence = sequencer_->GetHighestPublishedSequence(next_sequence, gating_sequence_->Get());
  if(next_sequence <= available_sequence) {
    bool process_next_event;
    int64_t processed_sequence = current_sequence;
    try {
      do {
        T* event = data_provider_->Get(next_sequence);
        process_next_event = handler->OnEvent(event, next_sequence, next_sequence == available_sequence);
        processed_sequence = next_sequence;
        next_sequence++;
      }
      while(next_sequence <= available_sequence && process_next_event);
    } catch(...) {

    }

    sequence_->Set(processed_sequence);
    return PollState::PROCESSING;
  } else if(sequencer_->GetCursor() >= next_sequence) {
    return PollState::GATING;
  } else {
    return PollState::IDLE;
  }
}

template<typename T>
  EventPoller<T>* EventPoller<T>::NewInstance(DataProvider<T>* data_provider,
                                              Sequencer* sequencer, SequencePtr sequence,
                                              SequencePtr cursor_sequence,
                                              const std::vector<SequencePtr>& gating_sequences) {
  SequencePtr gating_sequence;
  if(gating_sequences.size() == 0) {
    gating_sequence = cursor_sequence;
  } else if(gating_sequences.size() == 1) {
    gating_sequence = gating_sequences[0];
  } else {
  }

  return new EventPoller<T>(data_provider, sequencer, sequence, gating_sequence);
}

template<typename T>
  SequencePtr EventPoller<T>::GetSequence() {
  return sequence_;
}

} //end namespace

#endif //EVENT_POLL_H_
