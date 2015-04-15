
/*
 * Copyright 2015 jordanxlj
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SEQUENCE_GROUPS_H_
#define SEQUENCE_GROUPS_H_

#include <atomic>
#include <vector>
#include "magic_types.h"
#include "sequence.h"

namespace magic_bean {

class Cursored;

class SequenceGroups {
  struct Node {
    SequencePtr sequence;
    std::shared_ptr<Node> next;
  };

 public:
  SequenceGroups();
  ~SequenceGroups();

  void AddSequences(Cursored* cursor, const std::vector<SequencePtr>& sequences_to_add);
  bool RemoveSequence(SequencePtr sequence);

  int64_t GetMinimumSequence(int64_t minimum) const;

 private:
  void AddSequence(Cursored* cursor, SequencePtr sequence);

 private:
   std::atomic<std::shared_ptr<Node>> head_;
};

} //end namespace

#endif //SEQUENCE_GROUPS_H_
