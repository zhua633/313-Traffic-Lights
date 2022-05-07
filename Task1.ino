#include <util/delay.h>

uint8_t timerstate;

ISR(TIMER1_COMPA_vect) {

  //using timer for the RGY blinks
  if (timerstate == 3) {
    timerstate = 1;
  }
  else {
    timerstate++;
  }
}



int main(void) {

  timerstate  = 1;

  DDRB |= (1 << DDB3); // set pin 11 of port B to output
  DDRB |= (1 << DDB2); // set pin 10 of port B to output
  DDRB |= (1 << DDB1); // set pin 9 of port B to output

  cli(); //disable all interrupts during configuration

  TCCR1A &= ~(1 << WGM11);
  TCCR1A &= ~(1 << WGM10);
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12); // Set the timer to CTC mode and
  //prescaler to 1024 -
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TCCR1B &= ~(1 << CS11);

  // see Datasheet Table 15-5 and 15-6
  TCNT1 = 0;   // Initialize timer to 0
  OCR1A = 15624;   // Set CTC compare value
  TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt for OCR1A compare match

  sei(); //enable all interrupts


  while (1) {

    switch (timerstate) {
      case 1:
        PORTB |= 1 << PB3;    //red light on
        PORTB &= ~(1 << PB2); //yellow light off
        PORTB &= ~(1 << PB1); //green light off
        break;

      case 2:
        PORTB &= ~(1 << PB3); //red light off
        PORTB &= ~(1 << PB2); //yellow light off
        PORTB |= 1 << PB1;    //green light on
        break;

      case 3:
        PORTB &= ~(1 << PB3); //red light off
        PORTB |= 1 << PB2;    //yellow light on
        PORTB &= ~(1 << PB1); //green light off
        break;

      default:
        break;
    }
  }
}
