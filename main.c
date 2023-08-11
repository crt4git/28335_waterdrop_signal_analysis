#include "DSP2833x_Project.h"
#include "stdlib.h"

//�ڲ�����
interrupt void adc_isr(void);
interrupt void timer_isr(void);
Uint16 getThrehold(float32 V_IN);
//��ر���������
#define bufferSize 1000
#define filteringValue 90
Uint16 ConversionCount;
float32 Voltage1[bufferSize];
float32 V1;
float32 V2;
float32 Voltage2[bufferSize];
float32 threhold;
Uint16 threholdCount;
float32 cmpValue;
int k;

int main(void)
{
    //��ʼ��
    InitSysCtrl();
    DINT;
    //��ʼ�������жϹ���
    InitPieCtrl();
    InitPieVectTable();
    IER = 0x0000;
    IFR = 0x0000;

    EALLOW;
    PieVectTable.TINT0=&timer_isr;
    PieVectTable.ADCINT = &adc_isr;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx6=1;
    PieCtrlRegs.PIEIER1.bit.INTx7=1;

    IER |= M_INT1;
    EINT;
    ERTM;
    //������ʼ��
    ConversionCount = 0;
    k=0;
    //��ʼ�����ģ��
    InitCpuTimers();
    InitGpio();
    InitAdc();

    //AD���ã� ��ϸ������DSP2833x_Adc.c��InitAdc() ��
    EALLOW;
    #if (CPU_FRQ_150MHZ)
     #define ADC_MODCLK 0x3
    #endif
    #if (CPU_FRQ_100MHZ)
     #define ADC_MODCLK 0x2
    #endif
    EDIS;

    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK;
    EDIS;
    //��ʱ��0����
    ConfigCpuTimer(&CpuTimer0, 150, 5);
    StartCpuTimer0();

    while(1){

    }

}


void  adc_isr(void)
{
    Voltage1[ConversionCount] = AdcRegs.ADCRESULT0 >>4;
    Voltage2[ConversionCount] = AdcRegs.ADCRESULT1 >>4;

    //ÿ��3000��������ȡֵ
    if(k==3){
      V1=Voltage1[ConversionCount];
    }else if(k==6){
      V2=Voltage1[ConversionCount];
      k=0;
    }
    //������Сֵ���õ���ֵ
    threhold=getThrehold(Voltage1[ConversionCount]);

    //��ֵ�Ƚϣ������������
    if(Voltage1[ConversionCount]>threhold){
        threholdCount++;
    }else{
        threholdCount=0;
    }
    if(threholdCount>5){
        GpioDataRegs.GPASET.bit.GPIO0=1;
    }else if(threholdCount==0){
        GpioDataRegs.GPACLEAR.bit.GPIO0=1;
    }

    //��ջ�����
    if(ConversionCount == bufferSize){
        ConversionCount = 0;
        k++;
    }
    else{
        ConversionCount++;
    }


    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void timer_isr(){

    //AD��ʼת���ź�
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;

    PieCtrlRegs.PIEACK.all=PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;
}


Uint16 getThrehold(float32 V_IN)
{
    //j��sumΪ��ȡ��ֵƽ��ֵ����С��ֵ������Χ
    float32 threholdOut=0;
    //ÿ��һ���������ڸ���һ�±Ƚϻ�׼ֵ�����ٵ�ѹ
    if(ConversionCount==0){
        //V1��V2��ÿ��3000��������ȡֵ����Ϊ����ͬʱȡ���������С
        //���V1��V2�ڸ���ôԶȡֵ������˵��������ǵ͵�ƽ
        //��V1��V2���͵��Ǹ�����Ϊ��ǰ��׼ֵ
        if(abs(V1-V2)<50)
        cmpValue=V1<V2? V1:V2;
    }
    threholdOut=V_IN<cmpValue? V_IN:cmpValue;//�Ƚϵ�ǰ��ѹ�ͻ�׼ֵ

    return threholdOut+filteringValue;

}


