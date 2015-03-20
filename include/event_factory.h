#ifndef EVENT_FACTORY_H_
#define EVENT_FACTORY_H_

namespace magic_bean {

template<typename T>
class EventFactory {
 public:
  virtual ~EventFactory() {};
  virtual T* NewInstance() = 0;
};

} //end namespace

#endif //EVENT_FACTORY_H_
