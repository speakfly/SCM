#ifndef _EEPROM_H
#define _EEPROM_H

#include <regx52.h>
// #include "EEPROM.c"

unsigned char read_eeprom(unsigned char add);
void write_eeprom(unsigned char add,unsigned char d);

#endif