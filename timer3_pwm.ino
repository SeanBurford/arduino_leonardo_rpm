volatile unsigned long timer3_toggle_count = 0;
volatile unsigned int timer1_counts[10] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Triggered when TCNT3 = 0 if bit TOIE3 is set in TIMSK3.
//ISR (TIMER3_OVF_vect) {
//}

// Record timer 1 value into timer1_counts array.
// Triggered 10 times per second.
ISR (TIMER3_COMPA_vect) {
  unsigned int tcnt1 = TCNT1;
  timer1_counts[0] = timer1_counts[1];
  timer1_counts[1] = timer1_counts[2];
  timer1_counts[2] = timer1_counts[3];
  timer1_counts[3] = timer1_counts[4];
  timer1_counts[4] = timer1_counts[5];
  timer1_counts[5] = timer1_counts[6];
  timer1_counts[6] = timer1_counts[7];
  timer1_counts[7] = timer1_counts[8];
  timer1_counts[8] = timer1_counts[9];
  timer1_counts[9] = tcnt1;
  timer3_toggle_count++;
}

static void start_timer3() {
  cli();

  // Reset Timer config bits
  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3C = 0;

  // Set count to zero
  TCNT3 = 0;
  timer3_toggle_count = 0;
  
  // Clear timer interrupt bits so we don't fire immediately
  TIFR3 = bit (TOV3) | bit(OCF3A);

  // Enable interrupts on match (TCNT = OCR3A)
  TIMSK3 = bit(OCIE3A);

  // Configure counter to count to 6250 then go back to zero.
  // With a 16MHz clock / prescaler 256, that takes 1/10 seconds.
  // See Waveform mode 14 (pg. 133 of the ATMega32u4 datasheet).
  ICR3 = 6250;  // Top: Count up to 6250.
  OCR3A = 3125; // Enable OC3A output, 50% duty cycle.
  // Set OC3A on compare match, clear at top.
  TCCR3A = bit(COM3A1) | bit(COM3A0) | bit(WGM31);
  TCCR3B = bit(WGM33) | bit(WGM32) | bit(CS32);

  sei();
}

void timer3_setup() {
  start_timer3();
}

unsigned int read_tcnt3() {
  cli();
  unsigned int i = TCNT3;
  sei();
  return i;
}
