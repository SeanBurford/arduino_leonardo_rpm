static volatile unsigned int prev_icr;
static volatile unsigned int capture_val;
static volatile unsigned int curr_overflows;

volatile unsigned int history[16];

// Triggered when TCNT1 = 0 if bit TOIE3 is set in TIMSK3.
ISR (TIMER1_OVF_vect) {
  if (curr_overflows == 0) {
    curr_overflows = 1;
  } else {
    curr_overflows = 2;
    capture_val = 0;
  }
}

// Triggered on capture events if bit TOIE3 is set in TIMSK3.
ISR (TIMER1_CAPT_vect) {
  // Check if an overflow event is also pending.
  if (TIFR1 & TOV1 > 0) {
     curr_overflows += 1;
  }

  // Calculate how long it has been since the previous capture.
  unsigned int icr = ICR1;
  if (curr_overflows == 0 || (curr_overflows == 1 && icr < prev_icr)) {
    capture_val = icr - prev_icr;
    if (capture_val < 32) {
      // We must have missed an overflow
      capture_val = 65535;
    }
  } else {
    capture_val = 0;
  }
  prev_icr = icr;
  curr_overflows = 0;
  /*
  history[15] = history[14];
  history[14] = history[13];
  history[13] = history[12];
  history[12] = history[11];
  history[11] = history[10];
  history[10] = history[9];
  history[9] = history[8];
  history[8] = history[7];
  history[7] = history[6];
  history[6] = history[5];
  history[5] = history[4];
  history[4] = history[3];
  history[3] = history[2];
  history[2] = history[1];
  history[1] = history[0];
  history[0] = prev_icr;
  /**/
}

static void start_timer1() {
  cli();

  // Reset Timer config bits
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;

  // Set count to zero
  TCNT1 = 0;
  
  // Clear timer interrupt bits so we don't fire immediately
  TIFR1 = bit (TOV1) | bit(ICF1);

  // Enable interrupts on overflow and on capture.
  TIMSK1 = bit(TOIE1) | bit(ICIE1);

  // Configure counter for mode 0: count from 0 to 0xFFFF.
  // Prescaler is 64, giving a good range of possible RPM values.
  // ICNC1 enables the capture noise canceller (4 samples).
  // ICES1 sets the capture trigger to the rising edge.
  ICR1 = 0;
  TCCR1A = 0;
  TCCR1B = bit(ICNC1) | bit(CS11) | bit(CS10);

  sei();
}

void timer1_setup() {
  prev_icr = 0;
  capture_val = 0;
  curr_overflows = 0;
  
  start_timer1();
}

unsigned int read_tcnt1() {
  cli();
  unsigned int i = TCNT1;
  sei();
  return i;
}

unsigned int prev_capture() {
  cli();
  unsigned int i = capture_val;
  sei();
  return i;
}
