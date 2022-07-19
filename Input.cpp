#include "Input.h";

Input::Input() {}

long Input::getValue() {
  return this->_value;
}

bool Input::btnAisPressed() {
  return M5.BtnA.wasReleased();
}

bool Input::btnBisPressed() {
  return M5.BtnB.wasReleased();
}

bool Input::btnAisPressed(int ms) {
  return M5.BtnA.wasReleasefor(ms);
}

bool Input::btnBisPressed(int ms) {
  return M5.BtnB.wasReleasefor(ms);
}

void Input::updateValue() {
  this->_value++;
}
