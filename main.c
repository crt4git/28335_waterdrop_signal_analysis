#include "DSP2833x_Project.h"
#include "stdlib.h"

//内部
interrupt void adc_isr(void);
interrupt void timer_isr(void);
int min(int a, int b);
Uint16 getThrehold(float32 V1_IN, float32 V2_IN);

//
#define bufferSize 1000

Uint16 ConversionCount;
float32 Voltage1[bufferSize];
float32 V1;
float32 V2;
float32 Voltage2[bufferSize];
float32 temp;
float32 tempArr[bufferSize];
Uint16 threholdCount;
float32 threhold;
int flagADC;

int main(void)
{
   //初始化
   InitSysCtrl();

   InitCpuTimers();

   EALLOW;
   #if (CPU_FRQ_150MHZ)
     #define ADC_MODCLK 0x3
   #endif
   #if (CPU_FRQ_100MHZ)
     #define ADC_MODCLK 0x2
   #endif
   EDIS;

   InitGpio();

   EALLOW;

   GpioCtrlRegs.GPAMUX1.bit.GPIO0=0;
   GpioCtrlRegs.GPAMUX1.bit.GPIO1=0;
   GpioCtrlRegs.GPADIR.all=0x0003;
   EDIS;


   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;
   EDIS;

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.TINT0=&timer_isr;
   PieVectTable.ADCINT = &adc_isr;
   EDIS;

   InitAdc();

   PieCtrlRegs.PIEIER1.bit.INTx6=1;
   PieCtrlRegs.PIEIER1.bit.INTx7=1;
   IER |= M_INT1;
   EINT;
   ERTM;
   ConfigCpuTimer(&CpuTimer0, 150, 4);
   StartCpuTimer0();


   ConversionCount = 0;


//   AdcRegs.ADCTRL1.bit.SEQ_CASC=1;
//   AdcRegs.ADCTRL1.bit.ACQ_PS=1;
//   AdcRegs.ADCMAXCONV.all = 0x0000;
//   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
//
//
//   AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0;
//   AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
//   AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1=0;
//
//   AdcRegs.ADCTRL3.bit.ADCCLKPS=0x0000;
//
//   AdcRegs.ADCTRL3.bit.SMODE_SEL=1;
//   EPwm1Regs.TBCTL.bit.CLKDIV=0;
//   EPwm1Regs.TBCTL.bit.HSPCLKDIV=0;
//   EPwm1Regs.ETSEL.bit.SOCAEN = 1;
//   EPwm1Regs.ETSEL.bit.SOCASEL = 4;
//   EPwm1Regs.ETPS.bit.SOCAPRD = 1;
//   EPwm1Regs.CMPA.half.CMPA = 0x0080;
//   EPwm1Regs.TBPRD = 300;
//   EPwm1Regs.TBCTL.bit.CTRMODE = 0;




   for(;;)
   {

   }

}


void  adc_isr(void)
{
  Voltage1[ConversionCount] = AdcRegs.ADCRESULT0 >>4;
  Voltage2[ConversionCount] = AdcRegs.ADCRESULT1 >>4;

  if(Voltage1[ConversionCount]>temp){
//     threholdCount++;
     GpioDataRegs.GPASET.bit.GPIO0=1;
  }else{
//     threholdCount=0;
     GpioDataRegs.GPACLEAR.bit.GPIO0=1;
  }
//  if(threholdCount>5){
//
//  }else if(threholdCount==0){
//
//  }



  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}

void timer_isr(){

    if(ConversionCount == bufferSize)
    {
     ConversionCount = 0;
    }
    else{
      ConversionCount++;
    }
    if(flagADC==0){
        V1=Voltage1[ConversionCount];
        flagADC=1;
    }else{
        V2=Voltage1[ConversionCount];
        flagADC=0;
    }

    temp=getThrehold(V1, V2);
    tempArr[ConversionCount]=temp;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;

    PieCtrlRegs.PIEACK.all=PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

Uint16 getThrehold(float32 V1_IN, float32 V2_IN)
{
    float32 sum=0;
    int j=0;
    if(ConversionCount==0){
        threhold=V1_IN;
        sum=0;
        j=0;
    }
    sum+=threhold<min(V1_IN, V2_IN)? threhold:min(V1_IN, V2_IN);
    j++;

    return sum/j+130;

}


