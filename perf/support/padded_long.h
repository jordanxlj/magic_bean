#ifndef PADDED_LONG_H_
#define PADDED_LONG_H_

#include "mutable_long.h"

class PaddedLong : public MutableLong {
 public:
  PaddedLong() {};

  PaddedLong(int64_t initial_value)
    : MutableLong(initial_value) {}

 public:
  volatile int64_t p1, p2, p3, p4, p5, p6;
};

#endif //PADDED_LONG_H_
