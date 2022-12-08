/*------------------------------------------------------------------------**
** File:        EPwm.c                                                    **
** Author:      Serge Gavin, IESE                                         **
** Date:        7 may 2008                                                **
**                                                                        **
** Description: Funtions used to initialize the six EPwm modules          **
**                                                                        **
**------------------------------------------------------------------------*/

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void InitEPwm1(float frequency, float deadband_time, float phase);
void InitEPwm2(float frequency, float deadband_time, float phase);
void InitEPwm3(float frequency, float deadband_time, float phase);

/*------------------------------------------------------------------------**
** function:    InitEPwmx                                                 **
** Author:      Serge Gavin, IESE                                         **
** Date:        23. october 2008                                          **
**                                                                        **
** in parameters:                                                         **
**   - frequency    : float     carrier frequency in Hertz                **
**   - deadband_time: float     deadband time in microseconds             **
**   - phase        : float     carrier wave phase in degrees             **
**                                                                        **
** out parameters:                                                        **
**   - none                                                               **
**                                                                        **
** Notes:                                                                 **
**   Frequency of 0.0 means EPwm stopped                                  **
**   Phase parameter means phase between ePwmX and ePwm1                  **
**   Warning, parameters are in "float" format, don't forget the coma     **
**------------------------------------------------------------------------*/


