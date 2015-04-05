#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

namespace magic_bean {

class RingBufferPad {
 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

template<typename T>
class RingBufferFields : public RingBufferPad {
 public:
  RingBufferField(EventFactory<T>* event_factory, Sequencer* sequencer);
 protected:
  T* ElementAt(int64_t sequence);
 private:
  void Fill(EventFactory<T>* event_factory);

 private:
  int64_t index_mask_;
  T* entries_;
  int buffer_size_;
  Sequencer* sequencer_;
};

template<typename T>
class RingBuffer : public RingBufferFields<T>, public Cursored, public DataProvider<T>, public Sequenced {
 public:
  RingBuffer(EventFactory<T>* event_factory, Sequencer* sequencer);

  static RingBuffer<T>* CreateSingleProducer(EventFactory<T>* event_factory,
                                             int buffer_size, WaitStrategy* wait_strategy);
  static RingBuffer<T>* CreateSingleProducer(EventFactory<T>* event_factory, int buffer_size);

  virtual T* Get(int64_t sequence) override;
  virtual int64_t Next() override;
  virtual int64_t Next(int n) override;

  virtual int64_t TryNext() throw(InsufficientCapacityException) override;
  virtual int64_t TryNext(int n) throw(InsufficientCapacityException) override;

  T* ClaimAndGetPreallocated(int64_t sequence);
  bool IsPublished(int64_t sequence);

  void AddGatingSequences(const std::vector<SequencePtr>& gating_sequences);
  bool RemoveGatingSequence(SequencePtr gating_sequence);
  int64_t GetMinimumGatingSequence();

  SequenceBarrier* NewBarrier(const std::vector<SequencePtr>& sequences_to_track);
  EventPoller<T>* NewPoller(std::vector<SequencePtr>& gating_sequences);

  virtual int64_t GetCursor() override;
  int GetBufferSize() const;
  bool HasAvailableCapacity(int required_capacity);

  virtual void Publish(int64_t sequence) override;
  virtual void Publish(int64_t lo, int64_t hi) override;

  int64_t RemainingCapacity();
};

} //end namespace

#endif //RING_BUFFER_H_
