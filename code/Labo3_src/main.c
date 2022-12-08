/*------------------------------------------------------------------------**
** File:		main.c													  **
** Author:		Courteau Guillaume, IESE								  **	
** Date:		16 novembre 2022      									  **
** 																		  **
** Project:		Labo MA_PowElSys									      **
**																		  **
** Description:															  **
** Séance 3 - Snychronisation de l'onduleur triphasé sur le réseau        **
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
#define	EnPWM		GpioDataRegs.GPADAT.bit.GPIO27
#define	nPWMout		GpioDataRegs.GPADAT.bit.GPIO26
#define	LED1		GpioDataRegs.GPBDAT.bit.GPIO48
#define	LED2		GpioDataRegs.GPBDAT.bit.GPIO49
#define	LED3		GpioDataRegs.GPBDAT.bit.GPIO50  

#define	DIV3		0.33333	    		// 1.0/3.0
#define	SQRT3_1		0.57735 			// 1/sqrt(3.0)
#define OMEGA 		314.159 			// 2*PI*50
#define PI			3.1416


/*------------------------------------------------------------------------**
**						 	Global vars									  **
**------------------------------------------------------------------------*/

// PWM
//////// Paramètres
float			f_porteuse = 5000.0;					// fréquence du signal triangulaire
float 			deadband = 3.0;							// Temps d'anti-chevauchement
float			phase = 0.0;							// Phase entre les signals triangulaire
float			T_period, T_deadband, T_phase = 0.0;
int				DivClk;									// Diviseur d'horloge PWM
//////// Ucm
float     		f_sin = 50.0;							// fréquence Ucm
float 			w = 0.0;								// pulsation Ucm
float 			T_ech;									// Période échantillonnage Ucm
float   		cos_1, cos_2, cos_3;					// Signal de commande du PWM1,2 et 3 sans composante DC
float   		cm_bo_1, cm_bo_2, cm_bo_3;				// Signal de commande du PWM1,2 et 3 avec composante DC et ajustée à la porteuse en boucle ouverte
float   		ratio;      							// Rapport entre Ucm et la porteuse et également composante DC 
float   		ma = 0.5;      							// taux de modulation
float	     	k = 0.0; 								// numéro de l'échantillon
float     		nb_ech; 								// nombre d'échantillon
float   		phi;        							// Déphasage entre signaux de commandes
//////// Mesures
float 			ratio_ADC = 2048;						// 2^12/2 (sur 12 bits car 4 bits sont réservés)
float			inv_ratio_ADC_2 = 447.3/2048;			// Pleine échelle de mesure = 450 V + erreur résistances = -0.6%) / 2^11
float 			U_nom = 325.0;
float			gain_U = 0.0;
float 		    U_L1, U_L2, U_L3 = 0.0;
float			U_L_grid_mono = 0.0;
float           U_L1_onduleur = 0.0;
float			U_L2_onduleur = 0.0;
//////// PLL
float 			b0_s;
float 			b1_s;
float 			cos_theta;
float 			sin_theta;
float 			Ualpha = 0.0;
float 			Ubeta = 0.0;
float			Ud; 
float           Uq_var; // variable seule pour l'avoir dans Matlab
float           U_comm_ideal; // variable seule pour l'avoir dans Matlab
float	 		Uq[2] = {0.0,0.0};  // il nous faut la valeur précédante
float			Tpe = 0.0;
float           Ti = 0.0;
float           Tn = 0.0;
float			alpha = 30.0;
float			KP_S = 0.0;
float			KI_S = 0.0;
float 			Delta_w = 0.0;
float 			w0[2] = {0.0,0.0};
float			theta = 0.0;


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
  InitEPwm1(f_porteuse, deadband, phase);          
  InitEPwm2(f_porteuse, deadband, phase);
  InitEPwm3(f_porteuse, deadband, phase);
  
  // Initialize ADC modules
  InitAdc();
  
  // Enable PWMs
  nPWMout = 0;
  EnPWM   = 1;   // Désactive les PWM

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

     PieCtrlRegs.PIEIER9.bit.INTx1=1;     // SCIARX, PIE Group 9, INT1
     IER |= 0x0100;                       // Enable CPU INT for Group 1 et 9
  // END "Code to be integrated in main.c before global interrupt enable"

  EINT;
 
  // Main program loop 
  while(1)
  {	
	//----- Modifier valeurs parametres "f_sin", "ma", "f_porteuse" et "deadband" -----//
	
	//----------------------//
	//       Sécurité       //
	//----------------------//
	
	// Fréquence de Ucm
	if(f_sin < 0.0 || f_sin > 100.0)
	{
		f_sin = 0.0;
	}
	// Taux de modulation
	if(ma < 0.0 || ma > 1.0)
	{
		ma = 0.0;
	}
	// Fréquence de Uh
	if(f_porteuse < 500 || f_porteuse > 25000)
	{
		f_porteuse = 5000;
	}
	// Temps d'anti-chevauchement
	if(deadband < 2 || deadband > 10)
	{
		deadband = 3;
	}
	// Phase des Uh
	if(phase < 0.0 || phase > 180.0)
	{
		phase = 0.0;
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
	
	// Cette partie est à remplir de la même manière que pour le labo 2 en BO
	
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
		
	ratio	= ((float)EPwm1Regs.TBPRD)/2.0;			// Permet l'ajustage des signaux Ucm par rapport aux signaux Uh (PWM1,2 et 3)

	EnPWM   = 0;									// activer PWM

  }
}