/*------------------------------------------------------------------------**
**                          InitEPwm1                                     **
**------------------------------------------------------------------------*/
void InitEPwm1(float frequency, float deadband_time, float phase)
{

  float T_period, T_deadband, T_phase = 0.0;
  int   DivClk;

  DivClk = 0;
  if (frequency != 0.0)
  {
    // corresponding period and deadband values
    T_period = (75e6/frequency);
    T_deadband = (deadband_time * 150);
  
    // check if the clock division is necessary
    while(T_period > 65535.0 || T_deadband > 1023.0)
    {
      T_period = T_period/2;
      T_deadband = T_deadband/2;
      DivClk++;
    }

    // corresponding phase value
    if (phase < 0.0)
      phase = -phase;
    T_phase = phase/180*T_period;
  }


  EPwm1Regs.TBCTL.all = 0x0003;         // Disable TimeBase
  EPwm1Regs.TBPRD = (Uint16)T_period;   // Period Register
  EPwm1Regs.TBPHS.half.TBPHS =(Uint16)T_phase;// Phase shift Register
  EPwm1Regs.TBCTR = 0x0000;             // Counter Register
  
  EPwm1Regs.TBCTL.all = 0xE012;     // Time Base Ctrl reg
/* 
  bit 15-14     11  :   FREE,SOFT   11 = Free run
  bit 13        1   :   PHSDIR      1 = Count up
  bit 12-10     000 :   CLKDIV      0 = x/1
  bit 9-7       000 :   HSPCLKDIV   0 = x/1
  bit 6         0   :   SWFSYNC     Write 1 will force sync.
  bit 5-4       01  :   SYNCOSEL    source of SyncOut signal (00:SyncIn, 01:CTR=0, 10:CTR=CMPRB, 11:Disable)
  bit 3         0   :   PRDLD       0 = TBPRD shadowed
  bit 2         0   :   PHSEN       1 = use the TBPHS reg when a SyncIn occurs
  bit 1-0       10  :   CTRMODE     00 = up, 01 = Down, 10=Up/Down, 11=Stop
*/
  EPwm1Regs.TBCTL.bit.CLKDIV = DivClk;
  EPwm1Regs.TBCTL.bit.PHSDIR = (phase >= 0.0);

  EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD>>1;// Dutycycle = 0.5
  EPwm1Regs.CMPB = 0x0000;          // Compare reg B
 
  EPwm1Regs.CMPCTL.all = 0x0050;    // Compare Ctrl reg
/* 
  bit 15-10 0000000   :   reserved
  bit 9         0   :   SHDWBFULL   CMPB Full status bit
  bit 8         0   :   SHDWAFULL   CMPA Full status bit
  bit 7         0   :   reserved
  bit 6         1   :   SHDWBMODE   0 = CMPB shadowed
  bit 5         0   :   reserved
  bit 4         1   :   SHDWAMODE   0 = CMPA shadowed
  bit 3-2       00  :   LOADBMODE   Loading of CMPB mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
  bit 1-0       00  :   LOADAMODE   Loading of CMPA mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
*/

  EPwm1Regs.AQCTLA.all = 0x0060;    // Action qualifier output A
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       01  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       10  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxA output low
    * 10 = Set, force EPWMxA output high
    * 11 = Toggle EPWMxA
*/

  EPwm1Regs.AQCTLB.all = 0x0000;    // Action qualifier output B
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       00  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       00  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxB output low
    * 10 = Set, force EPWMxB output high
    * 11 = Toggle EPWMxB
*/

  EPwm1Regs.DBCTL.all = 0x000B;
/* 
  bit 15-6      0   :   reserved
  bit 5-4       00  :   INMODE      (00=EPWMxA is source for both falling and rising edge
                                     01=EPWMxA is source for falling edge, EPWMxB is source for rising edge,
                                     10=EPWMxA is source for rising edge, EPWMxB is source for fallingg edge,
                                     11=EPWMxB is source for both falling and rising edge)
  bit 3-2       10  :   POLSEL      (00=both active high, 01=active low compl. 10=active high comp. 11= both active low)
  bit 1-0       11  :   OUTMODE     (00=Bypassed, 01=Falling edge delay only, 10=rising edge delay, 11=both edge)
*/

  EPwm1Regs.DBRED = (Uint16)T_deadband;         // Dead-Band rising edge delay reg
  EPwm1Regs.DBFED = (Uint16)T_deadband;         // Dead-Band falling edge delay reg

  EPwm1Regs.ETSEL.all = 0x0900;     // Event-Trigger Selection Register
/* 
  bit 15        0   :   SOCBEN      1 = Enable ADC start of conversion B
  bit 14-12     000 :   SOCBSEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 11        1   :   SOCAEN      1 = Enable ADC start of conversion A
  bit 10-8      001 :   SOCASEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 7-4       0000:   reserved
  bit 3         0   :   INTEN       1 = Enable EPWMx_INT
  bit 2-0       000 :   INTSEL      (001: TBCTR = 0, 010: TBCRT=TBPRD
                                     100: CMPA up, 101: CMPA down
                                     110: CMPB up, 111: CMPB down)
*/

  EPwm1Regs.ETPS.all = 0x0100;      // Event-Trigger Prescale Register
/* 
  bit 15-14     00  :   SOCBCNT     read only
  bit 13-12     00  :   SOCBPRD     Number of event that generates the pulse
  bit 11-10     00  :   SOCACNT     read only
  bit 9-8       01  :   SOCAPRD     Number of event that generates the pulse
  bit 7-4       0000:   reserved
  bit 3-2       00  :   INTCNT      read only
  bit 1-0       00  :   INTPRD      Number of event that generates the int
*/

  //EPwm1Regs.ETCLR.all = 0xFFFF;       // Clear all pending interupts
  //PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
  //IER |= 0x0004;
}


