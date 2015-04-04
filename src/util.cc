#include "util.h"
#include "sequence.h"

namespace magic_bean {

int64_t Util::GetMinimumSequence(const std::vector<SequencePtr>& sequences, int64_t minimum) {
  for(size_t i = 0; i < sequences.size(); i++) {
    int64_t value = sequences[i]->Get();
    minimum = value < minimum ? value : minimum;
  }
  return minimum;
}

} //end namespace
