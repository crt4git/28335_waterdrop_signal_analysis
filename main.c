/*ˮ�ι��źŷ���
 * ����Ƶ�ʣ�190kHZ
 * �źŷ��ȣ�����2v*/

#include "DSP2833x_Project.h"
#include "stdlib.h"

//�ڲ�����
interrupt void adc_isr(void);
interrupt void timer_isr(void);
float32 getThrehold(float32 V_IN);
//��ر���������
#define bufferSize 1000
#define filteringValue 90
Uint16 conversionCount;
float32 voltage1[bufferSize];
float32 V1;
float32 V2;
float32 voltage2[bufferSize];
float32 threhold;
Uint16 threholdCount;
float32 cmpValue;
//ֵ����������
int k,i;
float32 sum;



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
    conversionCount = 0;
    k=0;
    i=0;
    sum=0;
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


void adc_isr(void)
{
    voltage1[conversionCount] = AdcRegs.ADCRESULT0 >>4;
    voltage2[conversionCount] = AdcRegs.ADCRESULT1 >>4;

    //ÿ��3000��������ȡֵ
    if(k==3){
      V1=voltage1[conversionCount];
    }else if(k==6){
      V2=voltage1[conversionCount];
      k=0;
    }
    //������Сֵ���õ���ֵ
    threhold=getThrehold(voltage1[conversionCount]);

    //��ֵ�Ƚϣ������������
    if(voltage1[conversionCount]>threhold){
        threholdCount++;
    }else{
        threholdCount=0;
    }
    if(threholdCount>5){
        GpioDataRegs.GPASET.bit.GPIO0=1;
        //��ȡˮ���ź����
    }else if(threholdCount==0){
        GpioDataRegs.GPACLEAR.bit.GPIO0=1;

    }

    //��ջ�����
    if(conversionCount == bufferSize){
        conversionCount = 0;
        k++;
    }
    else{
        conversionCount++;
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


float32 getThrehold(float32 V_IN){

    //ÿ��һ���������ڸ���һ�±Ƚϻ�׼ֵ�����ٵ�ѹ
    if(conversionCount==0){
        //V1��V2��ÿ��3000��������ȡֵ����Ϊ����ͬʱȡ���������С
        //���V1��V2�ڸ���ôԶȡֵ������˵��������ǵ͵�ƽ
        //��V1��V2���͵��Ǹ�����Ϊ��ǰ��׼ֵ
        if(abs(V1-V2)<50)
        cmpValue=V1<V2? V1:V2;
        sum=0;
        i=0;
    }
    sum+=V_IN<cmpValue? V_IN:cmpValue;//�Ƚϵ�ǰ��ѹ�ͻ�׼ֵ
    i++;

    return (sum/i)+filteringValue;

}


