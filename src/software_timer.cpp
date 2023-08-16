#include <software_timer.h>


#define INTERRUPT_CYCLE			10 // 10 milliseconds
#define PRESCALER				64
#define COUNTER_START 			(65536 - 2500) // 16M is Core Clock
static void TIMER_Init();


#define TICK 10
uint64_t timer1_counter = 0;
int timer1_flag = 0;

uint64_t timer2_counter = 0;
int timer2_flag = 0;

void setTimer1(uint64_t duration){
	timer1_counter = duration/TICK;
	timer1_flag = 0;
}

void setTimer2(uint64_t duration){
	timer2_counter = duration/TICK;
	timer2_flag = 0;
}


void timerRun(){
	if(timer1_counter > 0){
		timer1_counter--;
		if(timer1_counter <= 0){
			timer1_flag = 1;
            Serial.print("Change FSM\n");
		}
	}
    if(timer2_counter > 0){
        timer2_counter--;
        if(timer2_counter <= 0){
            timer2_flag = 1;
            Serial.print("Update LCD\n");
        }
    }   
}

void SWTimer_Init(void){
	TIMER_Init();
    setTimer1(15);
    setTimer2(15);
}

// Init TIMER 10ms
static void TIMER_Init(){
	cli();          					    // Disable interrupts                       
    
    /* Reset Timer/Counter1 */
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 = 0;
    
    /* Setup Timer/Counter1 */
    TCCR1B |= (1 << CS11) | (1 << CS10);    // prescale = 64
    TCNT1 = COUNTER_START;				    // 65536 - 10000/(64/16)
    TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable 
    sei();                                  // Enable interrupts
}

ISR (TIMER1_OVF_vect) 
{
    TCNT1 = COUNTER_START;
	//SCH_Update();
    timerRun();
    getKeyInput();
}