#ifndef FUNCTION_EVENT_HANDLER_H_
#define FUNCTION_EVENT_HANDLER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "function_event.h"

enum class FunctionStep {
  ONE,
  TWO,
  THREE
};

class FunctionEventHandler : public magic_bean::EventHandler<FunctionEvent> {
 public:
  explicit FunctionEventHandler(FunctionStep step)
    : function_step_(step) {}

  void Reset(int64_t expected_count) {
    step_three_counter_.Set(0);
    count_ = expected_count;
    ready_ = false;
  }

  int64_t GetStepThreeCounter() { return step_three_counter_.Get(); }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!ready_)
      cond_.wait(lock);
  }

  virtual void OnEvent(FunctionEvent* event, int64_t sequence, bool end_of_batch) {
    switch(function_step_) {
    case FunctionStep::ONE:
      event->SetStepOneResult(event->GetOperandOne() + event->GetOperandTwo());
      break;
    case FunctionStep::TWO:
      event->SetStepTwoResult(event->GetStepOneResult() + 3);
      break;
    case FunctionStep::THREE:
      if((event->GetStepTwoResult() & 4) == 4)
        step_three_counter_.Set(step_three_counter_.Get() + 1);
      break;
    }

    if(count_ == sequence) {
      std::unique_lock<std::mutex> lock(mutex_);
      ready_ = true;
      cond_.notify_all();
    }
  }

 private:
  FunctionStep function_step_;
  PaddedLong step_three_counter_;
  int64_t count_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //FUNCTION_EVENT_HANDLER_H_
