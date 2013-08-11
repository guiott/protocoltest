void Joystick(void)
{
Joy[JoyIndx] = (Joy[JoyIndx]  + analogRead(JoyIndx)) / 2; // average
  JoyIndx ++;
  if(JoyIndx >= 4)
  {
    JoyIndx = 0;
  }
  /* Switching in advance to the next port to measure, this allows the voltage
    to stabilize to the new value on the ADC Sample/Hold
  */
  Dummy = analogRead(JoyIndx);
}
