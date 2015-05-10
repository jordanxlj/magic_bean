#ifndef VALUE_BATCH_PUBLISHER_H_
#define VALUE_BATCH_PUBLISHER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "ring_buffer.h"
#include "value_event.h"

class ValueBatchPublisher {
 public:
 ValueBatchPublisher(magic_bean::RingBuffer<ValueEvent>* ring_buffer,
                     int64_t iterations, int batch_size)
    : ring_buffer_(ring_buffer)
    , iterations_(iterations)
    , batch_size_(batch_size) {}

  void Run() {
    try {
      Wait();

      for(int64_t i = 0; i < iterations_; i += batch_size_) {
        int64_t hi = ring_buffer_->Next(batch_size_);
        int64_t lo = hi - (batch_size_ - 1);
        for(int64_t l = lo; l <= hi; l++) {
          ValueEvent* event = ring_buffer_->Get(l);
          event->SetValue(l);
        }
        ring_buffer_->Publish(lo, hi);
      }
    } catch(...) {
      throw std::runtime_error("run failed.");
    }
  }

  void SignalAll() {
    std::unique_lock<std::mutex> lock(mutex_);
    ready_ = true;
    cond_.notify_all();
  }

 private:
  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!ready_)
      cond_.wait(lock);
  }

 private:
  magic_bean::RingBuffer<ValueEvent>* ring_buffer_;
  int64_t iterations_;
  int batch_size_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //VALUE_BATCH_PUBLISHER_H_
