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
  ~Sequence();

  int64_t Get() const;
  void Set(int64_t value);

  bool CompareAndSet(int64_t expected_value, int64_t new_value);
  int64_t IncrementAndGet();
  int64_t AddAndGet(int64_t increment);
};

} //end namespace

#endif //SEQUENCE_H_
