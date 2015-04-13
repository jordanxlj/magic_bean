#include "multi_producer_sequencer.h"
#include <stdexcept>
#include "insufficient_capacity_exception.h"
#include "sequence_groups.h"
#include "wait_strategy.h"
#include "util.h"

namespace magic_bean {

MultiProducerSequencer::MultiProducerSequencer(int buffer_size, WaitStrategy* wait_strategy)
  : AbstractSequencer(buffer_size, wait_strategy) {
  gating_sequence_cache_ = SequencePtr(new Sequence);
  available_buffer_ = new int[buffer_size];
  index_mask_ = buffer_size - 1;
  index_shift_ = Util::Log2(buffer_size);
  InitializeAvailableBuffer();
}

void MultiProducerSequencer::InitializeAvailableBuffer() {
  for(int i = buffer_size_ - 1; i != 0; i--)
    SetAvailableBufferValue(i, -1);
  SetAvailableBufferValue(0, -1);
}

void MultiProducerSequencer::SetAvailableBufferValue(int index, int flag) {
  available_buffer_[index] = flag;
}


MultiProducerSequencer::~MultiProducerSequencer() {
  delete [] available_buffer_;
}

bool MultiProducerSequencer::HasAvailableCapacity(int required_capacity) {
  return HasAvailableCapacity(required_capacity, cursor_->Get());
}

bool MultiProducerSequencer::HasAvailableCapacity(int required_capacity, int64_t cursor_value) {
  int64_t wrap_point = (cursor_value + required_capacity) - buffer_size_;
  int64_t cached_gating_sequence = gating_sequence_cache_->Get();
  if(wrap_point > cached_gating_sequence || cached_gating_sequence > cursor_value) {
    int64_t min_sequence = GetMinimumSequence(cursor_value);
    gating_sequence_cache_->Set(min_sequence);
    if(wrap_point > min_sequence)
      return false;
  }
  return true;
}

int64_t MultiProducerSequencer::Next() {
  return Next(1);
}

int64_t MultiProducerSequencer::Next(int n) {
  if(n < 1)
    throw std::invalid_argument("next n is invalid");

  int current, next;
  do {
    current = cursor_->Get();
    next = current + n;

    int64_t wrap_point = next - buffer_size_;
    int64_t cached_gating_sequence = gating_sequence_cache_->Get();

    if(wrap_point > cached_gating_sequence || cached_gating_sequence > current) {
      int64_t gating_sequence = GetMinimumSequence(current);
      if(wrap_point > gating_sequence)
        continue;

      gating_sequence_cache_->Set(gating_sequence);
    } else if(cursor_->CompareAndSet(current, next))
      break;
  } while (true);

  return next;
}

int64_t MultiProducerSequencer::TryNext() throw (InsufficientCapacityException) {
  return TryNext(1);
}

int64_t MultiProducerSequencer::TryNext(int n) throw (InsufficientCapacityException) {
  if(n < 1)
    throw std::invalid_argument("n must be > 0");

  int64_t current, next;
  do {
    current = cursor_->Get();
    next = current + n;

    if(!HasAvailableCapacity(n, current))
      throw InsufficientCapacityException("not has available capacity");
  } while (!cursor_->CompareAndSet(current, next));

  return next;
}

int64_t MultiProducerSequencer::RemainingCapacity() const {
  int64_t consumed = GetMinimumSequence(cursor_->Get());
  int64_t produced = cursor_->Get();
  return GetBufferSize() - (produced - consumed);
}

void MultiProducerSequencer::Claim(int64_t sequence) {
  cursor_->Set(sequence);
}

void MultiProducerSequencer::Publish(int64_t sequence) {
  SetAvailable(sequence);
  wait_strategy_->SignalAllWhenBlocking();
}

void MultiProducerSequencer::Publish(int64_t lo, int64_t hi) {
  for(int64_t l = lo; l <= hi; l++)
    SetAvailable(l);
  wait_strategy_->SignalAllWhenBlocking();
}

void MultiProducerSequencer::SetAvailable(int64_t sequence) {
  SetAvailableBufferValue(CalculateIndex(sequence), CalculateAvailabilityFlag(sequence));
}

int MultiProducerSequencer::CalculateAvailabilityFlag(int64_t sequence) {
  return (int) (sequence >> index_shift_);
}

int MultiProducerSequencer::CalculateIndex(int64_t sequence) {
  return ((int)sequence) & index_mask_;
}

bool MultiProducerSequencer::IsAvailable(int64_t sequence) {
  int index = CalculateIndex(sequence);
  int flag = CalculateAvailabilityFlag(sequence);
  return available_buffer_[index] == flag;
}

int64_t MultiProducerSequencer::GetHighestPublishedSequence(int64_t low_bound,
                                                            int64_t available_sequence) {
  for(int64_t sequence = low_bound; sequence < available_sequence; sequence++)
    if(!IsAvailable(sequence))
      return sequence - 1;

  return available_sequence;
}

} //end namespace
