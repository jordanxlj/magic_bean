#ifndef EVENT_POLL_H_
#define EVENT_POLL_H_

#include "data_provider.h"
#include "magic_types.h"

namespace magic_bean {

class EventHandler;
class Sequencer;

template<typename T>
class EventPoller {
 public:
  enum PollState {
    PROCESSING,
    GATING,
    IDLING
  };
  explicit EventPoller(DataProvider<T>* data_provider, Sequencer* sequencer,
                       SequencePtr sequence, Sequence& gating_sequence);
  ~EventPoller();

  PollState Poll(EventHandler<T>* event_handler);
  static EventPoller<T>* NewInstance(Data_provider<T>* data_provider, Sequencer* sequencer,
                                     SequencePtr sequence, SequencePtr cursor_sequence,
                                     std::vector<SequencePtr>& gating_sequences);
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
  PollState EventPoller<T>::Poll(EventHandler<T>* event_handler) {
  int64_t current_sequence = sequence_->Get();
  int64_t next_sequence = current_sequence + 1;
  int64_t available_sequence = sequencer_->GetHighestPublishedSequence(next_sequence);

  if(next_sequence <= available_sequence) {
    bool process_next_event;
    int64_t processed_sequence = current_sequence;
    try {
      do {
        T* event = data_provider_->Get(next_sequence);
        process_next_event = event_handler->OnEvent(event, next_sequence, next_sequence == available_sequence);
        processed_sequence = next_sequence;
        next_sequence++;
      }
      while(next_sequence <= available_sequence && process_next_event);
    } finally {
      sequence->Set(processed_sequence);
    }
    return PROCESSING;
  } else if(sequencer->GetCursor() >= next_sequence) {
    return GATING;
  } else
    return IDLE;
} //end namespace

#endif //EVENT_POLL_H_