//======================= Routine d'interruption ADC =======================//
// INT1.6
interrupt void ADCINT_ISR(void)     // ADC
{ 	
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
	
	nb_ech = f_porteuse/f_sin;
	ratio = T_period/2;

	if (k < nb_ech)
    {
        k = k+1.0;
    }
    else
    {
        k = 0.0;
    }

	//----------------------//	
	// 		 Mesures U	    //
	//----------------------//
	
	U_L1 = ((AdcRegs.ADCRESULT0 >> 4)-ratio_ADC)*inv_ratio_ADC_2;
	U_L2 = ((AdcRegs.ADCRESULT1 >> 4)-ratio_ADC)*inv_ratio_ADC_2;
	U_L3 = ((AdcRegs.ADCRESULT2 >> 4)-ratio_ADC)*inv_ratio_ADC_2;

	
	//----------------------//	
	// synchronisation PLL  //
	//----------------------//
  	
	// Transformee de Clarke (triphasé -> biphasé réf. fixe)

	Ualpha = DIV3*(2*U_L1-U_L2-U_L3);
	Ubeta = SQRT3_1*(U_L2-U_L3);

	// Parametres de l'angle (cos_theta & sin_theta)

	cos_theta = cos(theta);
	sin_theta = sin(theta);

	// Transformee de Park (biphasé réf. fixe -> biphasé réf. tournant)

	Ud = cos_theta*Ualpha + sin_theta*Ubeta;
	Uq[0] = -sin_theta*Ualpha + cos_theta*Ubeta;
	
	// Calcul des paramètres du régulateur de la PLL

	//Tpe = 1/f_porteuse;
	Tpe = 1.5 * T_ech;
	Ti = Tpe*Tpe*U_nom*alpha*alpha*alpha;
	//Tn = 8*Tpe;
	Tn = alpha*alpha*Tpe;

  	// Discrétisation du régulateur PI (Tustin) + frequence reseau
    KI_S = 1/Ti;
    KP_S = Tn*KI_S;

  	
  	// Integrateur de frequence via Tustin

    Delta_w = Uq[0]*(KP_S + 0.5*KI_S*T_ech)+Uq[1]*(0.5*KI_S*T_ech-KP_S)+Delta_w;

    //Delta_w = (KP_S + KI_S*T_ech/2.0)*Uq[0] + (KI_S*T_ech/2.0 - KP_S)*Uq[1] + Delta_w;
    //Delta_w = (KP_S + KI_S*T_ech/2)*Uq[1] + (KI_S*T_ech/2 - KP_S)*Uq[0] + Delta_w;
    //Delta_w = Uq[1] *(T_ech-2.0*Tn)/(2.0*Ti)+Uq[0]*(T_ech+2.0*Tn)/(2.0*Ti) + Delta_w;
    w0[0] = Delta_w+OMEGA;

    //theta = (2.0/T_ech)*w0[0]+(2.0/T_ech)*w0[1]-theta;
    theta = theta + (w0[1]+w0[0])/2.0 * T_ech;

	// Limite theta
    if(theta > 2.0*PI)
	{
	   	theta = theta - 2.0*PI;
	}
	
	if(theta < -2.0*PI)
	{
	 	theta = theta + 2.0*PI;
	}
	
    // Variables pour MAT2DSP

	//		*****************
	//		*  A compléter  *
	//		*****************

	// Mise a jour des variables
 	Uq[1] = Uq[0];
	w0[1] = w0[0];
	Uq_var = Uq[0];

	U_comm_ideal = Ud*cos(theta); // pour l'afficher dans Matlab
	
	
	// Cette partie est à remplir de la même manière que pour le labo 2 en BO
	
	// Signaux de commandes Ucm
	cos_1 = cos(theta+PI); //
	cos_2 = cos(theta-phi+PI);
	cos_3 = cos(theta-2*phi+PI);
   
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
