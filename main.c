/*ˮ�ι��źŷ���
 * ����Ƶ�ʣ�190kHZ
 * �źŷ��ȣ�����2v*/

#include "DSP2833x_Project.h"
#include "stdlib.h"

#define Uint8 unsigned char
#define slaveAddr 0xFF //�ӻ���ַ

//ad��غ���
interrupt void adc_isr(void);
float getThrehold(float V_IN); //��ֵ����

//��ʱ��0�ж�
interrupt void timer_isr(void);

//ecap1�жϺ���
interrupt void ecap_isr();

//ad����
#define bufferSize 200 //��������С
#define filteringValue 90 //�˲�ֵ
Uint32 wPeriod, averageTime;
Uint16 conversionCount;
float voltage1[bufferSize]; //ת��1
float voltage2[bufferSize]; //ת��2
float threhold; //��ֵ
Uint16 threholdCount; //��ֵ����

float cmpValue; //��ֵ���ٻ�׼ֵ
int k,i;
float sumCmpValue;
float V1; //ÿ��600������ȡ��
float V2;

//i2c����

//ecap



int main(void)
{


    //��ʼ��ϵͳ
    InitSysCtrl();
    DINT;
    //��ʼ�������жϹ���
    InitPieCtrl();
    InitPieVectTable();
    IER = 0x0000;
    IFR = 0x0000;

    EALLOW;
    PieVectTable.TINT0=&timer_isr;
    PieVectTable.ADCINT=&adc_isr;
    PieVectTable.ECAP2_INT=&ecap_isr;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx6=1;
    PieCtrlRegs.PIEIER1.bit.INTx7=1;
    PieCtrlRegs.PIEIER4.bit.INTx2=1;

    IER |= M_INT1;
    IER |= M_INT4;
    EINT;
    ERTM;
    //������ʼ��
    conversionCount = 0;
    k=0;
    i=0;
    sumCmpValue=0;

    //��ʼ�����ģ��
    InitCpuTimers();
    InitGpio();
    InitAdc();
    InitECap();
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
//    if(doGetThrehold)
    threhold=getThrehold(voltage1[conversionCount]);

    //��ֵ�Ƚϣ������������
    if(voltage1[conversionCount]>threhold){
        threholdCount++;
    }else{
        threholdCount=0;

    }
    if(threholdCount>5){
        GpioDataRegs.GPASET.bit.GPIO0=1;
//        GpioDataRegs.GPCSET.bit.GPIO79=1;
//        doGetThrehold=0;
        //��ȡˮ���ź����
    }else if(threholdCount==0){
        GpioDataRegs.GPACLEAR.bit.GPIO0=1;
//        GpioDataRegs.GPCCLEAR.bit.GPIO79=1;
//        doGetThrehold=1;
        //��ȡˮ���ź��յ�
    }

    //��ջ�����
    if(conversionCount==bufferSize){
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


float getThrehold(float V_IN){

    //ÿ��һ���������ڸ���һ�±Ƚϻ�׼ֵ�����ٵ�ѹ
    if(conversionCount==0){
        //V1��V2��ÿ��600��������ȡֵ����Ϊ����ͬʱȡ���������С
        //���V1��V2�ڸ���ôԶȡֵ������˵��������ǵ͵�ƽ
        //��V1��V2���͵��Ǹ�����Ϊ��ǰ��׼ֵ
        if(abs(V1-V2)<50)
        cmpValue=V1<V2? V1:V2;
        sumCmpValue=0;
        i=0;
    }

    sumCmpValue+=V_IN<cmpValue? V_IN:cmpValue; //ȡƽ��ֵ����������
    i++;

    return (sumCmpValue/i)+filteringValue;
}


void ecap_isr(){

    wPeriod+=ECap2Regs.CAP3-ECap2Regs.CAP1;
    PieCtrlRegs.PIEACK.bit.ACK4 = 1; //�������Ѿ���Ӧ�ж�
    ECap2Regs.ECCLR.all = 0xFFFF; //�������ж����
}
