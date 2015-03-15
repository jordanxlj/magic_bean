#ifndef SEQUENCE_H_
#define SEQUENCE_H_

class LhsPadding {
 protected:
  int64_t p1_, p2_, p3_, p4_, p5_, p6_, p7_;
};

class Value : public LhsPadding {
 protected:
  volatile int64_t value_;
};

class RhsPadding : public Value {
 protected:
  int64_t p9_, p10_, p11_, p12_, p13_, p14_, p15_;
};

class Sequence : public RhsPadding {
 public:
};

#endif //SEQUENCE_H_
