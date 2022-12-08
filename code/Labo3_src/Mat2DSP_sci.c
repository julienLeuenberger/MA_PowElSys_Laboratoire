//###########################################################################
//
// FILE:    sci.c
// TITLE:   Function for SCI (UART) low level communication
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


#include "Mat2DSP_sci.h"
#include "Mat2DSP_config.h"
#include "Mat2DSP_crc.h"
#include "string.h"


struct SciTx_s SciTx;
struct SciRx_s SciRx;


#define CONCAT(a,b)         ((a<<8)+b)
#define LOWBYTE(a)			(a & 0x00FF)
#define HIGHBYTE(a)         ((a & 0xFF00)>>8)

inline void CmdTreatment();
void SciCtsTo(Uint16 val);
int  SciTxCtsRead();

//-----------------------------------------------------------------------------
// Initalize the SCI-B GPIO
void M2D_SciInitGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)19
//	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up for GPIO7  (SCIRXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;	   // Enable pull-up for GPIO29 (SCITXDB)18
//	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;	   // Enable pull-up for GPIO12 (SCITXDB)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3;  // Asynch input GPIO28 (SCIRXDA)
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SCIRXDA)

/* Configure SCI-B pins using GPIO regs*/
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO28 for SCIRXDA operation
//	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;    // Configure GPIO7  for SCIRXDA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO29 for SCITXDA operation
//	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;   // Configure GPIO12 for SCITXDA operation

    EDIS;
}

//-----------------------------------------------------------------------------
// Initalize the SCI-B for a specified protocol, including :
// 8-bit word, baud rate, 1 STOP bit, no parity
void M2D_SciInit()
{
	SciaRegs.SCICCR.all  = 0x0007;   			// 1 stop bit,  No loopback
			                               		// No parity,8 char bits,
			                               		// async mode, idle-line protocol
	SciaRegs.SCICTL1.all = 0x0003;  			// enable TX, RX, internal SCICLK,
	                               				// Disable RX ERR, SLEEP, TXWAKE
	SciaRegs.SCIHBAUD 	 = SCI_PRD_HI;
	SciaRegs.SCILBAUD 	 = SCI_PRD_LO;
	SciaRegs.SCIFFTX.all = 0xE020;				// Fifo enable, Interrupt when TX FIFO has 0 byte
	SciaRegs.SCIFFRX.all = 0x2021;				// Fifo enable, Interrupt when RX FIFO has 1 byte
	SciaRegs.SCIFFCT.all = 0x00;

	SciaRegs.SCICTL1.all = 0x0023;     			// Relinquish SCI from Reset

	SciTx.State 		 = START_PROT_SEND;		// Initialize de Tx struct
	SciTx.Adr 			 = 0;
	SciTx.Length		 = 0;
	SciTx.NByteSent	 = 0;
	SciTx.IsSending	 = 0;

	SciRx.State		 = STX_WAIT;			// Initialize de Rx struct
	SciRx.Length		 = 0x0000;
	SciRx.Crc			 = 0x0000;
	SciRx.NByteReceived = 0x0000;
}

