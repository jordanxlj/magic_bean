#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

namespace magic_bean {

class RingBufferPad {
 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

template<typename T>
class RingBufferField : public RingBufferPad {
 public:
} //end namespace

#endif //RING_BUFFER_H_
