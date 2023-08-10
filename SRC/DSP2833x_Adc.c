#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_usDELAY  5000L

//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);

    AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //级联排序器
    AdcRegs.ADCTRL1.bit.ACQ_PS=1; //采样窗口为2个ADC时钟
    AdcRegs.ADCTRL1.bit.SUSMOD=1;

    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;

    AdcRegs.ADCMAXCONV.all = 0x0000; //最大转换1，同时转换就是1+1
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; //A0和B0


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    ADC_cal();
    EDIS;




    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
    // after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
    // CPU_RATE define statement in the DSP2833x_Examples.h file must
    // contain the correct CPU clock period in nanoseconds.

    AdcRegs.ADCTRL3.all = 0x00E1;  // Power up bandgap/reference/ADC circuits
    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels
}

//===========================================================================
// End of file.
//===========================================================================
