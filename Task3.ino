#include<avr/interrupt.h>
#include<util/delay.h>

//declaring variables
int i = 1; // the number of cars that reached LB_1 +1 (starting at one because can't have empty array)
int j = 1; // the number of cars that reached LB_2 +1(starting at one because can't have empty array)
int count;
int breached;
uint8_t velocity;
int LB1_state = 1;
int LB2_state = 1;
int dutyCycle;
int  timer_1_state;
volatile int timer2count;
uint16_t startTime[100];
uint16_t endTime[100];
uint16_t timeElapsed[100];


//use 16-bit timer 1 to generate PWM
ISR(TIMER1_COMPA_vect) {
  timer_1_state = ~timer_1_state;
}

//use 8-bit timer 2 for light blinks
ISR(TIMER2_COMPA_vect) {
  //iterate timer 2 count each time the interrupt is triggered
  timer2count++;
  //if timer2count is greater than 122, 200ms or one period for light blink has passed
  //thus clear timer2count and increase blink count
  if (timer2count > 122) {
    timer2count = 0;
    count++;
  }
}

//external interrupt triggered by LB2 pushbutton
ISR(INT0_vect) {
  LB2_state = ~LB2_state;//toggle button state
}

//external interrupt  triggered by LB1 pushbutton
ISR(INT1_vect) {
  LB1_state = ~LB1_state;//toggle button state
}

void setup() {
}

void loop() {
  DDRB |= (1 << DDB3); //set pin 11(RED) of port B to output
  DDRB |= (1 << DDB4); //set pin 12(GREEN) of port B to output
  DDRB |= (1 << DDB1); //set pin 9 of port B to PWM output

  breached = 0; //set LB1 breached to zero
  startTime[100] = 0;
  endTime[100] = 0;
  timeElapsed[100] = 0; // set the arrays to zero
  timer_1_state = 1;
  timer2count = 0; //set the timer counts and states to default

  cli(); //disable all interrupts during configuration

  // Configure external interrupts using EICRA and EIMSK
  EICRA |= (1 << ISC10) | (1 << ISC11); //rising edge of INT1 generates interrupt request
  EICRA |= (1 << ISC00) | (1 << ISC01); //rising edge of INT0 generates interrupt request
  EIMSK |= (1 << INT1) | (1 << INT0); //enable the two external interrupts, INT0 and INT1

  //TIMER 1 FOR OSCILLOSCOPE
  TCCR1A = (1 << COM1A1) + (1 << COM1B1) + (1 << WGM11); //set the timer to Fast PWM mode 14
  TCCR1B = (1 << WGM13) + (1 << WGM12); //set OC1A at top
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TCCR1B &= ~(1 << CS11);  //prescaler to 1024
  ICR1 = 15624; //one second delay

  //TIMER 2 FOR BLINKS
  TCCR2A |= (1 << WGM20);
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << WGM22); //set the timer to fast pwm mode 7
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); //prescaler to 1024
  TCNT2 = 0;   //initialize timer to 0
  OCR2A = 30;   //1 second is 15624 counts, 100 ms is 1562 counts, which is 52 of 30 counts
  TIMSK2 |= (1 << OCIE2A); //enable CTC interrupt for OCR2A compare match

  sei();//enable all interrupt requests

  while (1) {

    //if LB1 button is not pressed
    if (LB1_state == 1) {
      PORTB &= ~(1 << PB3); //turning red light off
      PORTB &= ~(1 << PB1); //turning green light off
    }

    //else if only LB2 button is pressed at the start, toggle LB2 state back to default
    else if (LB2_state != 1 && j >= i) {
      LB2_state = ~ LB2_state;
    }

    // if LB1 button is prpessed
    else if (LB1_state != 1) {
      startTime[i] = millis(); // Add the current starting time to the array
      i++;// add one to the cars count
      count = 0;
      TCNT1 = 0; //clear timer
      timer2count = 0;
      // Blink red LED blink once with 200 ms period
      while (count < 1) {
        if (timer2count > 52) {
          PORTB |= (1 << PB3); //turning red LED on
        } else {
          PORTB &= ~(1 << PB3); //turning red LED off
        }
      }
      PORTB &= ~(1 << PB3); //turning red LED off.
      LB1_state = 1; //set LB1_state back to default.
      breached = 1; //flag LB1 as breached.
    }

    //if LB2 is pressed, and LB1 was breached, and number of cars that have reached LB2 is less than
    // the number of cars that have reached LB1
    if (LB2_state != 1 && breached == 1 && j < i) {
      endTime[j] = millis(); //Add the current end time to the array
      timeElapsed[j] = (endTime[j] - startTime[j]);//Store elapsed time in array

      count = 0;
      timer2count = 0;
      TCNT1 = 0; //clear timer

      //blink once with 200ms period
      while (count < 1) {
        if (timer2count > 52) {
          PORTB &= ~(1 << PB4); //turning green LED off
        } else {
          PORTB |= (1 << PB4); //turning green LED on
        }
      }
      PORTB &= ~(1 << PB4); //turning green LED off

      velocity = 72000 / timeElapsed[j]; //estimated velocity in km/hr
      //if velocity is higher than 100 km/hr, set duty cycle to be 100%
      if (velocity >= 100) {
        OCR1A = 15624;
        OCR1B = 0;
      } else {
        // Else set dutycycle proportional to the velocity of the car
        dutyCycle = velocity * 10;
        OCR1A = 15624.00 * dutyCycle / 1000.00; // Set CTC compare value to be duty cycle
        OCR1B = 15624.00 - OCR1A;
      }
      j++; //iterate number of cars passing LB_2
      LB2_state = 1; //set LB2 state back to default.
    }
  }

}
