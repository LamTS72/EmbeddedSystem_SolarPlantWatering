#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <Arduino.h>

#define NORMAL_STATE  1
#define PRESSED_STATE 0

extern int button_flag;
extern int PinButton;


int isBUTTON_Pressed();
void getKeyInput();


#endif /* INC_BUTTON_H_ */
