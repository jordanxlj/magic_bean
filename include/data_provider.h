#ifndef DATA_PROVIDER_H_
#define DATA_PROVIDER_H_

#include <cstdint>

namespace magic_bean {

template<typename T>
class DataProvider {
 public:
  virtual ~DataProvider() {};
  virtual T* Get(int64_t sequence) = 0;
};

} //end namespace

#endif //DATA_PROVIDER_H_
