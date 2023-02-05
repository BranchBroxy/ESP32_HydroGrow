
PinStruct set_pump_mode(int pump_mode) 
{
  PinStruct pump_map;

  if (pump_mode == 0)
  {
    pump_map = (PinStruct){0, 0, 0, 0};
  }
  else if (pump_mode == 1)
  {
    pump_map = (PinStruct){80, 80, 80, 90};
  }
  else if (pump_mode == 2)
  {
    pump_map = (PinStruct){100, 100, 80, 95};
  }
  else if (pump_mode == 3)
  {
    pump_map = (PinStruct){85, 90, 100, 100};
  }
  else {
    pump_map = (PinStruct){0, 0, 0, 0};
  }

  return pump_map;
}

void set_pumps(PinStruct pump_map, void (*func)(unsigned char, unsigned int), const int PWMChannel)
{
  func(PWMChannel, pump_map.bloom_pump * 10.24);
  WebSerial.println(pump_map.bloom_pump * 10.24);
}