/*------------------------------------------------------------------------**
**                          InitEPwm2                                     **
**------------------------------------------------------------------------*/
void InitEPwm2(float frequency, float deadband_time, float phase)
{
  float T_period, T_deadband, T_phase = 0.0;
  int   DivClk;

  DivClk = 0;
  if (frequency != 0.0)
  {
    // corresponding period and deadband values
    T_period = (75e6/frequency);
    T_deadband = (deadband_time * 150);
  
    // check if the clock division is necessary
    while(T_period > 65535.0 || T_deadband > 1023.0)
    {
      T_period = T_period/2;
      T_deadband = T_deadband/2;
      DivClk++;
    }

    // corresponding phase value
    if (phase < 0.0)
      T_phase = -phase/180*T_period;
    else
      T_phase = phase/180*T_period;
  }

  EPwm2Regs.TBCTL.all = 0x0003;     // Disable TimeBase
  EPwm2Regs.TBPRD = (Uint16)T_period;   // Period Register
  EPwm2Regs.TBPHS.half.TBPHS =(Uint16)T_phase;  // Phase shift Register
  EPwm2Regs.TBCTR = 0x0000;         // Counter Register
  
  EPwm2Regs.TBCTL.all = 0xE006;     // Time Base Ctrl reg
/* 
  bit 15-14     00  :   FREE,SOFT   11 = Free run
  bit 13        0   :   PHSDIR      1 = Count up
  bit 12-10     000 :   CLKDIV      0 = x/1
  bit 9-7       000 :   HSPCLKDIV   0 = x/1
  bit 6         0   :   SWFSYNC     Write 1 will force sync.
  bit 5-4       00  :   SYNCOSEL    source of SyncOut signal (00:SyncIn, 01:CTR=0, 10:CTR=CMPRB, 11:Disable)
  bit 3         0   :   PRDLD       0 = TBPRD shadowed
  bit 2         1   :   PHSEN       1 = use the TBPHS reg when a SyncIn occurs
  bit 1-0       10  :   CTRMODE     00 = up, 01 = Down, 10=Up/Down, 11=Stop
*/
  EPwm2Regs.TBCTL.bit.CLKDIV = DivClk;
  EPwm2Regs.TBCTL.bit.PHSDIR = (phase >= 0.0);


  EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD>>1;// Dutycycle = 0.5
  EPwm2Regs.CMPB = 0x0000;          // Compare reg B
 
  EPwm2Regs.CMPCTL.all = 0x0050;    // Compare Ctrl reg
/* 
  bit 15-10     0   :   reserved
  bit 9         0   :   SHDWBFULL   CMPB Full status bit
  bit 8         0   :   SHDWAFULL   CMPA Full status bit
  bit 7         0   :   reserved
  bit 6         0   :   SHDWBMODE   0 = CMPB shadowed
  bit 5         0   :   reserved
  bit 4         0   :   SHDWAMODE   0 = CMPA shadowed
  bit 3-2       00  :   LOADBMODE   Loading of CMPB mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
  bit 1-0       00  :   LOADAMODE   Loading of CMPA mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
*/

  EPwm2Regs.AQCTLA.all = 0x0060;    // Action qualifier output A
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       01  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       10  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxA output low
    * 10 = Set, force EPWMxA output high
    * 11 = Toggle EPWMxA
*/

  EPwm2Regs.AQCTLB.all = 0x0000;    // Action qualifier output B
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       00  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       00  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxB output low
    * 10 = Set, force EPWMxB output high
    * 11 = Toggle EPWMxB
*/

  EPwm2Regs.DBCTL.all = 0x000B;
  /*
    bit 15-6      0   :   reserved
    bit 5-4       00  :   INMODE      (00=EPWMxA is source for both falling and rising edge
                                       01=EPWMxA is source for falling edge, EPWMxB is source for rising edge,
                                       10=EPWMxA is source for rising edge, EPWMxB is source for fallingg edge,
                                       11=EPWMxB is source for both falling and rising edge)
    bit 3-2       10  :   POLSEL      (00=both active high, 01=active low compl. 10=active high comp. 11= both active low)
    bit 1-0       111  :   OUTMODE     (00=Bypassed, 01=Falling edge delay only, 10=rising edge delay, 11=both edge)
  */

  EPwm2Regs.DBRED = (Uint16)T_deadband;// Dead-Band rising edge delay reg
  EPwm2Regs.DBFED = (Uint16)T_deadband;// Dead-Band falling edge delay reg  

  EPwm2Regs.ETSEL.all = 0x0900;     // Event-Trigger Selection Register
/* 
  bit 15        0   :   SOCBEN      1 = Enable ADC start of convertion B
  bit 14-12     000 :   SOCBSEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 11        0   :   SOCAEN      1 = Enable ADC start of convertion A
  bit 10-8      000 :   SOCASEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 7-4       0   :   reserved
  bit 3         0   :   INTEN       1 = Enable EPWMx_INT
  bit 2-0       000 :   INTSEL      (001: TBCTR = 0, 010: TBCRT=TBPRD
                                     100: CMPA up, 101: CMPA down
                                     110: CMPB up, 111: CMPB down)
*/

  EPwm2Regs.ETPS.all = 0x01000;      // Event-Trigger Prescale Register
/* 
  bit 15-14     00  :   SOCBCNT     read only
  bit 13-12     00  :   SOCBPRD     Number of event that generates the pulse
  bit 11-10     00  :   SOCACNT     read only
  bit 9-8       00  :   SOCAPRD     Number of event that generates the pulse
  bit 7-4       0000:   reserved
  bit 3-2       00  :   INTCNT      read only
  bit 1-0       00  :   INTPRD      Number of event that generates the int
*/

  //EPwm2Regs.ETCLR.all = 0xFFFF;       // Clear all pending interupts
  //PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
  //IER |= 0x0004;
}


