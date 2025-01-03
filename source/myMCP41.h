/* myMCP41.h
Basic definitions for MCP41X SPI digipots 
*/
#ifndef MYMCP41_H
#define MYMCP41_H
#include <SPI.h>
#include "myMCP.h"

#define MCP41_WRITE_W   (0x00)
#define MCP41_READ_W    (0x0C)    // (0x03 << 2)
#define MCP41_WRITE_T   (0x40)
#define MCP41_READ_T    (0x4C)
#define MCP41_INC       (0x04)
#define MCP41_DEC       (0x08)

SPISettings MCP41(4000000,MSBFIRST,SPI_MODE0);

#define SS_DEFAULT 10

void MCP41begin(uint8_t setting, uint8_t ss = SS_DEFAULT){
  SPI.begin();
  pinMode(ss, OUTPUT);
  digitalWrite(ss, HIGH);
  MCP41writeT(TCONA | TCONB | TCONW | TCONHW, ss); // ensure all the pot pins are connected and not in shutdown mode
  MCP41writeW(setting,ss);
}
void MCP41writeW(uint8_t val, uint8_t ss = SS_DEFAULT){
  SPI.beginTransaction(MCP41);
    digitalWrite(ss, LOW);
    SPI.transfer16((MCP41_WRITE_W << 8) | val);
    digitalWrite(ss, HIGH);
  SPI.endTransaction();
}
uint8_t MCP41readW(uint8_t ss = SS_DEFAULT){
  uint16_t temp1;
  SPI.beginTransaction(MCP41);
    digitalWrite(ss, LOW);
    temp1 = SPI.transfer16(MCP41_READ_W << 8 ); 
    digitalWrite(ss, HIGH);
  SPI.endTransaction();
  return temp1 & MCP_MASK; // only bottom  7 or 8 bits are significant
}
void MCP41writeT(uint8_t val, uint8_t ss = SS_DEFAULT){
  SPI.beginTransaction(MCP41);
    digitalWrite(ss, LOW);
    SPI.transfer16((MCP41_WRITE_T << 8) | val);
    digitalWrite(ss, HIGH);
  SPI.endTransaction();
}
uint8_t MCP41readT(uint8_t ss = SS_DEFAULT){
  uint16_t temp1;
  SPI.beginTransaction(MCP41);
    digitalWrite(ss, LOW);
    temp1 = SPI.transfer16(MCP41_READ_T << 8);
    digitalWrite(ss, HIGH);
  SPI.endTransaction();
  return temp1 & 0x0F; // only bottom 4 bits are significant
}
#endif /* MYMCP41_H */