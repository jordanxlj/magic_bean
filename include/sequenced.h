#ifndef SEQUENCED_H_
#define SEQUENCED_H_

namespace magic_bean {

class Sequenced {
 public:
  virtual ~Sequenced() {};

  virtual int GetBufferSize() const = 0;
  virtual bool HasAvailableCapacity(int required_capacity) const = 0;
  virtual int64_t RemainingCapacity() const = 0;

  virtual int64_t Next() = 0;
  virtual int64_t Next(int n) = 0;

  virtual int64_t TryNext() throw InsufficientCapacityException = 0;
  virtual int64_t TryNext(int n) throw InsufficientCapacityException = 0;

  virtual void Publish(int64_t sequence) = 0;
  virtual void Publish(int64_t lo, int64_t hi) = 0;
};

} //end namespace

#endif //SEQUENCED_H_
