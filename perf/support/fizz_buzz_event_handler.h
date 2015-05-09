#ifndef FIZZ_BUZZ_EVENT_HANDLER_H_
#define FIZZ_BUZZ_EVENT_HANDLER_H_

#include <mutex>
#include <condition_variable>
#include "event_handler.h"
#include "padded_long.h"
#include "fizz_buzz_event.h"

enum class FizzBuzzStep {
  FIZZ,
  BUZZ,
  FIZZ_BUZZ,
};

class FizzBuzzEventHandler : public magic_bean::EventHandler<FizzBuzzEvent> {
 public:
  explicit FizzBuzzEventHandler(FizzBuzzStep step)
    : fizz_buzz_step_(step) {}

  void Reset(int64_t expected_count) {
    fizz_buzz_counter_.Set(0);
    count_ = expected_count;
    ready_ = false;
  }

  int64_t GetFizzBuzzCounter() { return fizz_buzz_counter_.Get(); }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!ready_)
      cond_.wait(lock);
  }

  virtual void OnEvent(FizzBuzzEvent* event, int64_t sequence, bool end_of_batch) {
    switch(fizz_buzz_step_) {
    case FizzBuzzStep::FIZZ:
      if(event->GetValue() % 3 == 0) {
        event->SetFizz(true);
      }
      break;
    case FizzBuzzStep::BUZZ:
      if(event->GetValue() % 5 == 0) {
        event->SetBuzz(true);
      }
      break;
    case FizzBuzzStep::FIZZ_BUZZ:
      if(event->IsFizz() && event->IsBuzz()) {
        fizz_buzz_counter_.Set(fizz_buzz_counter_.Get() + 1);
      }
      break;
    }
    if(count_ == sequence) {
      std::unique_lock<std::mutex> lock(mutex_);
      ready_ = true;
      cond_.notify_all();
    }
  }

 private:
  FizzBuzzStep fizz_buzz_step_;
  PaddedLong fizz_buzz_counter_;
  int64_t count_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool ready_;
};

#endif //FIZZ_BUZZ_EVENT_HANDLER_H_
