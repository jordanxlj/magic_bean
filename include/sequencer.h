#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include <vector>
#include <cursored.h>
#include <magic_types.h>
#include <sequence.h>
#include <sequenced.h>

namespace magic_bean {

class SequenceBarrier;

class Sequencer : public Cursored, public Sequenced {
 public:
  virtual ~Sequencer() {};

  virtual void Claim(int64_t sequence) = 0;
  virtual bool IsAvailable(int64_t sequence) = 0;

  virtual void AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) = 0;
  virtual bool RemoveGatingSequence(SequencePtr gating_sequence) = 0;

  virtual SequenceBarrier* NewBarrier(const std::vector<SequencePtr>& sequences_to_track) = 0;
  virtual int64_t GetMinimumSequence() const = 0;
  virtual int64_t GetHighestPublishedSequence(int64_t next_sequence, int64_t available_sequence) = 0;
};

} //end namespace

#endif //SEQUENCER_H_
