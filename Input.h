#ifndef _INPUT_H
#define _INPUT_H

#include <M5StickCPlus.h>

typedef class Input {
  public:
    Input();
    void init(void (*)(void));
    long getValue();
    bool btnAisPressed();
    bool btnAisPressed(int);
    bool btnBisPressed();
    bool btnBisPressed(int);
    void updateValue();
  private:
    unsigned long _value = 0; 
} Input;

#endif
