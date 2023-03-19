#include <Adafruit_RGBLCDShield.h>

extern volatile unsigned long timer1_overflow_count;
extern volatile unsigned int timer1_capture;
extern volatile unsigned long timer1_cap_overflow;

#define LCD_RED 0x1
#define LCD_YELLOW 0x3
#define LCD_GREEN 0x2
#define LCD_TEAL 0x6
#define LCD_BLUE 0x4
#define LCD_VIOLET 0x5
#define LCD_WHITE 0x7

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// display_state controls which screen is shown.
static int8_t display_state = 0;

// update_count counts screen refreshes.
static uint8_t update_count = 0;

// buttons_pressed is true if any buttons are currently pressed.
static bool buttons_pressed = false;

// next_update is the millis() when a screen refresh is due.
static long next_update = 0;

void ui_setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(LCD_RED);
}

static unsigned long show_rpm(unsigned long now) {
  char msg[17];

  // Most recent reading - 500ms prior, times 2.
  unsigned int rpm = 2 * (timer1_counts[9] - timer1_counts[4]);

  // Find the highest RPM across five readings from the previous second.
  unsigned int maxrpm = 0;
  for (int i = 0; i < 5; i++) {
    unsigned int j = 2 * (timer1_counts[i + 5] - timer1_counts[i]);
    if (j > maxrpm) {
      maxrpm = j;
    }
  }

  sprintf(msg, "RPM %5u", rpm);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  sprintf(msg, "Max %5u", maxrpm);
  lcd.setCursor(0, 1);
  lcd.print(msg);

  return now + 200;
}


static unsigned long show_timer1(unsigned long now) {
  unsigned int t1_count = read_tcnt1();
    
  char msg[17];
  sprintf(msg, "TCNT1 %5u %3u", t1_count, timer1_overflow_count);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  if (timer1_clocksource_is_external() == true) {
    sprintf(msg, "TCCR1B %02X (ext)", TCCR1B);
  } else {
    sprintf(msg, "TCCR1B %02X (int)", TCCR1B);
  }
  lcd.setCursor(0, 1);
  lcd.print(msg);

  return now + 250;
}

static unsigned long show_timer3(unsigned long now) {
  unsigned int t3_count = read_tcnt3();
    
  char msg[17];
  sprintf(msg, "TCNT3 %5u", t3_count);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  return now + 250;
}

static unsigned long show_timer1_capture(unsigned long now) {
  char msg[17];

  sprintf(msg, "ICR1 %5u %3u", timer1_capture, timer1_cap_overflow);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  return now + 250;
}

static unsigned long show_diffs(unsigned long now) {
  char msg[17];
  
  sprintf(msg, "%5u %5u", 
          timer1_counts[9] - timer1_counts[8],
          timer1_counts[8] - timer1_counts[7]);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  sprintf(msg, "%5u %5u", 
          timer1_counts[7] - timer1_counts[6],
          timer1_counts[6] - timer1_counts[5]);
  lcd.setCursor(0, 1);
  lcd.print(msg);

  return now + 100;
}

static void tick_tock(unsigned int update_count) {
  char msg[17];

  if (update_count & 1 == 1) {
    msg[0] = 0xDD;
  } else {
    msg[0] = 0xC2;
  }
  msg[1] = 0x00;
  lcd.setCursor(15, 1);
  lcd.print(msg);
}

static unsigned long show_error(unsigned long now, unsigned int display_state) {
  char msg[17];

  sprintf(msg, "display state %d", display_state);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  return now + 1000;
}

void ui_loop() {
  long now = millis();
  if (now > next_update) {
    if (display_state < 0) {
      display_state = 4;
    }
    if (display_state > 4) {
      display_state = 0;
    }
    switch (display_state) {
      case 0: next_update = show_rpm(now);
              break;
      case 1: next_update = show_timer1(now);
              break;
      case 2: next_update = show_timer3(now);
              break;
      case 3: next_update = show_timer1_capture(now);
              break;
      case 4: next_update = show_diffs(now);
              break;
      default: next_update = show_error(now, display_state);
               break;
    }
    tick_tock(update_count++);
  }

  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    if (buttons_pressed == false) {
      buttons_pressed = true;
      if (buttons & BUTTON_LEFT) {
        display_state = display_state - 1;
      }
      if (buttons & BUTTON_RIGHT) {
        display_state = display_state + 1;
      }
      if (buttons & BUTTON_SELECT) {
        timer1_toggle_clocksource();
      }
      //if (buttons & BUTTON_UP) {
      //  digitalWrite(INTERVAL_PIN, HIGH);
      //}
      //if (buttons & BUTTON_DOWN) {
      //  digitalWrite(INTERVAL_PIN, LOW);
      //}
      lcd.clear();
      next_update = now;
    }
  } else {
    buttons_pressed = false;
  } 
}
