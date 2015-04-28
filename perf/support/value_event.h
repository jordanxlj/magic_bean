#ifndef VALUE_EVENT_H_
#define VALUE_EVENT_H_

#include "event_factory.h"

class ValueEvent {
 public:
  ValueEvent() : value_(-1) {};
  int64_t GetValue() const { return value_; };
  void SetValue(int64_t value) { value_ = value; };

 private:
  int64_t value_;
};

class ValueEventFactory : public magic_bean::EventFactory<ValueEvent> {
 public:
  ValueEventFactory() {};
  ~ValueEventFactory() {};

  virtual ValueEvent* NewInstance() override {
    return new ValueEvent();
  };
};

#endif //VALUE_EVENT_H_
