void recvMsg(uint8_t *data, size_t len) {
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  WebSerial.println(d);
  if (d == "reset") {
    reset_msg_rec = true;
  }
    if (d == "wurzel mode") {
    sliderValue = 1;
  }
    if (d == "blatt mode") {
    sliderValue = 2;
  }
  if (d == "growing mode") {
    sliderValue = 3;
  }
  
}
