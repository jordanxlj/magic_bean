#ifndef VALUE_MUTATION_EVENT_HANDLER_H_
#define VALUE_MUTATION_EVENT_HANDLER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "value_event.h"

enum class Operation {
  ADDITION,
  SUBSTRACTION,
  AND
};

class ValueMutationEventHandler : public magic_bean::EventHandler<ValueEvent> {
 public:
  explicit ValueMutationEventHandler(Operation operation)
    : operation_(operation) {}

  void Reset(int64_t expected_count) {
    value_.Set(0);
    count_ = expected_count;
    ready_ = false;
  }

  int64_t GetValue() { return value_.Get(); }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!ready_)
      cond_.wait(lock);
  }

  virtual void OnEvent(ValueEvent* event, int64_t sequence, bool end_of_batch) {
    switch(operation_) {
    case Operation::ADDITION:
      value_.Set(value_.Get() + event->GetValue());
      break;
    case Operation::SUBSTRACTION:
      value_.Set(value_.Get() - event->GetValue());
      break;
    case Operation::AND:
      value_.Set(value_.Get() & event->GetValue());
      break;
    }

    if(count_ == sequence) {
      std::unique_lock<std::mutex> lock(mutex_);
      ready_ = true;
      cond_.notify_all();
    }
  }

 private:
  Operation operation_;
  PaddedLong value_;
  int64_t count_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //VALUE_MUTATION_EVENT_HANDLER_H_
