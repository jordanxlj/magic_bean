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
