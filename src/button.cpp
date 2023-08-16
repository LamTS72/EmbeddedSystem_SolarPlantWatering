#include <button.h>

int PinButton = 4;

int KeyReg0 = NORMAL_STATE;
int KeyReg1 = NORMAL_STATE;
int KeyReg2 = NORMAL_STATE;
int KeyReg3 = NORMAL_STATE;
int TimerForKeyPress = 20;

int button_flag = 0;


int isBUTTON_Pressed(){
	if(button_flag == 1){
		button_flag = 0;
		Serial.print("Button is pressed\n");
		return 1;
	}
	return 0;
}

void subKeyProcess( ){
	button_flag = 1;
}

void getKeyInput(){
	KeyReg0 = KeyReg1;
	KeyReg1 = KeyReg2;
	KeyReg2 = digitalRead(PinButton);
	if ((KeyReg0 == KeyReg1) && (KeyReg1 == KeyReg2)) {
		if (KeyReg3 != KeyReg2){
			KeyReg3 = KeyReg2;
			if (KeyReg2 == PRESSED_STATE){
				TimerForKeyPress = 100;
				subKeyProcess();
			}
		}
		else {
			TimerForKeyPress--;
			if (TimerForKeyPress == 0){
				KeyReg3 = NORMAL_STATE;
				TimerForKeyPress = 100;
			}
		}
	}
}