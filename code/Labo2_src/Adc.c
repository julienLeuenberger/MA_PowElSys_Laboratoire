/*------------------------------------------------------------------------**
** File:        Adc.c                                                     **
** Author:      Serge Gavin, IESE                                         **
** Date:        9 december 2008                                           **
**                                                                        **
** Description: Funtions used to initialize adc peripheral                **
**																	      **
** Mis à jour par Guillaume Courteau le 26.01.2022                        **
**																		  **
**                                                                        **
**------------------------------------------------------------------------*/

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_usDELAY  5000L

/*------------------------------------------------------------------------**
** function:    InitAdc                                                   **
** Author:      Serge Gavin, IESE                                         **
** Date:        9. dec 2008                                               **
**------------------------------------------------------------------------*/
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);


    // *IMPORTANT*
    // The ADC_cal function, which  copies the ADC calibration values from TI reserved
    // OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
    // Boot ROM. If the boot ROM code is bypassed during the debug process, the
    // following function MUST be called for the ADC to function according
    // to specification. The clocks to the ADC MUST be enabled before calling this
    // function.
    // See the device data manual and/or the ADC Reference
    // Manual for more information.

        EALLOW;
        SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
//      ADC_cal();
        EDIS;

    AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
/*
  bit 15-8          :   Reserved
  bit 7-6       11  :   ADCBGRFDN   Power up bandgap/reference/ADC circuits
  bit 5         1   :   ADCPWDN     All analog circuitry in core is powered up
  bit 4-1       0000:   ADCCLKPS    Core clock => see datasheet
  bit 0         0   :   SMODE_SEL => Sequential mode
*/
    //  DSP28x_usDelay(5000);                  // Delay before converting ADC channels

    AdcRegs.ADCTRL1.all = 0x0380;   // ADC control Reg 1
/* 
  bit 15        0   :   reserved
  bit 14        0   :   RESET
  bit 13-12     00  :   SUSMODE     00 = don't stop on emulation suspend
  bit 11-8      0011:   ACQ_PS      Acquisition window time 0011 = 3*ADCclk
  bit 7         1   :   CPS         Core clock prescaler 0 = x/1
  bit 6         0   :   CONT_RUN    Continuous run 0 = Start/Stop mode
  bit 5         0   :   SEQ_OVRD    Sequencer Override
  bit 4         0   :   SEQ_CASC    Casquaded sequencer operation 1=cascaded
  bit 3-0       0000:   reserved
*/

    AdcRegs.ADCTRL2.all = 0x0900;   // ADC control Reg 2
/* 
  bit 15        0   :   EPWM_SOCB_SEQ   1 = sequencer started by EPwm_SOCB
  bit 14        0   :   RST_SEQ1        Reset sequencer 1
  bit 13        0   :   SOC_SEQ1        Force Start Of Convertion of Sequencer 1
  bit 12        0   :   reserved
  bit 11        1   :   INT_ENA_SEQ1    1 = Sequencer 1 interrupt enable
  bit 10        0   :   INT_MOD_SEQ1    SEQ1 interrupt mode => A METTRE A ZERO SINON L'INTERRUPTION ADC SE LANCE 1 FOIS SUR 2
  bit 9         0   :   reserved
  bit 8         1   :   EPWM_SOCA_SEQ1  1 = sequencer1 started by EPwm_SOCA
  bit 7         0   :   EXT_SOC_SEQ1    1 = Enable external SOC from GPIOA
  bit 6         0   :   RST_SEQ2        Reset sequencer 2
  bit 5         0   :   SOC_SEQ2        Force Start Of Convertion of Sequencer 2
  bit 4         0   :   reserved
  bit 3         0   :   INT_ENA_SEQ2    1 = Sequencer 2 interrupt enable
  bit 2         0   :   INT_MOD_SEQ2    SEQ2 interrupt mode
  bit 1         0   :   reserved
  bit 0         0   :   EPWM_SOCB_SEQ2  1 = sequencer2 started by EPwm_SOCB
*/

    AdcRegs.ADCMAXCONV.all = 0x0003;    // Number of conversions of Seq 1 and Seq 2
/* 
  bit 15-7      0   :   reserved
  bit 6-4       000 :   Max Conv sequencer 2 (0 means 1 conversion)
  bit 3-0       0000:   Max Conv sequencer 1 (0 means 1 conversion)
*/

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x00; // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x01; // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x02; // Setup ADCINA2 as 3rd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x03; // Setup ADCINA3 as 4th SEQ1 conv.
    
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x04; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x05; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x06; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x07; // Chanels to convert...
    
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x08; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x09; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0x0A; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0x0B; // Chanels to convert...
   
    AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0x0C; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0x0D; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 0x0E; // Chanels to convert...
    AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 0x0F; // Chanels to convert...

    // Clear pendings interrupt from Seq1 and 2
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    AdcRegs.ADCST.bit.INT_SEQ2_CLR = 1;

   // Enable ADCINT in PIE
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    IER |= 0x0001;

}

//===========================================================================
// End of file.
//===========================================================================
