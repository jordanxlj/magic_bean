#ifndef STUB_EVENT_H_
#define STUB_EVENT_H_

#include "event_factory.h"
#include "event_translator_one_arg.h"

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

class StubEventTranslator : public EventTranslatorOneArg<StubEvent, int> {
 public:
  StubEventTranslator() {};
  ~StubEventTranslator() {};

  virtual void TranslateTo(StubEvent* event, int64_t sequence, int arg0) override {
    event->SetValue(arg0);
  }
};

#endif //STUB_EVENT_H_
