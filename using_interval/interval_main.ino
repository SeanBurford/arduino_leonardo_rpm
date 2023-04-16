// Input Capture Pin: Timer 1 Input Capture
#define ICAP_PIN 4

#define MOTOR_PIN 6

extern volatile unsigned int history[];

unsigned long update_serial_at = 0;

void setup() {
  pinMode(ICAP_PIN, INPUT);
  // Enable internal pullup (if there is no external pullup).
  //digitalWrite(ICAP_PIN, HIGH);

  pinMode(MOTOR_PIN, OUTPUT);

  // Reset timer prescalers
  GTCCR = bit(PSRSYNC);
  // Configure timer 3 to generate 10 interrupts per second.
  timer1_setup();

  ui_setup();

  update_serial_at = millis();
}

void loop() {
  char msg[250];
  unsigned long now = millis();
  if (update_serial_at > 0 && now >= update_serial_at) {
    unsigned int rpm = rpm_from_interval();
    sprintf(msg, "rpm:%u\n", rpm);
    /*
    sprintf(msg, "%5u %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u %5u\n",
            history[0], history[0] - history[1],
            history[1], history[1] - history[2],
            history[2], history[2] - history[3],
            history[3], history[3] - history[4],
            history[4], history[4] - history[5],
            history[5], history[5] - history[6],
            history[6]);
    /**/
    Serial.print(msg);
    update_serial_at = update_serial_at + 100;
  }
  motor_loop(MOTOR_PIN);
  ui_loop();
}
