#ifndef LONG_ARRAY_PUBLISHER_H_
#define LONG_ARRAY_PUBLISHER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "ring_buffer.h"
#include "value_event.h"

class LongArrayPublisher {
 public:
 LongArrayPublisher(magic_bean::RingBuffer<int64_t>* ring_buffer,
                    int64_t iterations)
    : ring_buffer_(ring_buffer)
    , iterations_(iterations) {}

  void Run() {
    try {
      Wait();

      for(int64_t i = 0; i < iterations_; i++) {
        int64_t sequence = ring_buffer_->Next();
        int64_t* event = ring_buffer_->Get(sequence);
        for(int j = 0; j < 3; j++)
          event[j] = i + j;
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
  magic_bean::RingBuffer<int64_t>* ring_buffer_;
  int64_t iterations_;
  int batch_size_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //LONG_ARRAY_PUBLISHER_H_
