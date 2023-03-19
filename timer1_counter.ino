volatile unsigned long timer1_overflow_count = 0;

ISR (TIMER1_OVF_vect) {
  ++timer1_overflow_count;
}

volatile unsigned int timer1_capture = 0;
volatile unsigned long timer1_cap_overflow = 0;

ISR (TIMER1_CAPT_vect) {
  timer1_capture = ICR1;
  timer1_cap_overflow = timer1_overflow_count;
  if (TIFR1 & bit (TOV1)) {
    // The overflow interrupt is due to fire (after this one).
    timer1_cap_overflow += 1;
  }
}

static void start_timer1() {
  cli();

  // Reset Timer 1 config bits
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  
  // Clear timer 1 interrupt bits so we don't fire immediately
  TIFR1 = bit (ICF1) | bit (TOV1);

  // TOIE1 = Overflow interrupt enable
  // ICIE1 = Input capture interrupt enable
  TIMSK1 = bit(TOIE1) | bit(ICIE1);

  // Set count to zero
  TCNT1 = 0;
  timer1_overflow_count = 0;

  // Clock timer 1 with from ext clock source T1 rising edge.
  // ICNC enables noise cancellation (4 measurement delay).
  // On the Leonardo, T1 is pin 12
  TCCR1B =  bit(ICNC1) | bit(CS12) | bit(CS11) | bit(CS10);  

  sei();
}

inline bool timer1_clocksource_is_external() {
  return ((TCCR1B & 0x07) == 0x07);
}

void timer1_toggle_clocksource() {
  uint8_t tccr = TCCR1B;
  if (timer1_clocksource_is_external() == true) {
    //Serial.print("switch to clk/1024\n");
    tccr = bit(ICNC1) | bit(CS12) | bit(CS10);
  } else {
    //Serial.print("switch to external\n");
    tccr = bit(ICNC1) | bit(CS12) | bit(CS11) | bit(CS10);
  }
  TCCR1B = tccr; 
}

void timer1_setup() {
  start_timer1();
}

unsigned int read_tcnt1() {
  cli();
  unsigned int i = TCNT1;
  sei();
  return i;
}

unsigned long timer1_long() {
  cli();
  unsigned long r = ((unsigned long)timer1_overflow_count << 16) | TCNT1;
  sei();
  return r;
}

unsigned long timer1_prev_cap() {
  cli();
  unsigned long r = ((unsigned long)timer1_cap_overflow << 16) | timer1_capture;
  sei();
  return r;
}
