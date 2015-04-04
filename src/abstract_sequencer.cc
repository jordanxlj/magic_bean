#include "abstract_sequencer.h"
#include <wait_strategy.h>

namespace magic_bean {

AbstractSequencer::AbstractSequencer(int buffer_size, WaitStrategy* wait_strategy)
  : buffer_size_(buffer_size)
  , wait_strategy_(wait_strategy)
  , cursor_(new Sequence) {
}

AbstractSequencer::~AbstractSequencer() {}

int64_t AbstractSequencer::GetCursor() {
  return cursor_->Get();
}

int AbstractSequencer::GetBufferSize() const {
  return buffer_size_;
}

void AbstractSequencer::AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) {
}

bool AbstractSequencer::RemoveGatingSequence(SequencePtr gating_sequence) {
  return false;
}

int64_t AbstractSequencer::GetMinimumSequence() const {
  return 0;
}

template<typename T>
EventPoller<T> AbstractSequencer::NewPoller(DataProvider<T>* provider,
                                            const std::vector<SequencePtr>& gating_sequences) {
  return EventPoller<T>::NewInstance(provider, this, new Sequence, cursor_, gating_sequences);
}

} //end namespace
