/*------------------------------------------------------------------------**
** Fichier:		Gpio.c													  **
** Auteur:		Serge Gavin, IESE										  **	
** Date:		5 mai 2008												  **
** 																		  **
** Description:	Contient la fontion permettant d'initialiser les GPIOs	  **
**																		  **
**------------------------------------------------------------------------*/

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

/*------------------------------------------------------------------------**
**						 	InitGpio									  **
**------------------------------------------------------------------------*/
void InitGpio(void)
{
   EALLOW;	
   
      									 //  = 00	 = 01		 = 10		 = 11  
										 // ------------------------------------------
   GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 01; // GPIO0  	EPWM1A  	Reserved 	Reserved
   GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 01; // GPIO1  	EPWM1B  	ECAP6	  	MFSRB
   GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 01; // GPIO2  	EPWM2A  	Reserved 	Reserved
   GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 01; // GPIO3  	EPWM2B  	ECAP5	 	MCLKRB
   GpioCtrlRegs.GPAMUX1.bit.GPIO4  = 01; // GPIO4  	EPWM3A  	Reserved 	Reserved
   GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 01; // GPIO5  	EPWM3B  	MFSRA		ECAP1
   GpioCtrlRegs.GPAMUX1.bit.GPIO6  = 00; // GPIO6  	EPWM4A  	EPWMSYNCI 	EPWMSYNCO
   GpioCtrlRegs.GPAMUX1.bit.GPIO7  = 00; // GPIO7  	EPWM4B  	MCLKRA		ECAP2
   GpioCtrlRegs.GPAMUX1.bit.GPIO8  = 00; // GPIO8  	EPWM5A  	CANTXB 		ADCSOCAO
   GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 00; // GPIO9  	EPWM5B  	SCITXDB 	ECAP3
   GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 00; // GPIO10 	EPWM6A  	CANRXB 		ADCSOCBO
   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 00; // GPIO11 	EPWM6B  	SCIRXDB 	ECAP4
   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 00; // GPIO12 	TZ1 	 	CANTXB 		SPISIMOB					   
   GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 00; // GPIO13 	TZ2  		CANRXB		SPISOMIB
   GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 00; // GPIO14 	TZ3  		SCITXDB 	SPICLKB
   GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 00; // GPIO15 	TZ4  		SCIRXDB 	SPISTEB	

   GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 00; // GPIO16 	SPISIMOA  	CANTXB 		TZ5
   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 00; // GPIO17 	SPISOMIA  	CANRXB 		TZ6	
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 00; // GPIO18 	SPICLKA  	SCITXDB 	CANRXA	
   GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 00; // GPIO19	SPISTEA  	SCIRXDB 	CANTXA
   GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 00; // GPIO20 	EQEP1A 		MDXA	 	CANTXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 00; // GPIO21 	EQEP1B  	MDRA	 	CANRXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 00; // GPIO22 	EQEP1S  	MCLKXA		SCITXDB
   GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 00; // GPIO23 	EQEP1I  	MFSXA		SCIRXDB
   GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 00; // GPIO24 	ECAP1  		EQEP2A		MDXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 00; // GPIO25 	ECAP2		EQEP2B		MDRB
   GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 00; // GPIO26 	ECAP3		EQEP2I		MCLKXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 00; // GPIO27 	ECAP4		EQEP2S		MFSXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 01; // GPIO28 	SCIRXDA		XZCS6		XZCS6
   GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 01; // GPIO29 	SCITXDA		XA19		XA19
   GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 00; // GPIO30 	CANRXA		XA18		XA18
   GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 00; // GPIO31 	CANTXA		XA17		XA17

										 //  = 00	 = 01		 = 10		 = 11  
										 // ------------------------------------------
   GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 00; // GPIO32 	SDAA		EPWMSYNCI	ADCSOCAO
   GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 00; // GPIO33 	SCLA		EPWMSYNCO	ADCSOCBO
   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 00; // GPIO34 	ECAP1		XREADY		XREADY
   GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 00; // GPIO35	SCITXDA		XR/W		XR/W
   GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 00; // GPIO36	SCIRXDA		XZCS0		XZCS0
   GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 00; // GPIO37	ECAP2		XZCS7		XZCS7
   GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 00; // GPIO38	Reserved	XWE0		XWE0	
   GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 00; // GPIO39	Reserved	XA16		XA16
   GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 00; // GPIO40 	Reserved	XA0			XA0	
   GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 00; // GPIO41 	Reserved	XA1			XA1	
   GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 00; // GPIO42 	Reserved	XA2			XA2
   GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 00; // GPIO43 	Reserved	XA3			XA3
   GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 00; // GPIO44 	Reserved	XA4			XA4	
   GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 00; // GPIO45 	Reserved	XA5			XA5
   GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 00; // GPIO46 	Reserved	XA6			XA6
   GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 00; // GPIO47 	Reserved	XA7			XA7

   GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 00; // GPIO48 	ECAP5			XD31
   GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 00; // GPIO49 	ECAP6			XD30 
   GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 00; // GPIO50 	EQEP1A			XD29
   GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 00; // GPIO51	EQEP1B			XD28
   GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 00; // GPIO52 	EQEP1S			XD27
   GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 00; // GPIO53 	EQEP1I			XD26
   GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 00; // GPIO54 	SPISIMOA		XD25
   GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 00; // GPIO55 	SPISOMIA		XD24
   GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 00; // GPIO56 	SPICLKA			XD23
   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 00; // GPIO57 	SPISTEA			XD22
   GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 00; // GPIO58 	MCLKRA			XD21
   GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 00; // GPIO59 	MFSRA			XD20
   GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 00; // GPIO60 	MCLKRB			XD19
   GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 00; // GPIO61 	MFSRB			XD18
   GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 00; // GPIO62 	SCIRXDC			XD17 
   GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 00; // GPIO63 	SCITXDC			XD16

   										 //  = 00 / 01		 	= 10 / 11  
										 // ------------------------------------------
   GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 00; // 	GPIO64				XD15
   GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 00; // 	GPIO65				XD14
   GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 00; // 	GPIO66				XD13
   GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 00; // 	GPIO67				XD12
   GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 00; // 	GPIO68				XD11
   GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 00; // 	GPIO69				XD10
   GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 00; // 	GPIO70				XD9
   GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 00; // 	GPIO71				XD8
   GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 00; // 	GPIO72				XD7
   GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 00; // 	GPIO73				XD6 
   GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 00; // 	GPIO74				XD5
   GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 00; // 	GPIO75				XD4
   GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 00; // 	GPIO76				XD3 
   GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 00; // 	GPIO77				XD2 
   GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 00; // 	GPIO78				XD1 
   GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 00; // 	GPIO79				XD0 
   
   GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 00; // 	GPIO80				XA8
   GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 00; // 	GPIO81				XA9
   GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 00; // 	GPIO82				XA10
   GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 00; // 	GPIO83				XA11
   GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 00; // 	GPIO84				XA12
   GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 00; // 	GPIO85				XA13
   GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 00; // 	GPIO86				XA14
   GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 00; // 	GPIO87				XA15

   // Gpio Direction : 0 = Input, 1 = Output
   GpioCtrlRegs.GPADIR.all = 0x0000;      // GPIO0-GPIO31   0 = Input 
   GpioCtrlRegs.GPBDIR.all = 0x0000;      // GPIO32-GPIO63  0 = Input
   GpioCtrlRegs.GPCDIR.all = 0x0000;      // GPIO64-GPIO87  0 = Input
   
   GpioCtrlRegs.GPBDIR.bit.GPIO35 = 1;	  // RnW
   GpioCtrlRegs.GPBDIR.bit.GPIO36 = 1;	  // CS
   
   GpioCtrlRegs.GPADIR.bit.GPIO1  = 1;    // used to pull down

   GpioCtrlRegs.GPCDIR.bit.GPIO74 = 1;	  // used to pull down
   GpioCtrlRegs.GPCDIR.bit.GPIO75 = 1;	  // used to pull down
   GpioCtrlRegs.GPCDIR.bit.GPIO76 = 1;	  // used to pull down

   GpioDataRegs.GPADAT.bit.GPIO1  = 0;
   
   GpioDataRegs.GPBDAT.bit.GPIO35 = 0;
   GpioDataRegs.GPBDAT.bit.GPIO35 = 0;
   
   GpioDataRegs.GPCDAT.bit.GPIO74 = 0;
   GpioDataRegs.GPCDAT.bit.GPIO75 = 0;
   GpioDataRegs.GPCDAT.bit.GPIO76 = 0;
   
	
   GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;	  // nPWMout
   GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;	  // EnPWM
  
 // A COMPLETER
 

   GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;	  // LED 1
   GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;	  // LED 2
   GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;	  // LED 3

   GpioDataRegs.GPBDAT.bit.GPIO48 ^=1; // Toggle
   


   // Each input can have different qualification
   // a) input synchronized to SYSCLKOUT
   // b) input qualified by a sampling window
   // c) input sent asynchronously (valid for peripheral inputs only)
   GpioCtrlRegs.GPAQSEL1.all = 0x0000;    // GPIO0-GPIO15 Synch to SYSCLKOUT 
   GpioCtrlRegs.GPAQSEL2.all = 0x0000;    // GPIO16-GPIO31 Synch to SYSCLKOUT
   GpioCtrlRegs.GPBQSEL1.all = 0x0000;    // GPIO32-GPIO47 Synch to SYSCLKOUT 
   GpioCtrlRegs.GPBQSEL2.all = 0x0000;    // GPIO48-GPIO63 Synch to SYSCLKOUT


   // Pull-ups can be enabled or disabled. 
   GpioCtrlRegs.GPAPUD.all = 0x0000;      // Pullup's enabled GPIO0-GPIO31
   GpioCtrlRegs.GPBPUD.all = 0x0000;      // Pullup's enabled GPIO32-GPIO63
   GpioCtrlRegs.GPCPUD.all = 0x0000;      // Pullup's enabled GPIO64-GPIO87

   EDIS;

}	
	
//===========================================================================
// End of file.
//===========================================================================
