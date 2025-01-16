#ifndef _SINGLETON_HOLDER_H_
#define _SINGLETON_HOLDER_H_

#include "Base.h"

class SingletonHolder: public Base {
public:
  void setup() override;
  void loop() override;
};

#endif