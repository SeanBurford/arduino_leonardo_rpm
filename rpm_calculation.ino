// The most recent 10 timer 1 counts from when timer 3 fired.
extern volatile unsigned int timer1_counts[12];

// RPM is the most recent reading - 500ms prior, times 2.
// Different gaps allow for different measurement periods:
//   1 = 1/12 of a second.
//   2 = 1/6 of a second.
//   3 = 1/4 of a second.
//   4 = 1/3 of a second.
//   6 = 1/2 of a second.
//   8 = 2/3 of a second.
//   9 = 3/4 of a second.
const int gap = 6;
// With a gap of 6, we need two measurement periods to make 1 second.
const int periods = 2;

unsigned int rpm_from_counts() {
  unsigned int rps = periods * (timer1_counts[11] - timer1_counts[11 - gap]);
  return rps * 60;
}
