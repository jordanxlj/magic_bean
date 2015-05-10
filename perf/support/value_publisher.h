#ifndef VALUE_PUBLISHER_H_
#define VALUE_PUBLISHER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "ring_buffer.h"
#include "value_event.h"

class ValuePublisher {
 public:
 ValuePublisher(magic_bean::RingBuffer<ValueEvent>* ring_buffer,
                int64_t iterations)
    : ring_buffer_(ring_buffer)
    , iterations_(iterations) {}

  void Run() {
    try {
      Wait();

      for(int64_t i = 0; i < iterations_; i++) {
        int64_t sequence = ring_buffer_->Next();
        ValueEvent* event = ring_buffer_->Get(sequence);
        event->SetValue(i);
        ring_buffer_->Publish(sequence);
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

#endif //VALUE_PUBLISHER_H_
