#ifndef LONG_ARRAY_EVENT_HANDLER_H_
#define LONG_ARRAY_EVENT_HANDLER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"

class LongArrayEventHandler : public magic_bean::EventHandler<int64_t> {
 public:
  LongArrayEventHandler() {};
  int64_t GetValue() { return value_.Get(); }
  void Reset(int64_t expected_count) {
    value_.Set(0);
    count_ = expected_count;
    ready_ = false;
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!ready_)
      cond_.wait(lock);
  }

  virtual void OnEvent(int64_t* event, int64_t sequence, bool end_of_batch) {
    for(int i = 0; i < 3; i++)
      value_.Set(value_.Get() + event[3]);
    if(--count_ == 0) {
      std::unique_lock<std::mutex> lock(mutex_);
      ready_ = true;
      cond_.notify_all();
    }
  }

 private:
  int64_t count_;
  PaddedLong value_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //LONG_ARRAY_EVENT_HANDLER_H_
