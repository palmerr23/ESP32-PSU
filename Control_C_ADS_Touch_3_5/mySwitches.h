/* Encoder and panel switches

;
*/
#ifndef MYSWITCHES_H
#define MYSWITCHES_H

// *********** Tac switches and encoder ************//
uint16_t oldHigh = -1;
short power = 0;
short currentDigit = 4;

long newPosition = 0, oldPosition = 0;    // no action first time around
 
void rotate(ESPRotary& enc)
{
  newPosition = enc.getPosition();
  valChanged(VAL_CHGD) ;
  //Serial.print("E ");
}

void clickedR(Button2& but) { 
  // 5.2 digits for now
  if (butDigit < 2)
     butDigit++;
  //Serial.print("ButR = ");
 // Serial.println(butDigit);
  swPressed = true;
  butDir = 1;
  valChanged(VAL_CHGD) ;
}

void clickedL(Button2& but) {  
  // 5.2 digits for now
  if (butDigit > -2)
    butDigit--;
  //Serial.print("ButL = ");
 // Serial.println(butDigit);
   swPressed = true;
   butDir = -1;
   valChanged(VAL_CHGD);
}
#endif