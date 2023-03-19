// Interval Pin: Timer 3 Output OC3A
#define INTERVAL_PIN 5

// Count Pin: Timer 1 Count Increment
#define COUNT_PIN 12
// Input Capture Pin: Timer 1 Input Capture
#define ICAP_PIN 4

// The most recent 10 timer 1 counts from when timer 3 fired.
extern volatile unsigned int timer1_counts[12];

unsigned long update_serial_at = 0;

void setup() {
  pinMode(COUNT_PIN, INPUT);
  // Enable internal pullup (if there is no external pullup).
  digitalWrite(COUNT_PIN, HIGH);

  pinMode(ICAP_PIN, INPUT);

  pinMode(INTERVAL_PIN, OUTPUT);

  // Reset timer prescalers
  GTCCR = bit(PSRSYNC);
  // Configure timer 3 to generate 10 interrupts per second.
  timer3_setup();
  // Configure timer 1 to count pulses on COUNT_PIN.
  timer1_setup();

  //if (timer1_clocksource_is_external() == true) {
  //  timer1_toggle_clocksource();
  //}

  ui_setup();

  update_serial_at = millis();
}

void loop() {
  char msg[100];
  unsigned long now = millis();
  if (update_serial_at > 0 && now >= update_serial_at) {
    unsigned int rpm = 2 * (timer1_counts[9] - timer1_counts[4]);
    sprintf(msg, "rpm:%u\n", rpm);
    /*
    sprintf(msg, "%3ld.%ld %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u | %5u %5u %5u %5u %5u %5u %5u %5u %5u\n",
                  timer3_toggle_count/10, timer3_toggle_count % 10,
                  timer1_counts[0], timer1_counts[1],
                  timer1_counts[2], timer1_counts[3],
                  timer1_counts[4], timer1_counts[5],
                  timer1_counts[6], timer1_counts[7],
                  timer1_counts[8], timer1_counts[9],
                  timer1_counts[1] - timer1_counts[0],
                  timer1_counts[2] - timer1_counts[1], timer1_counts[3] - timer1_counts[2],
                  timer1_counts[4] - timer1_counts[3], timer1_counts[5] - timer1_counts[4],
                  timer1_counts[6] - timer1_counts[5], timer1_counts[7] - timer1_counts[6],
                  timer1_counts[8] - timer1_counts[7], timer1_counts[9] - timer1_counts[8]
                  );
    //unsigned int i = 5 + (6 * (timer3_toggle_count % 10));
    //msg[i] = '<';
    */
    Serial.print(msg);
    update_serial_at = update_serial_at + 100;
  }

  ui_loop();
}
