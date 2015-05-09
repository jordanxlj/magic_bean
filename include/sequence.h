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

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <atomic>
#include <cstdint>

namespace magic_bean{

class LhsPadding {
 public:
  int64_t p1_, p2_, p3_, p4_, p5_, p6_, p7_;
};

class Value : public LhsPadding {
 public:
  std::atomic<int64_t> value_;
};

class RhsPadding : public Value {
 public:
  int64_t p9_, p10_, p11_, p12_, p13_, p14_, p15_;
};

class Sequence : public RhsPadding {
 public:
  Sequence();
  Sequence(int64_t initial_value);
  virtual ~Sequence();

  virtual int64_t Get() const;
  virtual void Set(int64_t value);

  virtual bool CompareAndSet(int64_t expected_value, int64_t new_value);
  virtual int64_t IncrementAndGet();
  virtual int64_t AddAndGet(int64_t increment);
};

} //end namespace

#endif //SEQUENCE_H_
