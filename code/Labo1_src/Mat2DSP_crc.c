//###########################################################################
//
// FILE:    crc.c
// TITLE:   Functions for CRC computation
// VERSION:	1.0		13.07.12	Y.Charrotton
// ASSUMPTIONS:
//   
// Description:
//
//###########################################################################

//
// copyright 2012 by
// Raoul Herzog
// Institute for Industrial Automation (iAi)
// HEIG-VD, University of Applied Sciences Western Switzerland
// CH-1401 YVERDON
// Switzerland
//
 
#include "Mat2DSP_crc.h"

//-----------------------------------------------------------------------------
// Re-Init the CRC Global value
Uint16 crc_init()
{
	return CRC16_INIT_VALUE;
}

//-----------------------------------------------------------------------------
// Add the effect of a byte to the CRC
Uint16 crc_update_byte(Uint16 crcVal, unsigned char x)
{
	Uint16 j;

	crcVal = crcVal ^ (x << 8);
	for (j = 0; j< 8 ;j++)    // Assuming 8 bits per byte
	{
		if (crcVal & 0x8000)     // if leftmost (most significant) bit is set
			crcVal = (crcVal << 1) ^ CRC16_POLY;
		else
			crcVal = crcVal << 1;
	}
	return crcVal;
}

//-----------------------------------------------------------------------------
// Add the effect of a word to the CRC
Uint16 crc_update_word(Uint16 crcVal, Uint16 x)
{
	crcVal = crc_update_byte(crcVal,(x>>8) & 0x00ff);
	crcVal = crc_update_byte(crcVal, x & 0x00ff);
	return crcVal;
}


//-----------------------------------------------------------------------------
// Compute the all CRC, based on a 16 bits words, separeted in two parts
Uint16 crc_complete(Uint16* mes, Uint16 size)
{
	Uint16 k = 0, crcVal;
	crcVal = crc_init();
	
	for(k = 0; k<size;k++)
		crcVal = crc_update_word(crcVal, mes[k]);
	
	return crcVal;	
}



