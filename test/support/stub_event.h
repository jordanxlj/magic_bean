#ifndef STUB_EVENT_H_
#define STUB_EVENT_H_

#include "event_factory.h"

using namespace magic_bean;

class StubEvent {
 public:
  StubEvent(int i) : value_(i) {};
  StubEvent() : value_(-1) {};
  int GetValue() const { return value_; };
  void SetValue(int value) { value_ = value; };

 private:
  int value_;
};

class StubEventFactory : public EventFactory<StubEvent> {
 public:
  StubEventFactory() {};
  ~StubEventFactory() {};

  virtual StubEvent* NewInstance() override {
    return new StubEvent(-1);
  };
};

#endif //STUB_EVENT_H_
