#ifndef ABSTRACT_SEQUENCER_H_
#define ABSTRACT_SEQUENCER_H_

#include <vector>
#include "event_poller.h"
#include "magic_types.h"
#include "sequencer.h"

namespace magic_bean {

class WaitStrategy;
class SequenceGroups;

class AbstractSequencer : public Sequencer {
 public:
  explicit AbstractSequencer(int buffer_size, WaitStrategy* wait_strategy);
  virtual ~AbstractSequencer();

  virtual int64_t GetCursor() override;
  virtual int GetBufferSize() const override;

  virtual void AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) override;
  virtual bool RemoveGatingSequence(SequencePtr gating_sequence) override;
  virtual SequenceBarrier* NewBarrier(const std::vector<SequencePtr>& sequences_to_track) override;

  virtual int64_t GetMinimumSequence() const override;
  int64_t GetMinimumSequence(int64_t minimum) const;

  template<typename T>
    EventPoller<T> NewPoller(DataProvider<T>* provider,
                             const std::vector<SequencePtr>& gating_sequences);
 protected:
  int buffer_size_;
  WaitStrategy* wait_strategy_;
  SequencePtr cursor_;
  SequenceGroups* sequence_groups_;
};

} //end namespace

#endif //ABSTRACT_SEQUENCER_H_
