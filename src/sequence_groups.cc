#include "sequence_groups.h"
#include "cursored.h"
#include "sequence.h"

namespace magic_bean {

SequenceGroups::SequenceGroups()
  : head_(nullptr) {}

SequenceGroups::~SequenceGroups() {}

void SequenceGroups::AddSequences(Cursored* cursor,
                                  const std::vector<SequencePtr>& sequences_to_add) {
  for(auto sequence_to_add : sequences_to_add)
    AddSequence(cursor, sequence_to_add);
}

void SequenceGroups::AddSequence(Cursored* cursor, SequencePtr sequence) {
  int64_t cursor_sequence = cursor->GetCursor();
  sequence->Set(cursor_sequence);

  auto p = std::make_shared<Node>();
  p->sequence = sequence;
  p->next = head_.load(std::memory_order_relaxed);

  while(!std::atomic_compare_exchange_weak_explicit(&head_, &p->next, p,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed))
    ;
}

bool SequenceGroups::RemoveSequence(SequencePtr sequence) {
  auto p = head_.load(std::memory_order_release);
  if(p && p->sequence == sequence
     && std::atomic_compare_exchange_strong_explicit(&head_, &p, p->next,
                                                     std::memory_order_acquire,
                                                     std::memory_order_relaxed)) {
    p.reset();
    return true;
  } else
    return false;
}

int64_t SequenceGroups::GetMinimumSequence(int64_t minimum) const {
  auto p = head_.load(std::memory_order_relaxed);
  int64_t minimum_value = minimum;
  while(p) {
    if(p->sequence->Get() < minimum_value)
      minimum_value = p->sequence->Get();
    p = p->next;
  }
  return minimum_value;
}

} //end namespace