//-----------------------------------------------------------------------------
// SCI-B Tx ISR
interrupt void SCI_TX_ISR(void)
{
    Uint16 remaind;
    static Uint16 crcVerif;
    volatile Uint16 TxWord;

    #if ACTIVATE_RXTX_LED == 1
    	extern Uint16 RxTxLEDCtr;
    #endif
     	
    PieCtrlRegs.PIEIER9.bit.INTx1 = 0;     // No Rx interrupts during Tx. SCIARX, PIE Group 9, INT1
	EINT;
	
    #if ACTIVATE_RXTX_LED == 1
		RxTxLEDCtr  = RXTX_LED_PRD;
		GPIO_RX_LED = 0;  // LED On
    #endif
 		
	// Handshaking management
  	if(SciTxCtsRead())					// Data sended only if master is ready
	{
		// Protocole Creation State Machine
		switch(SciTx.State)
		{
			case START_PROT_SEND :
				SciaRegs.SCITXBUF = STX;
				crcVerif		  = 0x800F;					// Always the same value at this point
				SciaRegs.SCITXBUF = SID;
				crcVerif 		  = crc_update_byte(crcVerif, SID);
				//SciaRegs.SCITXBUF=HIGHBYTE((SciTx.Length<<1)+7);					// Length not used anymore
				//crcVerif = crc_update_byte(crcVerif, HIGHBYTE((SciTx.Length<<1)+7));
				//SciaRegs.SCITXBUF=LOWBYTE((SciTx.Length<<1)+7);
				//crcVerif = crc_update_byte(crcVerif, LOWBYTE((SciTx.Length<<1)+7));
				SciTx.State = DAT_SEND;
				break;
				
			case DAT_SEND :
		  		remaind = ((SciTx.Length<<1) - SciTx.NByteSent);
		  			
		  		if (remaind == 0)
					SciTx.State = END_PROT_SEND;
				else
				{
		  			if(remaind > TX_FIFO_SIZE)
						remaind = TX_FIFO_SIZE;
	
				  	while(remaind!=0)
				  	{
				  		TxWord = SciTx.Adr[SciTx.NByteSent>>1];
				  		if((SciTx.NByteSent % 2) == 0)
				  		{
							SciaRegs.SCITXBUF=HIGHBYTE(TxWord);
							
							crcVerif = crc_update_byte(crcVerif, HIGHBYTE(TxWord));
				  		}
						else
						{
							SciaRegs.SCITXBUF=LOWBYTE(TxWord);
							crcVerif = crc_update_byte(crcVerif, LOWBYTE(TxWord));
						}
			   			
			   			SciTx.NByteSent++;
			   			remaind--;
			   			// We have to wait until buffer is ready to receive an other byte (not used... for the moment !?)
			   			//while(SciaRegs.SCICTL2.bit.TXRDY != 1);
				  	}
				}
				break;
				
			case END_PROT_SEND :
				SciaRegs.SCITXBUF = HIGHBYTE(crcVerif);
				SciaRegs.SCITXBUF = LOWBYTE(crcVerif);
				SciaRegs.SCITXBUF = ETX;
				SciTx.State	  = END_COM;
				break;
				
			case END_COM :
				SciTx.IsSending = 0;
				SciTx.State     = START_PROT_SEND;
				PieCtrlRegs.PIEIER9.bit.INTx2 = 0;	// Disable Tx interrupt because everything is sent
                PieCtrlRegs.PIEIER9.bit.INTx1 = 1;  // Enable again Rx interrupts. SCIARX, PIE Group 9, INT1
				break;
				
			default :
				SciTx.State = START_PROT_SEND;
		}
	}
	 	
    SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;  // Clear SCI Interrupt flag
   	PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK
   	
   	//GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
}

//-----------------------------------------------------------------------------
// SCI-B Rx ISR
interrupt void SCI_RX_ISR(void)
{
    unsigned char val;
    static Uint16 crcVerif;
    
    #if ACTIVATE_RXTX_LED == 1
    	extern Uint16 RxTxLEDCtr;
    #endif
     
    EINT;		// Reentrancy is now possible depending of the speed. Be carefull
        
    /* Activation de l'interruption en cas d'erreur ????
     * Si oui, activer RX ERR INT ENA dans SCICTL 1 
     * Et voir si RX ERROR dans RXST est activé */
     
    #if ACTIVATE_RXTX_LED == 1
		RxTxLEDCtr  = RXTX_LED_PRD;
		GPIO_RX_LED = 0;  // LED On
    #endif
    
    // Handshaking management
    if(SciaRegs.SCIFFRX.bit.RXFFST >= SCI_MAX_RX_FIFO)
    	SciCtsTo(1);			// CTS to 1 => RX paused
    else
		SciCtsTo(0);			// CTS to 0 => RX ON

	// Interrupt appear when FIFO == 1. There is always 1 char to read
 	val = SciaRegs.SCIRXBUF.bit.RXDT;

	if(SciaRegs.SCIRXBUF.bit.SCIFFFE == 1) // Reception Error
		val = 0;

 	if((SciRx.State != CRC_HI_WAIT) & (SciRx.State != CRC_LO_WAIT))				// Add the effect of all bytes to CRC, until Data End
 		crcVerif = crc_update_byte(crcVerif, val);

	// Protocole Interpretation State Machine
	switch(SciRx.State)
	{
		case STX_WAIT :
			if(val == STX)
			{
				SciRx.State 		 = SID_WAIT;
				SciRx.NByteReceived = 0x0000; 							// Start a new frame
				SciRx.Length 		 = 0x0000;
				crcVerif 			 = crc_init();
				crcVerif			 = crc_update_byte(crcVerif, val); 	// Begin the CRC with the STX
			}
			break;
		case SID_WAIT :
			if(val == SID | val == SID_BROADCAST)
				SciRx.State = LEN_HI_WAIT;
			else
				SciRx.State = STX_WAIT;
			break;
			
		case LEN_HI_WAIT :
			SciRx.Length = val<<8;
			SciRx.State  = LEN_LO_WAIT;
			break;
		case LEN_LO_WAIT :
			SciRx.Length |= val;
			if(SciRx.Length > MAX_WR_MSG_LEN)
				SciRx.State = STX_WAIT;		// MSG too long
			else
				SciRx.State = DAT_WAIT;
			break;
			
		case DAT_WAIT :
			if((SciRx.NByteReceived % 2) == 0)
				SciRx.Datas[SciRx.NByteReceived>>1] = val<<8;
			else
				SciRx.Datas[SciRx.NByteReceived>>1] |= val;
				
			SciRx.NByteReceived++;
			if(SciRx.NByteReceived >= SciRx.Length)
				SciRx.State = CRC_HI_WAIT;
			else
				SciRx.State = DAT_WAIT;
			break;
			
		case CRC_HI_WAIT :
			SciRx.Crc = val<<8;
			SciRx.State = CRC_LO_WAIT;
			break;
			
		case CRC_LO_WAIT :
			SciRx.Crc |= val;
			if (crcVerif == SciRx.Crc)
			{
				CmdTreatment();
				SciRx.State = ETX_WAIT;
			}
			else
				SciRx.State = STX_WAIT;
			break;

		case ETX_WAIT :
			if(val == ETX)					// Flush buffer if more datas then length
				SciRx.State = STX_WAIT;	// Also test the STX in case of a new message starting while still in this state
			break;
			
		default :
			SciRx.State = STX_WAIT;
		
	}
     
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag

    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack

}

