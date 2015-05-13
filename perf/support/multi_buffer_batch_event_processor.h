#ifndef MULTI_BUFFER_BATCH_EVENT_PROCESSOR_H_
#define MULTI_BUFFER_BATCH_EVENT_PROCESSOR_H_

#include <atomic>
#include <iostream>
#include <thread>
#include "alert_exception.h"
#include "event_processor.h"
#include "ring_buffer.h"

template<typename T>
class MultiBufferBatchEventProcessor : public magic_bean::EventProcessor {
 public:
 MultiBufferBatchEventProcessor(magic_bean::RingBuffer<T>** providers,
                                magic_bean::SequenceBarrier** barriers,
                                magic_bean::EventHandler<T>* handler, int size)
    : providers_(providers)
    , barriers_(barriers)
    , handler_(handler)
    , size_(size)
    , is_running_(false) {
    for(int i = 0; i < size; i++)
      sequences_.push_back(magic_bean::SequencePtr(new magic_bean::Sequence));
  }

  ~MultiBufferBatchEventProcessor() {
    sequences_.clear();
  }

  virtual magic_bean::SequencePtr GetSequence() override {
    return magic_bean::SequencePtr(new magic_bean::Sequence);
  }

  virtual void Halt() override {
    is_running_.store(false);
    barriers_[0]->Alert();
  }

  virtual bool IsRunning() override {
    return is_running_.load();
  }

  virtual void Run() override {
    bool current = false;
    if(!is_running_.compare_exchange_strong(current, true)) {
      std::cout << "exception, for already running." << std::endl;
      return;
    }

    for(int i = 0; i < size_; i++)
      barriers_[i]->ClearAlert();

    while(true) {
      try {
        for(int i = 0; i < size_; i++) {
          int64_t available = barriers_[i]->WaitFor(-1);
          magic_bean::SequencePtr sequence = sequences_[i];

          int64_t previous = sequence->Get();
          for(int64_t l = previous + 1; l <= available; l++)
            handler_->OnEvent(providers_[i]->Get(l), l, previous == available);

          sequence->Set(available);
          count_ += (available - previous);
        }
        std::this_thread::yield();
      } catch(magic_bean::AlertException& ex) {
        if(!is_running_.load())
          break;
      } catch(...) {
        std::cout << "unexpected exception." << std::endl;
      }
    }
  }

  int64_t GetCount() {
    return count_;
  }

  std::vector<magic_bean::SequencePtr> GetSequences() {
    return sequences_;
  }
 private:
  magic_bean::RingBuffer<T>** providers_;
  magic_bean::SequenceBarrier** barriers_;
  magic_bean::EventHandler<T>* handler_;
  int size_;
  std::atomic<bool> is_running_;
  std::vector<magic_bean::SequencePtr> sequences_;
  int64_t count_;
};

#endif //MULTI_BUFFER_BATCH_EVENT_PROCESSOR_H_
