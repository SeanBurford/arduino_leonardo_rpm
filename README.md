## Arduino Leonardo RPM Measurement

### Summary

Measures the frequency of a 3.3v signal on pin 12.

Displays the result on an Adafruit i2c 16x2 LCD and also outputs it on the
USB/serial connection in a format suitable for the Arduino IDE plotter tool.

### Pins

* 12: (T1, input) signal being measured.
* 5v: (power out) LCD shield power.
* gnd: (power out) LCD shield ground.
* scl: (i2c) LCD shield clock.
* sda: (i2c) LCD shield data.

Optional:

* 4: (ICP1, input) capture trigger (12Hz).
* 5: (OC3A, output) capture trigger (12Hz).

### Theory

Timer/counter 1 is configured to count pulses on T1.

Timer/counter 3 is configured to generate a trigger at 12Hz.

The timer/counter 3 trigger can either be used in an interrupt handler to collect
counts from timer/counter 1 (the current configuration), or it can be used to
trigger input capture on timer/counter 1 for a more accurate capture.

In practice, collecting counts in an interrupt handler has been sufficient for
a solid count.

### LCD Shield

Manual and wiring diagram (page 22):

* https://cdn-learn.adafruit.com/downloads/pdf/rgb-lcd-shield.pdf

FAQ:

* https://learn.adafruit.com/rgb-lcd-shield/f-dot-a-q
