
unsigned int rpm_from_interval() {
  unsigned int prev_cap = prev_capture();
  if (prev_cap > 0xfff0) {
    return 0;
  }
  float rpm = (250000.0 / prev_capture()) * 60;
  return (unsigned int)rpm;
}

bool is_running() {
  return prev_capture() > 0;
}
