#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "blocking_wait_strategy.h"
#include "cursored.h"
#include "data_provider.h"
#include "event_factory.h"
#include "event_poller.h"
#include "sequenced.h"
#include "single_producer_sequencer.h"

namespace magic_bean {

class Sequencer;
class WaitStrategy;

class RingBufferPad {
 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

template<typename T>
class RingBufferFields : public RingBufferPad {
 public:
  RingBufferFields(EventFactory<T>* event_factory, Sequencer* sequencer);
  virtual ~RingBufferFields();

 protected:
  T* ElementAt(int64_t sequence);

 private:
  void Fill(EventFactory<T>* event_factory);

 protected:
  int buffer_size_;
  Sequencer* sequencer_;

 private:
  int64_t index_mask_;
  T** entries_;
};

template<typename T>
  RingBufferFields<T>::RingBufferFields(EventFactory<T>* event_factory, Sequencer* sequencer) {
  sequencer_ = sequencer;
  buffer_size_ = sequencer->GetBufferSize();
  index_mask_ = buffer_size_ - 1;
  entries_ = new T*[buffer_size_];
  Fill(event_factory);
}

template<typename T>
  RingBufferFields<T>::~RingBufferFields() {
  for(int i = 0; i < buffer_size_; i++)
    delete entries_[i];
  delete [] entries_;
}

template<typename T>
  T* RingBufferFields<T>::ElementAt(int64_t sequence) {
  return entries_[sequence & index_mask_];
}

template<typename T>
  void RingBufferFields<T>::Fill(EventFactory<T>* event_factory) {
  for(int i = 0; i < buffer_size_; i++)
    entries_[i] = event_factory->NewInstance();
}

template<typename T>
class RingBuffer : public RingBufferFields<T>, public Cursored, public DataProvider<T>, public Sequenced {
 public:
  RingBuffer(EventFactory<T>* event_factory, Sequencer* sequencer);
  ~RingBuffer();

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

  virtual int64_t RemainingCapacity() const override;

 protected:
  int64_t p1, p2, p3, p4, p5, p6, p7;
};

template<typename T>
  RingBuffer<T>::RingBuffer(EventFactory<T>* event_factory, Sequencer* sequencer)
  : RingBufferFields<T>(event_factory, sequencer) {}

template<typename T>
  RingBuffer<T>::~RingBuffer() {}

template<typename T>
  RingBuffer<T>* RingBuffer<T>::CreateSingleProducer(EventFactory<T>* factory,
                                                     int buffer_size,
                                                     WaitStrategy* wait_strategy) {
  Sequencer* sequencer = new SingleProducerSequencer(buffer_size, wait_strategy);
  return new RingBuffer<T>(factory, sequencer);
}

template<typename T>
  RingBuffer<T>* RingBuffer<T>::CreateSingleProducer(EventFactory<T>* factory,
                                                     int buffer_size) {
  return CreateSingleProducer(factory, buffer_size, new BlockingWaitStrategy());
}

template<typename T>
  T* RingBuffer<T>::Get(int64_t sequence) {
  return RingBufferFields<T>::ElementAt(sequence);
}

template<typename T>
  int64_t RingBuffer<T>::Next() {
  return RingBufferFields<T>::sequencer_->Next();
}

template<typename T>
  int64_t RingBuffer<T>::Next(int n) {
  return RingBufferFields<T>::sequencer_->Next(n);
}

template<typename T>
  int64_t RingBuffer<T>::TryNext() throw(InsufficientCapacityException) {
  return RingBufferFields<T>::sequencer_->TryNext();
}

template<typename T>
  int64_t RingBuffer<T>::TryNext(int n) throw(InsufficientCapacityException) {
  return RingBufferFields<T>::sequencer_->TryNext(n);
}

template<typename T>
  T* RingBuffer<T>::ClaimAndGetPreallocated(int64_t sequence) {
  RingBufferFields<T>::sequencer_->Claim(sequence);
  return Get(sequence);
}

template<typename T>
  bool RingBuffer<T>::IsPublished(int64_t sequence) {
  return RingBufferFields<T>::sequencer_->IsAvailable(sequence);
}

template<typename T>
  void RingBuffer<T>::AddGatingSequences(const std::vector<SequencePtr>& gating_sequences) {
  RingBufferFields<T>::sequencer_->AddGatingSequences(gating_sequences);
}

template<typename T>
  bool RingBuffer<T>::RemoveGatingSequence(SequencePtr gating_sequence) {
  return RingBufferFields<T>::sequencer_->RemoveGatingSequence(gating_sequence);
}

template<typename T>
  int64_t RingBuffer<T>::GetMinimumGatingSequence() {
  return RingBufferFields<T>::sequencer_->GetMinimumGatingSequence();
}

template<typename T>
  SequenceBarrier* RingBuffer<T>::NewBarrier(const std::vector<SequencePtr>& sequences_to_track) {
  return RingBufferFields<T>::sequencer_->NewBarrier(sequences_to_track);
}

template<typename T>
  EventPoller<T>* RingBuffer<T>::NewPoller(std::vector<SequencePtr>& gating_sequences) {
  return RingBufferFields<T>::sequencer_->NewPoller(this, gating_sequences);
}

template<typename T>
  int64_t RingBuffer<T>::GetCursor() {
  return RingBufferFields<T>::sequencer_->GetCursor();
}

template<typename T>
  int RingBuffer<T>::GetBufferSize() const {
  return RingBufferFields<T>::buffer_size_;
}

template<typename T>
  bool RingBuffer<T>::HasAvailableCapacity(int required_capacity) {
  return RingBufferFields<T>::sequencer_->HasAvailableCapacity(required_capacity);
}

template<typename T>
  void RingBuffer<T>::Publish(int64_t sequence) {
  RingBufferFields<T>::sequencer_->Publish(sequence);
}

template<typename T>
  void RingBuffer<T>::Publish(int64_t lo, int64_t hi) {
  RingBufferFields<T>::sequencer_->Publish(lo, hi);
}

template<typename T>
  int64_t RingBuffer<T>::RemainingCapacity() const {
  return RingBufferFields<T>::sequencer_->RemainingCapacity();
}

} //end namespace

#endif //RING_BUFFER_H_
