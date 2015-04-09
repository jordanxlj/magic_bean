#ifndef BATCH_EVENT_PROCESSOR_H_
#define BATCH_EVENT_PROCESSOR_H_

#include <atomic>
#include <stdexcept>
#include "data_provider.h"
#include "event_handler.h"
#include "event_processor.h"
#include "magic_types.h"
#include "sequence.h"
#include "sequence_barrier.h"

namespace magic_bean {

template<typename T>
class BatchEventProcessor : public EventProcessor {
 public:
  BatchEventProcessor(DataProvider<T>* data_provider, SequenceBarrier* barrier,
                      EventHandler<T>* event_handler);
  ~BatchEventProcessor();

  virtual SequencePtr GetSequence() override;

  virtual void Halt() override;
  virtual bool IsRunning() override;
  virtual void Run() override;

 private:
  void NotifyTimeout(int64_t available_sequence);
  void NotifyStart();
  void NotifyShutdown();

 private:
  DataProvider<T>* data_provider_;
  SequenceBarrier* sequence_barrier_;
  EventHandler<T>* event_handler_;
  SequencePtr sequence_;
  std::atomic<bool> running_;
};

template<typename T>
BatchEventProcessor<T>::BatchEventProcessor(DataProvider<T>* data_provider,
                                            SequenceBarrier* barrier,
                                            EventHandler<T>* event_handler)
  : data_provider_(data_provider)
  , sequence_barrier_(barrier)
  , event_handler_(event_handler)
  , sequence_(new Sequence)
  , running_(false) {
}

template<typename T>
BatchEventProcessor<T>::~BatchEventProcessor() {}

template<typename T>
SequencePtr BatchEventProcessor<T>::GetSequence() {
  return sequence_;
}

template<typename T>
void BatchEventProcessor<T>::Halt() {
  running_.store(false, std::memory_order::memory_order_relaxed);
  sequence_barrier_->Alert();
}

template<typename T>
bool BatchEventProcessor<T>::IsRunning() {
  return running_.load(std::memory_order::memory_order_relaxed);
}

template<typename T>
void BatchEventProcessor<T>::Run() {
  bool running = false;
  if(!running_.compare_exchange_weak(running, true)) {
    //throw runtime_error("Thread is already running");
  }

  sequence_barrier_->ClearAlert();
  NotifyStart();

  T* event = nullptr;
  int64_t next_sequence = sequence_->Get() + 1;

  while(true) {
    try {
      int64_t available_sequence = sequence_barrier_->WaitFor(next_sequence);
      while(next_sequence <= available_sequence) {
        event = data_provider_->Get(next_sequence);
        event_handler_->OnEvent(event, next_sequence, next_sequence == available_sequence);
        next_sequence++;
      }
      sequence_->Set(available_sequence);
    } catch (TimeoutException& e) {
      NotifyTimeout(sequence_->Get());
    } catch (AlertException& e) {
      if(!running_.load(std::memory_order::memory_order_relaxed))
        break;
    } catch (...) {
      sequence_->Set(next_sequence);
      next_sequence++;
    }
  }

  NotifyShutdown();
  running_.store(false, std::memory_order::memory_order_relaxed);
}

template<typename T>
void BatchEventProcessor<T>::NotifyTimeout(int64_t available_sequence) {
}

template<typename T>
void BatchEventProcessor<T>::NotifyStart() {
}

template<typename T>
void BatchEventProcessor<T>::NotifyShutdown() {
}

} //end namespace

#endif //BATCH_EVENT_PROCESSOR_H_
