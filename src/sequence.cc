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

#include "sequence.h"

namespace magic_bean{

const int64_t kInitialValue = -1;

Sequence::Sequence() {
  value_.store(kInitialValue);
}

Sequence::Sequence(int64_t initial_value) {
  value_.store(initial_value);
}

Sequence::~Sequence() {}

int64_t Sequence::Get() const {
  return value_.load(std::memory_order::memory_order_acquire);
}

void Sequence::Set(int64_t value) {
  value_.store(value, std::memory_order::memory_order_release);
}

bool Sequence:: CompareAndSet(int64_t expected_value, int64_t new_value) {
  return value_.compare_exchange_strong(expected_value, new_value);
}

int64_t Sequence::IncrementAndGet() {
  return AddAndGet(1L);
}

int64_t Sequence::AddAndGet(int64_t increment) {
  int64_t current_value, new_value;
  current_value = value_.load(std::memory_order::memory_order_relaxed);
  do {
    new_value = current_value + increment;
  } while(!value_.compare_exchange_weak(current_value, new_value));
  return new_value;
}

} //end namespace
