/*------------------------------------------------------------------------**
**																		  **
** Project:     	Laboratoire EPR  (Master)							  **
** 																		  **
** Professor:       Mauro Carpita										  **
** Assistants:      Michael De Vivo, Bastian Chappuis                     **
** 																		  **
** File:			main.c												  **
** Author:		    Chappuis Bastian, IESE								  **
** Date:			10. january 2015									  **
** Revision :		Courteau Guillaume, IESE					     	  **
** Date:			27.01.2022											  **
** Version:         3													  **
**																		  **
** Séance 2 - Onduleur triphasé en boucle ouverte (fichier de démarrage)  **
** 																		  **
**------------------------------------------------------------------------*/

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"

#include "Mat2DSP_sci.h"
#include "Mat2DSP_config.h"
#include "Mat2DSP_crc.h"
#include "Mat2DSP_RT_scope.h"
#include "Mat2DSP_types.h"


/*------------------------------------------------------------------------**
**						 	inputs/outputs							  	  **
**------------------------------------------------------------------------*/
#define	EnPWM		GpioDataRegs.GPADAT.bit.GPIO27
#define	nPWMout		GpioDataRegs.GPADAT.bit.GPIO26
#define	LED1		GpioDataRegs.GPBDAT.bit.GPIO48
#define	LED2		GpioDataRegs.GPBDAT.bit.GPIO49
#define	LED3		GpioDataRegs.GPBDAT.bit.GPIO50  

#define	DIV3		0.33333	    		// 1.0/3.0
#define	SQRT3_1		0.57735 			// 1/sqrt(3.0)
#define OMEGA 		314.159 			// 2*PI*50

#define PI				3.1416


/*------------------------------------------------------------------------**
**						 	Global vars									  **
**------------------------------------------------------------------------*/
// PWM
//////// Paramètres
float			f_porteuse = 5000.0;					// fréquence du signal triangulaire [Hz]
float 			deadband = 3.0;							// Temps d'anti-chevauchement  [us]
float			phase = 0.0;							// Phase entre les signaux triangulaires
float			T_period, T_deadband, T_phase = 0.0;
int				DivClk;									// Diviseur d'horloge PWM
//////// Ucm
float     		f_sin = 50.0;							// fréquence Ucm
float 			T_ech;									// Période échantillonnage Ucm
float   		cos_1, cos_2, cos_3;					// Signal de commande du PWM1,2 et 3 sans composante DC
float   		cm_bo_1, cm_bo_2, cm_bo_3;				// Signal de commande du PWM1,2 et 3 avec composante DC et ajustée à la porteuse en boucle ouverte
float   		ratio;      							// Rapport entre Ucm et la porteuse et également composante DC 
float			w;										// pulsation
float   		ma = 0.5;      							// taux de modulation
float	     	k = 0.0; 								// numéro de l'échantillon
float     		nb_ech; 								// nombre d'échantillon
float   		phi;        							// Déphasage entre signals de commandes

/*------------------------------------------------------------------------**
**						 	Global const.								  **
**------------------------------------------------------------------------*/
#define F_SIN_MAX 100.0
#define F_SIN_MIN 0.1 // éviter les division par 0
#define MA_MAX 1.0
#define MA_MIN 0.0
#define F_PORT_MAX 25000.0
#define F_PORT_MIN 500.0
#define DEADBAND_TIME_MAX_US 5.0
#define DEADBAND_TIME_MIN_US 2.01 // strictement plus grand
#define PHASE_PORT_MAX 180.0
#define PHASE_PORT_MIN 0.0

