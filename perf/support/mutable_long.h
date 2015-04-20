#ifndef MUTABLE_LONG_H_
#define MUTABLE_LONG_H_

#include <cstdint>

class MutableLong {
 public:
  MutableLong()
    : value_(0) {}

  MutableLong(int64_t initial_value)
    : value_(initial_value) {}

  int64_t Get() {
    return value_;
  }

  void Set(int64_t value) {
    value_ = value;
  }

  void Increment() {
    value_++;
  }
 private:
  int64_t value_;
};

#endif //MUTABLE_LONG_H_
