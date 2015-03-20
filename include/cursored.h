#ifndef CURSORED_H_
#define CURSORED_H_

#include <cstdint>

namespace magic_bean {

class Cursored {
 public:
  virtual ~Cursored() {};
  virtual int64_t GetCursor() = 0;
};

} //end namespace

#endif //CURSORED_H_
