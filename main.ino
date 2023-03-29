// Interval Pin: Timer 3 Output OC3A
#define INTERVAL_PIN 5

// Count Pin: Timer 1 Count Increment
#define COUNT_PIN 12
// Input Capture Pin: Timer 1 Input Capture
#define ICAP_PIN 4

unsigned long update_serial_at = 0;

void setup() {
  pinMode(COUNT_PIN, INPUT);
  // Enable internal pullup (if there is no external pullup).
  //digitalWrite(COUNT_PIN, HIGH);

  pinMode(ICAP_PIN, INPUT);

  pinMode(INTERVAL_PIN, OUTPUT);

  // Reset timer prescalers
  GTCCR = bit(PSRSYNC);
  // Configure timer 3 to generate 10 interrupts per second.
  timer3_setup();
  // Configure timer 1 to count pulses on COUNT_PIN.
  timer1_setup();

  ui_setup();

  update_serial_at = millis();
}

void loop() {
  char msg[100];
  unsigned long now = millis();
  if (update_serial_at > 0 && now >= update_serial_at) {
    unsigned int rpm = rpm_from_counts();
    sprintf(msg, "rpm:%u\n", rpm);
    Serial.print(msg);
    update_serial_at = update_serial_at + 100;
  }

  ui_loop();
}
