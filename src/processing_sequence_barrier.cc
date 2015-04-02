#include "processing_sequence_barrier.h"
#include <sequencer.h>
#include <wait_strategy.h>

namespace magic_bean {

ProcessingSequenceBarrier::ProcessingSequenceBarrier(Sequencer* sequencer,
                                                     WaitStrategy* wait_strategy,
                                                     SequencePtr cursor_sequence,
                                                     const std::vector<SequencePtr>& dependent_sequences)
  : sequencer_(sequencer)
  , wait_strategy_(wait_strategy)
  , cursor_sequence_(cursor_sequence) {
  if(dependent_sequences.size() == 0) {
    dependent_sequence_ = cursor_sequence;
  } else {
  }
}

ProcessingSequenceBarrier::~ProcessingSequenceBarrier() {
}

int64_t ProcessingSequenceBarrier::WaitFor(int64_t sequence) throw (AlertException, TimeoutException) {
  CheckAlert();
  int64_t available_sequence = wait_strategy_->WaitFor(sequence, cursor_sequence_,
                                                       dependent_sequence_,
                                                       static_cast<SequenceBarrier*>(this));
  if(available_sequence < sequence)
    return available_sequence;

  return sequencer_->GetHighestPublishedSequence(sequence, available_sequence);
}

int64_t ProcessingSequenceBarrier::GetCursor() const {
  return dependent_sequence_->Get();
}

bool ProcessingSequenceBarrier::IsAlerted() const {
  return alerted_;
}

void ProcessingSequenceBarrier::Alert() {
  alerted_ = true;
  wait_strategy_->SignalAllWhenBlocking();
}

void ProcessingSequenceBarrier::ClearAlert() {
  alerted_ = false;
}

void ProcessingSequenceBarrier::CheckAlert() throw (AlertException) {
  if(alerted_)
    throw AlertException();
}

} //end namespace
