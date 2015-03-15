#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

template<typename T>
class EventHandler {
 public:
  virtual void OnEvent(const T& event, uint64_t sequence, bool end_of_batch) = 0;
};

#endif //EVENT_HANDLER_H_
