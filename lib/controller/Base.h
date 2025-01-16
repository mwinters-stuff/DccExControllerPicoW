#ifndef _BASE_H_
#define _BASE_H_

class Base {
public:

  virtual void setup() = 0;
  virtual void loop() = 0;

protected:
  ~Base() = default;
  Base() = default;
};

#endif