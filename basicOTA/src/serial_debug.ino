void ESPserialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
// SERIAL INTERRUPT
void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    // inputString += inChar;
    if (inChar == '0')
    {
      Serial.printf("timeroff -> %d\n", reg_timeoff);
      Serial.printf("ttime -> %d\n", ttime);
    }
    else if (inChar == '1')
    {
      for (i = 0; i <= 40; i++)
        Serial.printf("M : %d : %d\n", i, EEPROM.read(i));
    }
    else if (inChar == '2')
    {
      Serial.printf("h : %d\n", h);
      Serial.printf("m : %d\n", m);
    }
    else if (inChar == 'l')
    {
      digitalWrite(ESPGPIO_D2, 1);
    }
    else if (inChar == 'd')
    {
      digitalWrite(ESPGPIO_D2, 0);
    }


    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }
}
