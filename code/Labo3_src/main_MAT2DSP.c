#include "Mat2DSP_sci.h"
#include "Mat2DSP_config.h"
#include "Mat2DSP_crc.h"
#include "Mat2DSP_RT_scope.h"
#include "Mat2DSP_types.h"

void main(void)
{
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

}