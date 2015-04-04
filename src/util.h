#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include "magic_types.h"

namespace magic_bean {

class Util {
 public:
  static int64_t GetMinimumSequence(const std::vector<SequencePtr>& sequences, int64_t minimum);
};

} //end namespace

#endif //UTIL_H_
