/* myMCP.h
Basic definitions for MCP digipots - HAL abstraction of SPI and I2C versions

************** NOT IN USE **************
********* use myMCPXX instead **********
*/
#ifndef MYMCP_H
#define MYMCP_H

// command structure is common across MCP41 & MCP45 variants

// TCON register masks
#define TCONB          (0x01) // Terminal B Connection	
#define TCONW          (0x02) // Wiper Connection
#define TCONA          (0x04) // Terminal A Connection 
#define TCONHW         (0x08) // Shutdown 

#define MCPX31         (0x7F)      // 7 bit pots
#define MCPX51         (0xFF)	   // 8 bit pots
#define MCP_MASK       MCPX51      // change for X51 / X31 variants
#define TCON_MASK 	   (0x0F)
#define MCP45X          1 
#define MCP41X          2


struct digiPot {
  float minVal;     // value in real-world units at count = 0
  float maxVal;     // value in real-world units at max count 
  uint8_t maxCount; // same as the bit mask for MCP digipots
  uint8_t type;     // part type to allow different drivers 
  uint8_t channel;  // CSPIn for SPI, I2C chip address 
  int8_t polarity;   //  -1: increasing setting reduces voltage
} ;
/////////////////// MAY not use these //////////////////////////
// map DigiPot control onto SPI or I2C functions: e.g. MCP41begin() or MCP45begin()
// channel = CSPIN or I2C address
// type = MCP45 or MCP41
void MCP41begin(uint8_t setting, uint8_t ss);
void MCP41writeW(uint8_t val, uint8_t ss);
uint8_t MCP41readW(uint8_t ss);
void MCP41writeT(uint8_t val, uint8_t ss);
uint8_t MCP41readT(uint8_t ss);

void MCP45begin(uint8_t setting, uint8_t addr);
void MCP45writeW(uint8_t val, uint8_t addr);
uint8_t MCP45readW(uint8_t addr);
void MCP45writeT(uint8_t val, uint8_t addr);
uint8_t MCP45readT(uint8_t addr);

void MCPbegin(uint8_t set, uint8_t channel, uint8_t type){
#ifdef MYMCP45_H
  if (type == MCP45X)
    MCP45begin(set, channel);
#endif
#ifdef MYMCP41_H
  if (type == MCP41X)
     MCP41begin(set, channel);
#endif  
}
void MCPwriteW(uint8_t set, uint8_t channel, uint8_t type){
#ifdef MYMCP45_H
  if (type == MCP45X)
    MCP45writeW(set, channel);
#endif
#ifdef MYMCP41_H
  if (type == MCP41X)
     MCP41writeW(set, channel);
#endif  
}
uint8_t MCPreadW(uint8_t channel, uint8_t type){
#ifdef MYMCP45_H
  if (type == MCP45X)
    return MCP41readW(channel);
#endif
#ifdef MYMCP41_H
  if (type == MCP41X)
     return MCP41readW(channel);
#endif  
return 0;
}
void MCPwriteT(uint8_t set, uint8_t channel, uint8_t type){
#ifdef MYMCP45_H
  if (type == MCP45X)
    MCP45writeT(set, channel);
#endif
#ifdef MYMCP41_H
  if (type == MCP41X)
     MCP41writeT(set, channel);
#endif  
}

uint8_t MCPreadT(uint8_t channel, uint8_t type){
#ifdef MYMCP45_H
  if (type == MCP45X)
    return MCP41readT(channel);
#endif
#ifdef MYMCP41_H
  if (type == MCP41X)
     return MCP41readT(channel);
#endif  
return 0;
}

#endif /* MYMCP_H */