#include <Adafruit_RGBLCDShield.h>

#define LCD_RED 0x1
#define LCD_YELLOW 0x3
#define LCD_GREEN 0x2
#define LCD_TEAL 0x6
#define LCD_BLUE 0x4
#define LCD_VIOLET 0x5
#define LCD_WHITE 0x7

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// next_update is the millis() when a screen refresh is due.
static long next_update = 0;

// update_count counts screen refreshes.
static uint8_t update_count = 0;

// display_state controls which screen is shown.
static int8_t display_state = 0;

// was_running is true when the axle is spinning.
static bool was_running = false;

static unsigned int max_rpm = 0;

static bool buttons_pressed = false;

static unsigned long start_time = 0;

void ui_setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(LCD_RED);
}

static unsigned long show_rpm(unsigned long now) {
  char msg[17];

  unsigned int rpm = rpm_from_interval();
  if (rpm > max_rpm) {
    max_rpm = rpm;
  }

  // Calculate runtime
  unsigned long runtime = start_time;
  if (was_running == true) {
    runtime = (millis() - start_time);
  }

  sprintf(msg, "Max %5u %4d.%d", max_rpm, (unsigned int)(runtime / 1000), (int)(runtime % 10));
  lcd.setCursor(0, 0);
  lcd.print(msg);

  sprintf(msg, "RPM %5u %5u", rpm, curr_overflows);
  lcd.setCursor(0, 1);
  lcd.print(msg);

  return now + 200;
}

static unsigned long show_error(unsigned long now, unsigned int display_state) {
  char msg[17];

  sprintf(msg, "display state %d", display_state);
  lcd.setCursor(0, 0);
  lcd.print(msg);

  return now + 1000;
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

void ui_loop() {
  long now = millis();

  if (now > next_update) {
    if (is_running()) {
      if (!was_running) {
        max_rpm = 0;
        was_running = true;
        start_time = millis();
      }
    } else if (was_running) {
      was_running = false;
      start_time = (millis() - start_time);
    }

    if (display_state < 0) {
      display_state = 0;
    }
    if (display_state > 0) {
      display_state = 0;
    }
    switch (display_state) {
      case 0: next_update = show_rpm(now);
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
      lcd.clear();
      next_update = now;
    }
  } else {
    buttons_pressed = false;
  } 
}
