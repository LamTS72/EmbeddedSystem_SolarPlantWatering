#include <Arduino.h>
#include <Wire.h> 
#include <software_timer.h>
#include <string.h>
#include <button.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <LiquidCrystal_I2C.h>



// set the LCD address to 0x27 for a 16 chars and 2 line display 
LiquidCrystal_I2C lcd(0X3F,16,2);  

#define INIT              0
#define READY_AUTO        1
#define PUMPING_AUTO      2
#define STOP_PUMPING_AUTO 3
#define PUMPING_MAN       4
#define STOP_PUMPING_MAN  5

// #define time_display      1800 //3 //1800     //in 5 seconds to ready
// #define time_ready        3     //in 5 seconds to ready
// #define time_pump         600//15     //600   //pump in 10 mins
// #define time_no_pump      21600//21600    //21600 //every 6 hours

//for test
#define time_display      3 
#define time_ready        3    
#define time_pump         5
#define time_no_pump      10

#define PUMP               "Need pump"
#define NO_PUMP            "No pump"


//define pin for sensor soid
int PinSensor1 = A0;
int PinSensor2 = A1;

//define pin for relay
int PinRelay1 = 2;
int PinRelay2 = 3;

//define pin for gnd sensor
int GndSensor1 = 6;
int GndSensor2 = 7;



float Moisture_Sensor1 = 0.0;
float Moisture_Sensor2 = 0.0;



int state = 0;
String pump1_state = NO_PUMP;
String pump2_state = NO_PUMP;


void setup()
{

  lcd.init();                      
  lcd.backlight();
  lcd.clear();
  SWTimer_Init();
  Serial.begin(9600);
  analogReference(EXTERNAL);
  pinMode(PinSensor1, INPUT);
  pinMode(PinSensor2, INPUT);
  pinMode(GndSensor1, INPUT);
  pinMode(GndSensor2, INPUT);


  pinMode(PinRelay1, OUTPUT);
  pinMode(PinRelay2, OUTPUT);

  pinMode(PinButton, INPUT_PULLUP);

  digitalWrite(PinRelay1, HIGH);
  digitalWrite(PinRelay2, HIGH);

  //pinMode(LED_BUILTIN, OUTPUT);

}

//calculate moisture to voltage , int GndSensor)
float calcMoisture(int PinSensor, int GndSensor){
  pinMode(GndSensor, OUTPUT);
  digitalWrite(GndSensor, LOW);
  float value = (float(analogRead(PinSensor))/1023.0)*3.3;
  pinMode(GndSensor, INPUT);
  return value;
}

void pumping(int PinRelay){
  //turn on pump
  digitalWrite(PinRelay, LOW);
}

void stop_pumping(int PinRelay){
  //turn off pump
  digitalWrite(PinRelay, HIGH);
}

String check_4Pumping(float Moisture_value, int SensorNum, int PinRelay ){
  Serial.print("Moisture from sensor ");
  Serial.print(SensorNum);
  Serial.print(": ");
  Serial.print(Moisture_value);
  Serial.println(" V");

  //1.3V and lower -> wet -> no pump
  //2.3V and higher -> fry -> pump
  //2.75V and higher -> not in soil , its of the air -> no need pump
  if(Moisture_value >= 0.0 && Moisture_value <= 3.75){ //2.75
     Serial.println(PUMP);
     return PUMP;
  }  
  else{
    Serial.println(NO_PUMP);
    return NO_PUMP;
  }

}



void update_moisture_state(){
  lcd.setCursor(0,0);
  lcd.print("S1:");
  lcd.setCursor(3,0);
  lcd.print(Moisture_Sensor1);

  lcd.setCursor(9,0);
  lcd.print("S2:");
  lcd.setCursor(12,0);
  lcd.print(Moisture_Sensor2);

  lcd.setCursor(0,1);
  lcd.print("State:");
  lcd.setCursor(7,1);
  lcd.print(state);
}

void display_LCD(){
  if(timer2_flag == true){
      Moisture_Sensor1 = calcMoisture(PinSensor1, GndSensor1);
      Moisture_Sensor2 = calcMoisture(PinSensor2, GndSensor2);
      update_moisture_state();
      setTimer2((uint64_t)time_display*1000);
  }
}

void fsm_automated(){
  switch(state){
    case INIT:
      state = READY_AUTO;
      setTimer1((uint64_t)time_ready*1000);
      break;
    case READY_AUTO:
      update_moisture_state();
      if(timer1_flag == true){
        pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
        pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );
        if(pump1_state == PUMP ||  pump2_state == PUMP){
          state = PUMPING_AUTO;
          setTimer1((uint64_t)time_pump*1000);
        }
        else{
          state = STOP_PUMPING_AUTO;
          setTimer1((uint64_t)time_no_pump*1000);
        }
      }
      break;
    case PUMPING_AUTO:
      update_moisture_state();
      if(pump1_state == PUMP){
        pumping(PinRelay1);
      }
      if(pump2_state == PUMP){
        pumping(PinRelay2);
      }

      if(isBUTTON_Pressed() == 1){
        state = STOP_PUMPING_MAN;
      }
      if(timer1_flag == true){
        state = STOP_PUMPING_AUTO;
        setTimer1((uint64_t)time_no_pump*1000);
      }
      break;
    case STOP_PUMPING_AUTO:
      update_moisture_state();
      stop_pumping(PinRelay1);
      stop_pumping(PinRelay2);
      pump1_state = NO_PUMP;
      pump2_state = NO_PUMP;

      if(isBUTTON_Pressed() == 1){
        pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
        pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );
        state = PUMPING_MAN;
      }
      
      if(timer1_flag == true){
        state = READY_AUTO;
        setTimer1((uint64_t)time_ready*1000);
      }
      break;
  }
}


void fsm_manual(){
  switch(state){
    case PUMPING_MAN:
      update_moisture_state();
      if(pump1_state == PUMP){
        pumping(PinRelay1);
      }
      if(pump2_state == PUMP){
        pumping(PinRelay2);
      }
      if(isBUTTON_Pressed() == 1){
        state = STOP_PUMPING_MAN;
        setTimer1((uint64_t)time_no_pump*1000);
      }
      break;
    case STOP_PUMPING_MAN:
      update_moisture_state();
      stop_pumping(PinRelay1);
      stop_pumping(PinRelay2);
      pump1_state = NO_PUMP;
      pump2_state = NO_PUMP;
      if(isBUTTON_Pressed() == 1){
        pump1_state = check_4Pumping(Moisture_Sensor1, 1, PinRelay1 );
        pump2_state = check_4Pumping(Moisture_Sensor2, 2, PinRelay2 );    
        state = PUMPING_MAN;    
      }
      if(timer1_flag == true){
        state = READY_AUTO;
        setTimer1((uint64_t)time_ready*1000);
      }
      break;
  }
}


void test_case(){
    if(timer1_flag == true){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    setTimer1(5*1000);
  }
}

void loop()
{
  display_LCD();
  fsm_automated();
  fsm_manual();
}