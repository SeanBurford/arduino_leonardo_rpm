static volatile unsigned int prev_icr;
static volatile unsigned int capture_val;
static volatile unsigned int curr_overflows;

static volatile unsigned int prev_intervals[4] = {0,0,0,0};
static volatile unsigned int prev_partial = 0;
static volatile unsigned int debounce_count = 0;

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

  unsigned int icr = ICR1;

  // If we've had > 0xFFFF timer ticks return a rate of zero.
  if (curr_overflows > 1 || (curr_overflows == 1 && icr >= prev_icr)) {
    capture_val = 65535;
    curr_overflows = 0;
    prev_partial = 0;
    prev_icr = icr;
    // Reset expected values.
    prev_intervals[0] = 0;
    prev_intervals[1] = 0;
    prev_intervals[2] = 0;
    prev_intervals[3] = 0;
    return;
  }
  curr_overflows = 0;

  unsigned int interval = icr - prev_icr;
  prev_icr = icr;

  // Debounce short intervals.
  if (interval < 50) {
    prev_partial += interval;
    return;
  }

  // Calculate a window over the previous four intervals.
  unsigned int amin = 0xffff;
  unsigned int amax = 0;
  for (int i = 0; i < 4; i++) {
    unsigned int p = prev_intervals[i];
    if (p == 0) {
      // Initial table population, return raw interval value.
      prev_intervals[i] = interval;
      capture_val = interval;
      prev_partial = 0;
      return;
    }
    if (p > amax) {
      amax = p;
    }
    if (p < amin) {
      amin = p;
    }
  }

  // Add some buffer around the window.
  amin = amin - (amin / 8);
  unsigned int aamax = amax / 8;
  if (amax < amax + aamax) {
    amax = amax + aamax;
  } else {
    amax = 0xffff;
  }

  // Check if the value fits into the expected window.
  if (debounce_count > 2) {
    // Let this interval through if we've debounced the last couple.
    // If we consistently debounce we might fool ourselves into believing
    // that the speed is half what it really is.
    debounce_count = 0;
  }
  if (((interval + prev_partial) >= interval) and // Integer wrap
      ((interval + prev_partial) >= prev_partial) and // Integer wrap
      ((interval + prev_partial) >= amin) and
      ((interval + prev_partial) <= amax)) {
    // Previous two capture values combined fit into the window, combine them.
    interval = interval + prev_partial;
    debounce_count = debounce_count + 1;
  }  else if ((interval < amin or interval > amax) and prev_partial == 0) {
    // Interval is outside of window, store it for next round.
    prev_partial = interval;
    return;
  } else {
    debounce_count = 0;
  }
  capture_val = interval;
  prev_partial = 0;

  // Update array for window calculation
  prev_intervals[0] = prev_intervals[1];
  prev_intervals[1] = prev_intervals[2];
  prev_intervals[2] = prev_intervals[3];
  prev_intervals[3] = capture_val;

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
