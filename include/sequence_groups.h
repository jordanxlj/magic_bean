#ifndef SEQUENCE_GROUPS_H_
#define SEQUENCE_GROUPS_H_

#include <atomic>
#include <vector>
#include "magic_types.h"

namespace magic_bean {

class Cursored;

class SequenceGroups {
 public:
  SequenceGroups();
  ~SequenceGroups();

  void AddSequences(Cursored* cursor, const std::vector<SequencePtr>& sequences_to_add);
  bool RemoveSequence(SequencePtr sequence);

  int64_t GetMinimumSequence();

 private:
  void AddSequence(Cursored* cursor, SequencePtr sequence);

 private:
  struct Node {
    SequencePtr sequence;
    std::shared_ptr<Node> next;
  };
  std::atomic<std::shared_ptr<Node>> head_;
};

} //end namespace

#endif //SEQUENCE_GROUPS_H_
