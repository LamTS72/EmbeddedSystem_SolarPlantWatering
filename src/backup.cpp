// #include <Arduino.h>
// #include <Wire.h> 
// #include <software_timer.h>
// #include <string.h>
// #include <button.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <scheduler.h>

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3C 
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// #define INIT              0
// #define READY_AUTO        1
// #define PUMPING_AUTO      2
// #define STOP_PUMPING_AUTO 3
// #define PUMPING_MAN       4
// #define STOP_PUMPING_MAN  5

// #define time_display      3//1800     //in 5 seconds to ready
// #define time_ready        3     //in 5 seconds to ready
// #define time_pump         5//15     //600   //pump in 10 mins
// #define time_no_pump      5//21600    //21600 //every 6 hours

// #define PUMP               "Need pump"
// #define NO_PUMP            "No pump"
// // set the LCD address to 0x27 for a 16 chars and 2 line display 


// //define pin for sensor soid
// int PinSensor1 = A0;
// int PinSensor2 = A1;

// //define pin for relay
// int PinRelay1 = 2;
// int PinRelay2 = 3;


// float Moisture_Sensor1 = 0.0;
// float Moisture_Sensor2 = 0.0;

// int state = 0;
// String pump1_state = NO_PUMP;
// String pump2_state = NO_PUMP;
// // bool flag_run = false;
// // bool flag_stop = false;


// void setup()
// {
//   display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setTextColor(WHITE);

//   SWTimer_Init();
//   Serial.begin(9600);
//   analogReference(EXTERNAL);
//   pinMode(PinSensor1, INPUT);
//   pinMode(PinSensor2, INPUT);

//   pinMode(PinRelay1, OUTPUT);
//   pinMode(PinRelay2, OUTPUT);

//   pinMode(PinButton, INPUT_PULLUP);

//   digitalWrite(PinRelay1, HIGH);
//   digitalWrite(PinRelay2, HIGH);

//   //pinMode(LED_BUILTIN, OUTPUT);

// }

// //calculate moisture to voltage
// float calcMoisture(int PinSensor){
//   return (float(analogRead(PinSensor))/1023.0)*3.3;
// }

// void pumping(int PinRelay){
//   //turn on pump
//   digitalWrite(PinRelay, LOW);
// }

// void stop_pumping(int PinRelay){
//   //turn off pump
//   digitalWrite(PinRelay, HIGH);
// }

// String check_4Pumping(float Moisture_value, int SensorNum, int PinRelay ){
//   Serial.print("Moisture from sensor ");
//   Serial.print(SensorNum);
//   Serial.print(": ");
//   Serial.print(Moisture_value);
//   Serial.println(" V");

//   //1.3V and lower -> wet -> no pump
//   //2.3V and higher -> fry -> pump
//   //2.75V and higher -> not in soil , its of the air -> no need pump
//   if(Moisture_value >= 0.0 && Moisture_value <= 3.75){ //2.75
//      Serial.println(PUMP);
//      return PUMP;
//   }  
//   else{
//     Serial.println(NO_PUMP);
//     return NO_PUMP;
//   }
//   // else{
//   //   Serial.println("Sensor is not in soil");
//   //   return "Sensor is not in soil";
//   // }
// }
// // void clear_state(){
// //     display.setCursor(80,40);
// //     display.println("");
// //     display.display();
// // }

// void show_state(int state){
//     display.setCursor(0,40);
//     display.println("State:");
//     // display.setCursor(80,40);
//     // display.println("");
//     display.setCursor(80,40);
//     display.println(state);
    
//     display.display();
// }

// void display_LCD(){
//   if(timer2_flag == true){
//       Moisture_Sensor1 = calcMoisture(PinSensor1);
//       Moisture_Sensor2 = calcMoisture(PinSensor2);
//       display.setCursor(0,0);
//       display.println("S1:");
//       display.setCursor(40,0);
//       display.println(Moisture_Sensor1);

