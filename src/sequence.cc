#include "sequence.h"

namespace magic_bean{

const int64_t kInitialValue = -1;

Sequence::Sequence() {
  value_.store(kInitialValue);
}

Sequence::Sequence(int64_t initial_value) {
  value_.store(initial_value);
}

int64_t Sequence::Get() const {
  return value_.load(std::memory_order::memory_order_acquire);
}

void Sequence::Set(int64_t value) {
  value_.store(value, std::memory_order::memory_order_release);
}

int64_t Sequence::IncrementAndGet() {
  return AddAndGet(1L);
}

int64_t Sequence::AddAndGet(int64_t increment) {
  int64_t current_value, new_value;
  return 0;
}

} //end namespace
