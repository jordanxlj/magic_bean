#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

namespace magic_bean {

template<typename T>
class EventHandler {
 public:
  virtual void OnEvent(const T& event, int64_t sequence, bool end_of_batch) = 0;
};

} //end namespace

#endif //EVENT_HANDLER_H_
