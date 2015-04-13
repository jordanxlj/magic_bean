#include "single_producer_sequencer.h"
#include <stdexcept>
#include "insufficient_capacity_exception.h"
#include "sequence_groups.h"
#include "wait_strategy.h"
#include <iostream>

namespace magic_bean {

static const int64_t kInitialValue = -1;

SingleProducerSequencerPad::SingleProducerSequencerPad(int buffer_size,
                                                       WaitStrategy* wait_strategy)
  : AbstractSequencer(buffer_size, wait_strategy) {}

SingleProducerSequencerFields::SingleProducerSequencerFields(int buffer_size,
                                                             WaitStrategy* wait_strategy)
  : SingleProducerSequencerPad(buffer_size, wait_strategy)
  , next_value_(kInitialValue)
  , cached_value_(kInitialValue) {
}

SingleProducerSequencer::SingleProducerSequencer(int buffer_size, WaitStrategy* wait_strategy)
  : SingleProducerSequencerFields(buffer_size, wait_strategy) {}

SingleProducerSequencer::~SingleProducerSequencer() {}

bool SingleProducerSequencer::HasAvailableCapacity(int required_capacity) {
  int64_t next_value = next_value_;
  int64_t wrap_point = (next_value + required_capacity) - buffer_size_;
  int64_t cached_gating_sequence = cached_value_;


  if(wrap_point > cached_gating_sequence || cached_gating_sequence > next_value) {
    int64_t min_sequence = GetMinimumSequence(next_value);
    cached_value_ = min_sequence;

    std::cout << "required: "<< required_capacity << ", wrap: " << wrap_point << ", cached: " << cached_gating_sequence << ", next: " << next_value << ", min: " << min_sequence << std::endl;
    if(wrap_point > min_sequence)
      return false;
  }

  std::cout << "required: " << required_capacity << ", wrap: " << wrap_point << ", cached: " << cached_gating_sequence << ", next: " << next_value << std::endl;
  return true;
}

int64_t SingleProducerSequencer::Next() {
  return Next(1);
}

int64_t SingleProducerSequencer::Next(int n) {
  if(n < 1)
    throw std::invalid_argument("next n is invalid");

  int64_t next_value = next_value_;
  int64_t next_sequence = next_value + n;
  int64_t wrap_point = next_sequence - buffer_size_;
  int64_t cached_gating_sequence = cached_value_;

  if(wrap_point > cached_gating_sequence || cached_gating_sequence > next_value) {
    int64_t min_sequence;
    while(wrap_point > (min_sequence = GetMinimumSequence(next_value)))
      ;
    cached_value_ = min_sequence;
  }
  next_value_ = next_sequence;
  return next_sequence;
}

int64_t SingleProducerSequencer::TryNext() throw (InsufficientCapacityException) {
  return TryNext(1);
}

int64_t SingleProducerSequencer::TryNext(int n) throw (InsufficientCapacityException) {
  if(n < 1)
    throw std::invalid_argument("try next n is invalid");

  if(!HasAvailableCapacity(n))
    throw InsufficientCapacityException("not has available capacity");

  int64_t next_sequence = next_value_ += n;
  return next_sequence;
}

int64_t SingleProducerSequencer::RemainingCapacity() const {
  int64_t next_value = next_value_;
  int64_t consumed = GetMinimumSequence(next_value);
  int64_t produced = next_value;
  return GetBufferSize() - (produced - consumed);
}

void SingleProducerSequencer::Claim(int64_t sequence) {
  next_value_ = sequence;
}

void SingleProducerSequencer::Publish(int64_t sequence) {
  cursor_->Set(sequence);
  wait_strategy_->SignalAllWhenBlocking();
}

void SingleProducerSequencer::Publish(int64_t lo, int64_t hi) {
  Publish(hi);
}

bool SingleProducerSequencer::IsAvailable(int64_t sequence) {
  return sequence <= cursor_->Get();
}

int64_t SingleProducerSequencer::GetHighestPublishedSequence(int64_t next_sequence,
                                                             int64_t available_sequence) {
  return available_sequence;
}

} //end namespace
