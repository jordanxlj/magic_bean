#ifndef EVENT_TRANSLATOR_H_
#define EVENT_TRANSLATOR_H_

namespace magic_bean {

template<tyename T>
class EventTranslator {
 public:
  virtual ~EventTranslator() {};
  virtual TranslateTo(T* event, int64_t sequence) = 0;
};

} //end namespace

#endif //EVENT_TRANSLATOR_H_