/*------------------------------------------------------------------------**
**						 	   Main										  **
**------------------------------------------------------------------------*/
int main(void) {
	
/** Step 1. Initialize System Control **/

	// PLL, WatchDog, enable Peripheral Clocks
	InitSysCtrl();

/** Step 2. Initialize GPIO **/

	InitGpio();

/** Step 3. Clear all interrupts and initialize PIE vector table **/

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

/** Step 4. Initialize PWM **/

	// Initialize ePwm module(s)
	// InitEPwmx(frequency [Hz], deadtime [us], phase [°])
	// for function definition, look at file "EPwm.c"
	InitEPwm1(f_porteuse, deadband, phase);
	InitEPwm2(f_porteuse, deadband, phase);
	InitEPwm3(f_porteuse, deadband, phase);

/** Step 5. Initialize ADC **/

	InitAdc();

	// Enable PWMs
	nPWMout = 0;
	EnPWM   = 1;    // Désactive les PWM

// Code to initialize Mat2DSP
// BEGIN "Code to be integrated in main.c before global interrupt enable"
   M2D_SciInitGpio();
   M2D_SciInit();  	 
   IniScope();
   
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.SCIRXINTA = &SCI_RX_ISR;
   PieVectTable.SCITXINTA = &SCI_TX_ISR;
   //EDIS;   // This is needed to disable write to EALLOW protected registers

   //PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
  
   PieCtrlRegs.PIEIER9.bit.INTx3=1;     // SCIARX, PIE Group 9, INT1
   IER |= 0x0100; 	                    // Enable CPU INT for Group 1 et 9
// END "Code to be integrated in main.c before global interrupt enable"

  EINT;



	// Main program loop
	while(1)
	{
		//----- Les valeurs des parametres "f_sin", "ma", "f_porteuse" et "deadband" seront modifiées à la main lors de l'utilisation -----//
		
		//----------------------//
		//       Sécurités       //
		//----------------------//

	    // Voir données Laboratoire n°2 pour les bornes des variables
		
		if (f_sin > F_SIN_MAX)
		{
		    f_sin = F_SIN_MAX;
		}
		if (f_sin < F_SIN_MIN)
		{
		    f_sin = F_SIN_MIN;
		}
		
		if (ma > MA_MAX)
        {
            ma = MA_MAX;
        }
        if (ma < MA_MIN)
        {
            ma = MA_MIN;
        }

        if (f_porteuse > F_PORT_MAX)
        {
            f_porteuse = F_PORT_MAX;
        }
        if (f_porteuse < F_PORT_MIN)
        {
            f_porteuse = F_PORT_MIN;
        }

        if (deadband > DEADBAND_TIME_MAX_US)
        {
            deadband = DEADBAND_TIME_MAX_US;
        }
        if (deadband < DEADBAND_TIME_MIN_US)
        {
            deadband = DEADBAND_TIME_MIN_US;
        }

        if (phase > PHASE_PORT_MAX)
        {
            phase = PHASE_PORT_MAX;
        }
        if (phase < PHASE_PORT_MIN)
        {
            phase = PHASE_PORT_MIN;
        }

		
		//----------------------//
		//   Modification PWM   //
		//----------------------//
		
		// Calculs paramètres 	
		T_ech         = 1/f_porteuse;						// Période d'échantillonage == Période signal triangulaire
		nb_ech 		  = f_porteuse/f_sin;					// Nombre d'échantillons pour signals Ucm
		w      		  = 2*PI*f_sin;							// Omega signals Ucm
		phi  		  = (2.0*PI)/3.0;						// Phase entre signals Ucm
		
		// Initialisation des signaux Uh (triangulaires)
		DivClk = 0;
		if (f_porteuse != 0.0)				
		{
		  // corresponding period and deadband values
		  T_period = (75e6/f_porteuse);			
		  T_deadband = (deadband * 150);
	  
		  // check if the clock division is necessary
		  while(T_period > 65535.0 || T_deadband > 1023.0)
		  {
			T_period = T_period*0.5;
			T_deadband = T_deadband*0.5;
			DivClk++;
		  }
		
		  // corresponding phase value
		  if (phase < 0.0)
			phase = -phase;
		  T_phase = phase/180*T_period;
		}
		
		///// PWM 1
		
		EPwm1Regs.TBPRD = (Uint16)T_period;         // Period Register
		EPwm1Regs.DBRED = (Uint16)T_deadband; 		// Dead-Band rising edge delay reg
		EPwm1Regs.DBFED = (Uint16)T_deadband;        // Dead-Band falling edge delay reg
		
		///// PWM 2  
		
		EPwm2Regs.TBPRD = (Uint16)T_period;			// Period Register
		EPwm2Regs.DBRED = (Uint16)T_deadband;        // Dead-Band rising edge delay reg
		EPwm2Regs.DBFED = (Uint16)T_deadband;        // Dead-Band falling edge delay reg
		
		///// PWM 3 			
		
		EPwm3Regs.TBPRD = (Uint16)T_period;		    // Period Register
		EPwm3Regs.DBRED = (Uint16)T_deadband;        // Dead-Band rising edge delay reg
		EPwm3Regs.DBFED = (Uint16)T_deadband;        // Dead-Band falling edge delay reg
		
		ratio		  = ((float)EPwm1Regs.TBPRD)/2.0;	// Permet l'ajustage des signaux Ucm par rapport aux signaux Uh (PWM1,2 et 3)

		EnPWM   = 0;									// activer PWM
	}
}

//------------- ADC ----------------------
// INT1.6
interrupt void ADCINT_ISR(void)
{
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

	LED1 ^= 1;

	nb_ech = f_porteuse/f_sin;
	ratio = T_period/2;

	 
	// Accroissement du numéro de l'échantillon

	if (k < nb_ech)
    {
        k = k+1.0;
    }
    else
    {
        k = 0.0;
    }
	// Signaux de commandes Ucm
	cos_1 = cos(w*k*T_ech);
	cos_2 = cos(w*k*T_ech-phi);
	cos_3 = cos(w*k*T_ech-2*phi);
   
	// Ajustement d'échelle du signal commande
	cm_bo_1 = ma*cos_1*ratio + ratio;
	cm_bo_2 = ma*cos_2*ratio + ratio;
	cm_bo_3 = ma*cos_3*ratio + ratio;

    // PWM (affectation des valeurs des signaux Ucm à la valeur du registre de comparaison)
	
	EPwm1Regs.CMPA.half.CMPA = (Uint16) cm_bo_1;
	EPwm2Regs.CMPA.half.CMPA = (Uint16) cm_bo_2;
	EPwm3Regs.CMPA.half.CMPA = (Uint16) cm_bo_3;

	
	RTScope(); // Fonction scope de Mat2DSP
	
  // To receive more interrupts from this PIE group, acknowledge this interrupt
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/*------------------------------------------------------------------------**
**		No More									  						  **
**------------------------------------------------------------------------*/