//-----------------------------------------------------------------------------
// Treat the possible commands
inline void CmdTreatment()
{	
	Uint16 cmd = HIGHBYTE(SciRx.Datas[0]);
	Uint32 adr = 0x00000000;
	static Uint16 val = 0x0000;
	
    // Command Interpretation
 	switch(cmd)
	{
		case 'R' :
			// LOWBYTE(SciRx.Datas[0]);		// Mem Type, not yet implemented
			adr = SciRx.Datas[1];
			adr = adr<<16;
			adr+= SciRx.Datas[2];
			SciNonBlockingSend((Uint16 *) adr, SciRx.Datas[3]);
			break;
				
		case 'W' :
			// LOWBYTE(SciRx.Datas[0]);		// Mem Type, not yet implemented
			adr = SciRx.Datas[1];
			adr = adr<<16;
			adr+= SciRx.Datas[2];

			// memcpy((Uint16 *) adr, &SciRx.Datas[4], SciRx.Datas[3] );		// problem with the size argument that freeze the DSP (at least after CCS v6.2.0)

			switch (SciRx.Datas[3])
			{
				case 1:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0001);
					break;
				case 2:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0002);
					break;
				case 3:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0003);
					break;
				case 4:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0004);
					break;
				case 5:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0005);
					break;
				case 6:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0006);
					break;
				case 7:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0007);
					break;
				case 8:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0008);
					break;
				case 9:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0009);
					break;
				case 10:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000A);
					break;
				case 11:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000B);
					break;
				case 12:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000C);
					break;
				case 13:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000D);
					break;
				case 14:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000E);
					break;
				case 15:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x000F);
					break;
				case 16:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0010);
					break;
				case 17:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0011);
					break;
				case 18:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0012);
					break;
				case 19:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0013);
					break;
				case 20:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], 0x0014);
					break;
				default:
					memcpy((Uint16 *) adr, &SciRx.Datas[4], SciRx.Datas[3]);		// problem with the size argument that freeze the DSP (at least after CCS v6.2.0)
					break;
			}

			SciNonBlockingSend((Uint16 *) adr, 0);
			break;
		
		case 'E' :
			val = SciRx.Datas[0];
			SciNonBlockingSend(&val, 1);
			break;
		case 'S' :
			val = 0x0000;
			SciNonBlockingSend(&val, 1);
			break;
		case 'Z' :
			// Not Yet implemented 
			break;
					
		default :
			break;
	}
    


}

//-----------------------------------------------------------------------------
// Start a data sector transmission. Interruption is enabled here. 
// Bytes transmission are then managed automatically by the SCI_TX_Interrupt
void SciNonBlockingSend(Uint16* adr, Uint16 size)
{
	while(SciTx.IsSending == 1);
	
	SciTx.IsSending = 1;
    SciTx.Adr 	     = adr;
    SciTx.Length    = size;
    SciTx.NByteSent = 0;

	// Enable Interrupt, only after that datas have to be sent
	PieCtrlRegs.PIEIER9.bit.INTx2=1;     // SCIATX, Group 9, INT4
}

int SciTxCtsRead()
{
	// Handshaking Management
	return 1;					// Not yet implemented
}

void SciCtsTo(Uint16 val)
{
	// Handshaking Management
	
}

