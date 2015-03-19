#ifndef SEQUENCE_BARRIER_H_
#define SEQUENCE_BARRIER_H_

namespace magic_bean {

class SequenceBarrier {
 public:
  virtual ~SequenceBarrier() {};
  virtual int64_t WaitFor(int64_t sequence) throw (AlertException, TimeoutException) = 0;

  virtual int64_t GetCursor() const = 0;

  virtual bool IsAlerted() const = 0;
  virtual void Alert() = 0;

  virtual void ClearAlert() = 0;
  virtual CheckAlert() throw AlertException = 0;
};

} //end namespace

#endif //SEQUENCE_BARRIER_H_
