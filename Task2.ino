#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t timerstate;
uint8_t timerAstate;
uint8_t buttonState;
uint8_t whiteBlink;
uint8_t state;
uint8_t redLight;
uint8_t configMode;
volatile uint16_t resultConversion;

ISR(TIMER1_COMPA_vect) {

  //using timer for the white blinks
  timerAstate = ~timerAstate;
  whiteBlink++;

  //using timer for the RGY blinks
  if (timerstate == 3) {
    timerstate = 1;
  }
  else {
    timerstate++;
  }
}


//external interrupt INT0(button pushed)
ISR(INT0_vect) {
  buttonState = ~buttonState;//toggle button state
}

ISR(ADC_vect) {
  resultConversion = ADC;
}

void setup() {
}

void loop() {
  DDRB |= (1 << DDB3); // set pin 11 of port B to output
  DDRB |= (1 << DDB2); // set pin 10 of port B to output
  DDRB |= (1 << DDB1); // set pin 9 of port B to output
  DDRB |= (1 << DDB0); // set pin 8 of port B to output
  DDRD &= ~(1 << DDD2); // set pin 2 of port D as input
  DDRC &= ~(1 << DDC0); //set pin A0 of port D to input
  buttonState = 1; //default setting
  timerAstate = 1;
  timerstate = 1;
  state = 1;
  redLight = 0;
  whiteBlink = 0;
  resultConversion = 0;

  cli();  //disable all interuppts during configuration

  EIMSK |= (1 << INT0); //enables one external interrupt INT0.
  EICRA |= (1 << ISC00);
  EICRA |= (1 << ISC01); //rising edge of INT0 generates an external interrupt request.
  ADMUX |= (1 << REFS0); //AVcc with external capacitor at AREF pin
  ADCSRA = 0x0;
  ADCSRA |= (1 << ADEN) | (1 << ADIE); //enables the ADC and the conversion complete interrupt
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //set division factor to 128

  //Serial.print("Line 51 \n");
  TCCR1A &= ~(1 << WGM11);
  TCCR1A &= ~(1 << WGM10);
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12); // Set the timer to CTC mode and
  //prescaler to 1024 -
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TCCR1B &= ~(1 << CS11);

  // see Datasheet Table 15-5 and 15-6
  TCNT1 = 0;   // Initialize timer to 0
  TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt for OCR1A compare match
  GTCCR = 0;

  sei();// enable all interrupts

  while (1)
  {
    if (buttonState == 1 || configMode == 0) //Button has not been pressed, we are in default mode
    {
      PORTB |= 1 << PB0; //white light on
      OCR1A = 15625 * state; // Set CTC compare value
      if (timerstate == 1) {
        redLight = 1;
        PORTB |= 1 << PB3;    //red light on
        PORTB &= ~(1 << PB2); //yellow light off
        PORTB &= ~(1 << PB1); //green light off
        if (buttonState != 1) {
          configMode = 1;
        }
      } else if (timerstate == 2) {
        redLight = 0;
        PORTB &= ~(1 << PB3); //red light off
        PORTB &= ~(1 << PB2); //yellow light off
        PORTB |= 1 << PB1;    //green light on
      } else if (timerstate == 3) {
        redLight = 0;
        PORTB &= ~(1 << PB3); //red light off
        PORTB |= 1 << PB2;    //yellow light on
        PORTB &= ~(1 << PB1); //green light off
      }
    } else {
      if (redLight == 1) {
        //The button has been pressed enters configuration mode
        PORTB |= 1 << PB3;    //red light on
        PORTB &= ~(1 << PB2); //yellow light off
        PORTB &= ~(1 << PB1); //green light off
        PORTB &= ~ (1 << PB0); //white light off

        ADCSRA |= (1 << ADSC); //starts ADC conversion
        state = ((resultConversion / 256) + 1);
        OCR1A = 15624 / (state * 2);

        whiteBlink = 0;
        timerAstate = 1;
        while (whiteBlink < (state * 2)) {
          {
            if (timerAstate == 1) {
              PORTB |= 1 << PB0; //white light on
            } else {
              PORTB &= ~ (1 << PB0); //white light off
            }
          }
        }
        OCR1A = 31249;

        if (timerAstate == 1) {
          while (timerAstate == 1) {
            PORTB &= ~ (1 << PB0); //white light off
          }
        } else {
          while (timerAstate != 1) {
            PORTB &= ~ (1 << PB0); //white light off
          }
        }
      } else {
        configMode = 0;
      }
    }
  }
}
