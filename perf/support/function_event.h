#ifndef FUNCTION_EVENT_H_
#define FUNCTION_EVENT_H_

#include "event_factory.h"

class FunctionEvent {
 public:
  FunctionEvent() {};

  int64_t GetOperandOne() { return operand_one_; }
  void SetOperandOne(int64_t operand_one) { operand_one_ = operand_one; }
  int64_t GetOperandTwo() { return operand_two_; }
  void SetOperandTwo(int64_t operand_two) { operand_two_ = operand_two; }

  int64_t GetStepOneResult() { return step_one_result_; }
  void SetStepOneResult(int64_t step_one_result) { step_one_result_ = step_one_result; }
  int64_t GetStepTwoResult() { return step_two_result_; }
  void SetStepTwoResult(int64_t step_two_result) { step_two_result_ = step_two_result; }

 private:
  int64_t operand_one_;
  int64_t operand_two_;
  int64_t step_one_result_;
  int64_t step_two_result_;
};

class FunctionEventFactory : public magic_bean::EventFactory<FunctionEvent> {
 public:
  FunctionEventFactory() {};
  ~FunctionEventFactory() {};

  virtual FunctionEvent* NewInstance() override {
    return new FunctionEvent();
  };
};

#endif //FUNCTION_EVENT_H_