//       display.setCursor(0,20);
//       display.println("S2:");
//       display.setCursor(40,20);
//       display.println(Moisture_Sensor2);
//       display.display();

//       setTimer2((uint64_t)time_display*1000);
//   }
// }

// void fsm_automated(){
//   switch(state){
//     case INIT:
//       state = READY_AUTO;
//       setTimer1((uint64_t)time_ready*1000);
//       break;
//     case READY_AUTO:
//       show_state(state);
//       if(timer1_flag == true){
//         pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
//         pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );
//         if(pump1_state == PUMP ||  pump2_state == PUMP){
//           state = PUMPING_AUTO;
//           setTimer1((uint64_t)time_pump*1000);
//         }
//         else{
//           state = STOP_PUMPING_AUTO;
//           setTimer1((uint64_t)time_no_pump*1000);
//         }
//       }
//       break;
//     case PUMPING_AUTO:
//       show_state(state);
//       if(pump1_state == PUMP){
//         pumping(PinRelay1);
//       }
//       if(pump2_state == PUMP){
//         pumping(PinRelay2);
//       }

//       if(isBUTTON_Pressed() == 1){
//         state = STOP_PUMPING_MAN;
//       }
//       if(timer1_flag == true){
//         state = STOP_PUMPING_AUTO;
//         setTimer1((uint64_t)time_no_pump*1000);
//       }
//       break;
//     case STOP_PUMPING_AUTO:
//       show_state(state);
//       stop_pumping(PinRelay1);
//       stop_pumping(PinRelay2);
//       pump1_state = NO_PUMP;
//       pump2_state = NO_PUMP;

//       if(isBUTTON_Pressed() == 1){
//         pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
//         pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );
//         if(pump1_state == PUMP ||  pump2_state == PUMP){
//           state = PUMPING_MAN;
//         }
//       }
      
//       if(timer1_flag == true){
//         state = READY_AUTO;
//         setTimer1((uint64_t)time_ready*1000);
//       }
//       break;
//   }
// }


// void fsm_manual(){
//   switch(state){
//     case PUMPING_MAN:
//       show_state(state);
//       if(pump1_state == PUMP){
//         pumping(PinRelay1);
//       }
//       if(pump2_state == PUMP){
//         pumping(PinRelay2);
//       }
//       if(isBUTTON_Pressed() == 1){
//         state = STOP_PUMPING_MAN;
//         setTimer1((uint64_t)time_no_pump*1000);
//       }
//       break;
//     case STOP_PUMPING_MAN:
//       show_state(state);
//       stop_pumping(PinRelay1);
//       stop_pumping(PinRelay2);
//       pump1_state = NO_PUMP;
//       pump2_state = NO_PUMP;
//       if(isBUTTON_Pressed() == 1){
//         pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
//         pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );        
//         if(pump1_state == PUMP ||  pump2_state == PUMP){
//           state = PUMPING_MAN;
//         }
//       }
//       if(timer1_flag == true){
//         state = READY_AUTO;
//         setTimer1((uint64_t)time_ready*1000);
//       }
//       break;
//   }
// }


// void test_case(){
//     if(timer1_flag == true){
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     setTimer1(5*1000);
//   }
// }

// void loop()
// {
//   display_LCD();
//   fsm_automated();
//   fsm_manual();


//   // unsigned long curr_miliseconds = millis();
//   // if(curr_miliseconds - prev_miliseconds >= time_interval || prev_miliseconds == 0){
//   //   prev_miliseconds = curr_miliseconds;
//   //   Moisture_Sensor1 = calcMoisture(PinSensor1);
//   //   Moisture_Sensor2 = calcMoisture(PinSensor2);
//   //   Serial.print("Soil Moisture: ");
//   //   Serial.println(Moisture_Sensor1);
//   // }
//   // if(isBUTTON_Pressed() == true){
//   //   digitalWrite(LED_BUILTIN, !digitalRead(13));
//   // }

// }