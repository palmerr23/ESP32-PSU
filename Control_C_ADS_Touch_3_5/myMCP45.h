/* myMCP45.h
Basic definitions for MCP45X I2C digipots 
*/
#ifndef MYMCP45_H
#define MYMCP45_H
#include <Wire.h>
#include "myMCP.h"

#define MCP45_WRITE_W   (0x00)
#define MCP45_READ_W    (0x0C)    // (0x03 << 2)
#define MCP45_WRITE_T   (0x40)
#define MCP45_READ_T    (0x4C)
#define MCP45_INC       (0x04)
#define MCP45_DEC       (0x08)

#define MEM_WIPER       (0x00)
#define MEM_TCON        (0x40)

#define COM_WRITE           (0x00)
#define COM_READ            (0x0C)
#define COM_WIPERINC        (0x04)
#define COM_WIPERDEC        (0x08)

#define MCP_I2C_BASE   (0x3C)
#define MCPCSPEED 		400000  

void MCP45begin(uint8_t setting, uint8_t addr = MCP_I2C_BASE)
{
  Wire.begin();
  Wire.setClock(MCPCSPEED);
  MCP45writeT(TCONA | TCONB | TCONW | TCONHW, addr); // ensure all the pot pins are connected and not in shutdown mode
  MCP45writeW(setting, addr);
}
void MCP45writeW(uint8_t val, uint8_t addr = MCP_I2C_BASE)
{
  Wire.beginTransmission(addr);
    Wire.write(MEM_WIPER | COM_WRITE);
	Wire.write(val);
  Wire.endTransmission();
}
uint8_t MCP45readW(uint8_t addr = MCP_I2C_BASE)
{
  uint8_t val;
  Wire.beginTransmission(addr);
    Wire.write(MEM_WIPER | COM_READ);
  Wire.endTransmission(false);
  
  Wire.requestFrom(addr, (uint8_t)2);
  if(Wire.available())
  {
	  val = Wire.read(); // discard first byte
	  val = Wire.read();
  }

  return val & MCP_MASK; // only bottom  7 or 8 bits are significant
}
void MCP45writeT(uint8_t val, uint8_t addr = MCP_I2C_BASE)
{	
  Wire.beginTransmission(addr);      
    Wire.write(MCP45_WRITE_T);
    Wire.write(val);     
   Wire.endTransmission();
}
uint8_t MCP45readT(uint8_t addr = MCP_I2C_BASE)
{
  uint16_t val;
  Wire.beginTransmission(addr);
    Wire.write(MEM_TCON | COM_READ);
  Wire.endTransmission(false);    
 
  Wire.requestFrom(addr, (uint8_t)2);
  if(Wire.available())
  {	  
	  val = Wire.read(); // discard first byte
	  val = Wire.read();
  }
  return val & TCON_MASK; // only bottom 4 bits are significant
}
#endif /* MYMCP45_H */