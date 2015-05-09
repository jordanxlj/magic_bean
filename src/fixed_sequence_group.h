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

#ifndef FIXED_SEQUENCE_GROUP_H_
#define FIXED_SEQUENCE_GROUP_H_

#include <vector>
#include "magic_types.h"
#include "sequence.h"

namespace magic_bean{

class FixedSequenceGroup : public Sequence {
 public:
  FixedSequenceGroup(const std::vector<SequencePtr>& sequences);
  ~FixedSequenceGroup();

  virtual int64_t Get() const override;
  virtual void Set(int64_t value) override;

  virtual bool CompareAndSet(int64_t expected_value, int64_t new_value) override;
  virtual int64_t IncrementAndGet() override;
  virtual int64_t AddAndGet(int64_t increment) override;
 private:
  std::vector<SequencePtr> sequences_;
};

} //end namespace

#endif //FIXED_SEQUENCE_GROUP_H_
