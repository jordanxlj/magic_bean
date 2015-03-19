#ifndef INSUFFICIENT_CAPACITY_EXCEPTION_H_
#define INSUFFICIENT_CAPACITY_EXCEPTION_H_

#include <string>

namespace magic_bean{

class InsufficientCapacityException {
 public:
  explicit InsufficientCapacityException(const std::string& msg);
};

} //end namespace

#endif //INSUFFICIENT_CAPACITY_EXCEPTION_H_
