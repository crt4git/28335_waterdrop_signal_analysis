#include "DSP2833x_Project.h"
#include "stdlib.h"

//内部函数
interrupt void adc_isr(void);
interrupt void timer_isr(void);
Uint16 getThrehold(float32 V_IN);
//相关变量、参数
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
    //初始化
    InitSysCtrl();
    DINT;
    //初始化外设中断管理
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
    //变量初始化
    ConversionCount = 0;
    k=0;
    //初始化相关模块
    InitCpuTimers();
    InitGpio();
    InitAdc();

    //AD配置（ 详细配置在DSP2833x_Adc.c的InitAdc() ）
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
    //定时器0配置
    ConfigCpuTimer(&CpuTimer0, 150, 5);
    StartCpuTimer0();

    while(1){

    }

}


void  adc_isr(void)
{
    Voltage1[ConversionCount] = AdcRegs.ADCRESULT0 >>4;
    Voltage2[ConversionCount] = AdcRegs.ADCRESULT1 >>4;

    //每隔3000个采样点取值
    if(k==3){
      V1=Voltage1[ConversionCount];
    }else if(k==6){
      V2=Voltage1[ConversionCount];
      k=0;
    }
    //计算最小值，得到阈值
    threhold=getThrehold(Voltage1[ConversionCount]);

    //阈值比较，过滤输出脉冲
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

    //清空缓存区
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

    //AD开始转换信号
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;

    PieCtrlRegs.PIEACK.all=PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;
}


Uint16 getThrehold(float32 V_IN)
{
    //j和sum为了取阈值平均值，减小阈值波动范围
    float32 threholdOut=0;
    //每隔一个缓存周期更新一下比较基准值，跟踪电压
    if(ConversionCount==0){
        //V1和V2是每隔3000个采样点取值，因为这样同时取到波峰概率小
        //如果V1和V2在隔这么远取值还相差不大，说明大概率是低电平
        //把V1和V2更低的那个更新为当前基准值
        if(abs(V1-V2)<50)
        cmpValue=V1<V2? V1:V2;
    }
    threholdOut=V_IN<cmpValue? V_IN:cmpValue;//比较当前电压和基准值

    return threholdOut+filteringValue;

}


