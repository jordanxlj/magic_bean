#ifndef FIZZ_BUZZ_EVENT_H_
#define FIZZ_BUZZ_EVENT_H_

#include "event_factory.h"

class FizzBuzzEvent {
 public:
  FizzBuzzEvent() {};
  int64_t GetValue() const { return value_; }
  void SetValue(int64_t value) { value_ = value; }

  bool IsFizz() { return fizz_; }
  void SetFizz(bool fizz) { fizz_ = fizz; }

  bool IsBuzz() { return buzz_; }
  void SetBuzz(bool buzz) { buzz_ = buzz; }

 private:
  bool fizz_;
  bool buzz_;
  int64_t value_;
};

class FizzBuzzEventFactory : public magic_bean::EventFactory<FizzBuzzEvent> {
 public:
  FizzBuzzEventFactory() {}
  ~FizzBuzzEventFactory() {}

  virtual FizzBuzzEvent* NewInstance() override {
    return new FizzBuzzEvent();
  };
};

#endif //FIZZ_BUZZ_EVENT_H_
