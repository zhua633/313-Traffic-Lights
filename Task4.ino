#include <util/delay.h>
#include <avr/interrupt.h>

//declaring variables
uint8_t timer_2_state;
uint8_t buttonState;
volatile int count;
uint8_t timer2count;
uint8_t countCar;
//uint16_t
volatile int timer0count = 0;

//use 8-bit timer 0 for the white blinks
ISR(TIMER0_COMPA_vect) {
  timer0count++;
  if (timer0count > 122) {
    timer0count = 0;
    count++;
  }
}

//use 8-bit timer 2 for ryg lights.
ISR(TIMER2_COMPA_vect) {
  timer2count++;

  if (timer2count == 100) { //means 1 second has passed
    if (timer_2_state == 3) {
      timer_2_state = 1;
      timer2count = 0;//if timer state is 3 set it back to 1 and clear timer count
    } else {
      timer_2_state++;
      timer2count  = 0; //else iterate timer state and clear timer count
    }
  }
}

//button interrupt
ISR(INT0_vect) {
  buttonState = ~buttonState;//toggle button state
}

void setup() {
}

void loop() {
  DDRB |= (1 << DDB1); //set pin 9 of port B to output (OSCILLOSCOPE)
  DDRB |= (1 << DDB3); // set pin 11 of port B to output (RED LIGHT)
  DDRB |= (1 << DDB5); // set pin 13 of port B to output (YELLOW LIGHT)
  DDRB |= (1 << DDB4); // set pin 12 of port B to output (GREEN LIGHT)
  DDRB |= (1 << DDB0); // set pin 8 of port B to output (WHITE LIGHT)

  //set default states
  buttonState = 1;
  //  timer_0_state = 1;
  timer_2_state = 1;
  timer2count = 0;
  countCar = 0;
  count = 0;

 // cli();  //disable all interuppts during configuration

  EIMSK |= (1 << INT0); //enables one external interrupt INT0.
  EICRA |= (1 << ISC00);
  EICRA |= (1 << ISC01); //rising edge of INT0 generates an external interrupt request.


  //TIMER 0 FOR THE WHITE LIGHT BLINKS
  TCCR0A &= ~(1 << WGM00);
  TCCR0A |= (1 << WGM01);
  TCCR0B &= ~(1 << WGM02); //set the timer to CTC mode
  TCCR0B |= (1 << CS02) | (1 << CS00);
  TCCR0B &= ~(1 << CS01);  //prescaler to 1024
  TCNT0 = 0;   //initialize timer to 0
  OCR0A = 32; //1 second is 15624 counts, 125 ms is 1953 counts, which is 32 of 61 counts
  TIMSK0 |= (1 << OCIE0A); //enable CTC interrupt for OCR0A compare match

  //TIMER 1 FOR PWM GENERATION
  TCCR1A = (1 << COM1A1) + (1 << COM1B1) + (1 << WGM11); //set the timer to Fast PWM mode 14
  TCCR1B = (1 << WGM13) + (1 << WGM12); //set OC1A at bottom, icr1 at top
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TCCR1B &= ~(1 << CS11);  //prescaler to 1024
  ICR1 = 15624; //one second delay


  //TIMER 2 FOR THE RGY
  TCCR2A &= ~(1 << WGM20);
  TCCR2A |= (1 << WGM21);
  TCCR2B &= ~(1 << WGM22); //set the timer to CTC mode
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);   //prescaler to 1024
  TCNT2 = 0; //initialise timer to 0
  OCR2A = 155; //10ms second delay (default)
  TIMSK2 |= (1 << OCIE2A); //enable CTC interrupt for OCR2A compare match*/


  sei();// enable all interrupts

  while (1)
  {
    if (buttonState == 1) //button has not been pressed
    {
      if (timer_2_state == 1) {
        PORTB |= 1 << PB3;    //red light on
        PORTB &= ~(1 << PB5); //yellow light off
        PORTB &= ~(1 << PB4); //green light off
      } else if (timer_2_state == 2) {
        PORTB &= ~(1 << PB3); //red light off
        PORTB &= ~(1 << PB5); //yellow light off
        PORTB |= 1 << PB4;    //green light on
      } else if (timer_2_state == 3) {
        PORTB &= ~(1 << PB3); //red light off
        PORTB |= 1 << PB5;    //yellow light on
        PORTB &= ~(1 << PB4); //green light off
      }
    } else {
      if (timer_2_state == 1) { //means red light was on when the button was pressed
        countCar++; //add car to vehicle breach

        //clear timers and blink counts
        TCNT0 = 0;
        count = 0;
        timer0count = 0;

        //blink light twice at 200 ms periods
        while (count < 2) {
          if (timer0count > 61) {
            PORTB |= 1 << PB0;    //white light on
          } else {
            PORTB &= ~ 1 << PB0;   //white light off
          }
        }
        PORTB &= ~ 1 << PB0;   //white light off

        //100% duty cycle if more than 100 cars
        if (countCar >= 100) {
          OCR1A = 15624;
          OCR1B = 0;
        } else {
          OCR1A = 15624.00 * countCar / 100.00;
          OCR1B = 15624.00 - OCR1A;//else set PWM duty cycle to be car count as a percentage
        }
      }
      buttonState = 1; //clear button state back to default (not pressed).
    }
  }
}
