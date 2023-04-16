
long last_stop = 0;

void motor_loop(int pin) {  
  long elapsed = millis() - last_stop;

  if (is_running() == false and elapsed > 500) {
    last_stop = millis();
    analogWrite(pin, 0);
  } else {
    byte val = (127 - ((elapsed / 500) % 128));
    analogWrite(pin, val);
  }
  
  //byte val = 0;
  //switch (now / 3000) % 5) {
  //  case 0: val = 0;
  //          break;
  //  case 1: val = 255;
  //          break;
  //  case 2: val = 128;
  //          break;
  //  case 3: val = 96;
  //          break;
  //  case 4: val = 64;
  //          break;
  //}
}
  
