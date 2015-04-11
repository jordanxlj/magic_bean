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
  return value_.compare_exchange_weak(expected_value, new_value);
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
