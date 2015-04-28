#ifndef VALUE_ADDITION_EVENT_HANDLER_H_
#define VALUE_ADDITION_EVENT_HANDLER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "value_event.h"

class ValueAdditionEventHandler : public magic_bean::EventHandler<ValueEvent> {
 public:
  ValueAdditionEventHandler() {};
  int64_t GetValue() { return value_.Get(); }
  void Reset(int64_t expected_count) { count_ = expected_count; }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock);
  }

  virtual void OnEvent(ValueEvent* event, int64_t sequence, bool end_of_batch) {
    value_.Set(value_.Get() + event->GetValue());
    if(count_ == sequence) {
      std::unique_lock<std::mutex> lock(mutex_);
      cond_.notify_all();
    }
  }

 private:
  int64_t count_;
  PaddedLong value_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif //VALUE_ADDITION_EVENT_HANDLER_H_
