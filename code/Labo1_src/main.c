/*------------------------------------------------------------------------**
** File:		main.c													  **
** Author:		Courteau Guillaume, IESE								  **	
** Date:		29 octobre 2021      									  **
** 																		  **
** Project:		Labo MA PowElSys									      **
**																		  **
** Description:															  **
** Séance 1 : Introduction au DSP 									      **
**																		  **
**------------------------------------------------------------------------*/

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File


#include "Mat2DSP_sci.h"
#include "Mat2DSP_config.h"
#include "Mat2DSP_crc.h"
#include "Mat2DSP_RT_scope.h"
#include "Mat2DSP_types.h"


#include "math.h"

/*------------------------------------------------------------------------**
**						 	inputs/outputs							  	  **
**------------------------------------------------------------------------*/
#define	LED1		GpioDataRegs.GPBDAT.bit.GPIO48
#define	LED2		GpioDataRegs.GPBDAT.bit.GPIO49
#define	LED3		GpioDataRegs.GPBDAT.bit.GPIO50  


/*------------------------------------------------------------------------**
**						 	Global vars									  **
**------------------------------------------------------------------------*/

/*------------------------------------------------------------------------**
**						 	Global const.								  **
**------------------------------------------------------------------------*/


/*------------------------------------------------------------------------**
**						 	   Main										  **
**------------------------------------------------------------------------*/

void main(void)
{                 
  // Step 1. Initialize System Control:
  // PLL, WatchDog, enable Peripheral Clocks
  InitSysCtrl();    
        
  // Step 2. Initalize GPIO:
  InitGpio();   

  // Step 3. Clear all interrupts and initialize PIE vector table:
  // Disable CPU interrupts
  DINT;
 
  // Initialize the PIE control registers to their default state.
  // The default state is all PIE interrupts disabled and flags
  // are cleared.
  InitPieCtrl();

  // Disable CPU interrupts and clear all CPU interrupt flags:
  IER = 0x0000;
  IFR = 0x0000;

  // Initialize the PIE vector table with pointers to the shell Interrupt
  // Service Routines (ISR).
  // This will populate the entire table, even if the interrupt
  // is not used in this example.  This is useful for debug purposes.
  InitPieVectTable();

	
  // Initialize ePwm module(s) 
  // InitEPwmx(frequency [Hz], deadtime [us], phase [°]) 
  // for function definition, look at file "EPwm.c"
  InitEPwm1(10,0,0);
  InitEPwm2(0,0,0);
  InitEPwm3(0,0,0);
  
  // Initialize ADC modules
  InitAdc();
  
  EINT;
 
  // Main program loop 
  while(1)
  {	
	
  }
}

//======================= Routine d'interruption ADC =======================//
// INT1.6
interrupt void ADCINT_ISR(void)     // ADC
{ 	
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

	// Ecrire le code ici
	GpioDataRegs.GPBTOGGLE.bit.GPIO48 = 1;  // LED1 ^= 1;
	GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;  // LED2 ^= 1;
	

    // To receive more interrupts from this PIE group, acknowledge this interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/*------------------------------------------------------------------------**
**		No More									  						  **
**------------------------------------------------------------------------*/
