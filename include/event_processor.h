#ifndef EVENT_PROCESSOR_H_
#define EVENT_PROCESSOR_H_

#include "magic_types.h"

namespace magic_bean {

class EventProcessor {
 public:
  virtual ~EventProcessor() {};

  virtual SequencePtr GetSequence() = 0;
  virtual void Halt() = 0;
  virtual bool IsRunning() = 0;
  virtual void Run() = 0;
};

} //end namespace

#endif //EVENT_PROCESSOR_H_
