## Arduino Leonardo RPM Measurement

### Summary

Measures the RPM of a 3.3v signal on pin 12 by measuring the time for a
revolution.

Displays the result on an Adafruit i2c 16x2 LCD and also outputs it on the
USB/serial connection in a format suitable for the Arduino IDE plotter tool.

### Pins

* 3.3v: (power out) Sensor power.
* 5v: (power out) LCD shield power.
* gnd: (power out) LCD shield and sensor ground.
* scl: (i2c) LCD shield clock.
* sda: (i2c) LCD shield data.

And one of:

* 4: (Arduino Leonardo ICP1, input) signal being measured.
* 8: (Arduino Uno ICP1, input) signal being measured.

### Theory

Timer/counter 1 is configured to run at 250kHz.

A rising edge on ICP1 (pin 4 on the Leonardo) causes a timer 1 capture.

The captured value multiplied by 1/250,000 to calulate revolutions per second,
which is multiplied by 60 to get RPM.

![Arduino serial plot showing RPM](../img/serial_plot_interval.png)