/*------------------------------------------------------------------------**
**                          InitEPwm3                                     **
**------------------------------------------------------------------------*/
void InitEPwm3(float frequency, float deadband_time, float phase)
{
  float T_period, T_deadband, T_phase = 0.0;
  int   DivClk;

  DivClk = 0;
  if (frequency != 0.0)
  {
    // corresponding period and deadband values
    T_period = (75e6/frequency);
    T_deadband = (deadband_time * 150);
  
    // check if the clock division is necessary
    while(T_period > 65535.0 || T_deadband > 1023.0)
    {
      T_period = T_period/2;
      T_deadband = T_deadband/2;
      DivClk++;
    }

    // corresponding phase value
    if (phase < 0.0)
      T_phase = -phase/180*T_period;
    else
      T_phase = phase/180*T_period;
  }


  EPwm3Regs.TBCTL.all = 0x0003;         // Disable TimeBase
  EPwm3Regs.TBPRD = (Uint16)T_period;   // Period Register
  EPwm3Regs.TBPHS.half.TBPHS =(Uint16)T_phase;  // Phase shift Register
  EPwm3Regs.TBCTR = 0x0000;             // Counter Register
  
  EPwm3Regs.TBCTL.all = 0xE006;     // Time Base Ctrl reg
/* 
  bit 15-14     00  :   FREE,SOFT   11 = Free run
  bit 13        0   :   PHSDIR      1 = Count up
  bit 12-10     000 :   CLKDIV      0 = x/1
  bit 9-7       000 :   HSPCLKDIV   0 = x/1
  bit 6         0   :   SWFSYNC     Write 1 will force sync.
  bit 5-4       00  :   SYNCOSEL    source of SyncOut signal (00:SyncIn, 01:CTR=0, 10:CTR=CMPRB, 11:Disable)
  bit 3         0   :   PRDLD       0 = TBPRD shadowed
  bit 2         1   :   PHSEN       1 = use the TBPHS reg when a SyncIn occurs
  bit 1-0       00  :   CTRMODE     00 = up, 01 = Down, 10=Up/Down, 11=Stop
*/
  EPwm3Regs.TBCTL.bit.CLKDIV = DivClk;
  EPwm3Regs.TBCTL.bit.PHSDIR = (phase >= 0.0);

  EPwm3Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD>>1;// Dutycycle = 0.5
  EPwm3Regs.CMPB = 0x0000;          // Compare reg B
 
  EPwm3Regs.CMPCTL.all = 0x0050;    // Compare Ctrl reg
/* 
  bit 15-10     0   :   reserved
  bit 9         0   :   SHDWBFULL   CMPB Full status bit
  bit 8         0   :   SHDWAFULL   CMPA Full status bit
  bit 7         0   :   reserved
  bit 6         0   :   SHDWBMODE   0 = CMPB shadowed
  bit 5         0   :   reserved
  bit 4         0   :   SHDWAMODE   0 = CMPA shadowed
  bit 3-2       00  :   LOADBMODE   Loading of CMPB mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
  bit 1-0       00  :   LOADAMODE   Loading of CMPA mode (00:CTR=0, 01:CTR=PRD, 10:both, 11:freeze)
*/

  EPwm3Regs.AQCTLA.all = 0x0060;    // Action qualifier output A
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       01  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       10  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxA output low
    * 10 = Set, force EPWMxA output high
    * 11 = Toggle EPWMxA
*/

  EPwm3Regs.AQCTLB.all = 0x0000;    // Action qualifier output B
/* 
  bit 15-12     0   :   reserved
  bit 11-10     00  :   CBD         Action when TBCNT = CMPB and TBCNT is counting down
  bit 9-8       00  :   CBU         Action when TBCNT = CMPB and TBCNT is counting up
  bit 7-6       00  :   CAD         Action when TBCNT = CMPA and TBCNT is counting down
  bit 5-4       00  :   CAU         Action when TBCNT = CMPA and TBCNT is counting up
  bit 3-2       00  :   PRD         Action when TBCNT = TBPRD
  bit 1-0       00  :   ZRE         Action when TBCNT = Zero
    * 00 = Do nothing, action disabled
    * 01 = Clear, force EPWMxB output low
    * 10 = Set, force EPWMxB output high
    * 11 = Toggle EPWMxB
*/

  EPwm3Regs.DBCTL.all = 0x000B;
  /*
    bit 15-6      0   :   reserved
    bit 5-4       00  :   INMODE      (00=EPWMxA is source for both falling and rising edge
                                       01=EPWMxA is source for falling edge, EPWMxB is source for rising edge,
                                       10=EPWMxA is source for rising edge, EPWMxB is source for fallingg edge,
                                       11=EPWMxB is source for both falling and rising edge)
    bit 3-2       10  :   POLSEL      (00=both active high, 01=active low compl. 10=active high comp. 11= both active low)
    bit 1-0       111  :   OUTMODE     (00=Bypassed, 01=Falling edge delay only, 10=rising edge delay, 11=both edge)
  */

  EPwm3Regs.DBRED = (Uint16)T_deadband; // Dead-Band rising edge delay reg
  EPwm3Regs.DBFED = (Uint16)T_deadband; // Dead-Band falling edge delay reg

  EPwm3Regs.ETSEL.all = 0x0900;     // Event-Trigger Selection Register
/* 
  bit 15        0   :   SOCBEN      1 = Enable ADC start of convertion B
  bit 14-12     000 :   SOCBSEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 11        0   :   SOCAEN      1 = Enable ADC start of convertion A
  bit 10-8      000 :   SOCASEL     001:TBCTR=0, 010:TBCTR=TBPRD, 100:CMPA up, 101:CMPA down, 110:CMPB up, 111:CMPB down
  bit 7-4       0   :   reserved
  bit 3         0   :   INTEN       1 = Enable EPWMx_INT
  bit 2-0       000 :   INTSEL      (001: TBCTR = 0, 010: TBCRT=TBPRD
                                     100: CMPA up, 101: CMPA down
                                     110: CMPB up, 111: CMPB down)
*/

  EPwm3Regs.ETPS.all = 0x0100;      // Event-Trigger Prescale Register
/* 
  bit 15-14     00  :   SOCBCNT     read only
  bit 13-12     00  :   SOCBPRD     Number of event that generates the pulse
  bit 11-10     00  :   SOCACNT     read only
  bit 9-8       00  :   SOCAPRD     Number of event that generates the pulse
  bit 7-4       0000:   reserved
  bit 3-2       00  :   INTCNT      read only
  bit 1-0       00  :   INTPRD      Number of event that generates the int
*/

  //EPwm3Regs.ETCLR.all = 0xFFFF;       // Clear all pending interupts
  //PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
  //IER |= 0x0004;
}

// END OF FILE
