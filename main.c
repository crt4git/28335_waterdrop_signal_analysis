/*水滴光信号分析
 * 采样频率：190kHZ
 * 信号幅度：建议2v*/

#include "DSP2833x_Project.h"
#include "stdlib.h"

#define Uint8 unsigned char
#define slaveAddr 0xFF //从机地址

//ad相关函数
interrupt void adc_isr(void);
float getThrehold(float V_IN); //阈值跟踪

//定时器0中断
interrupt void timer_isr(void);

//ecap1中断函数
interrupt void ecap_isr();

//ad变量
#define bufferSize 200 //缓存区大小
#define filteringValue 90 //滤波值
Uint32 wPeriod, averageTime;
Uint16 conversionCount;
float voltage1[bufferSize]; //转换1
float voltage2[bufferSize]; //转换2
float threhold; //阈值
Uint16 threholdCount; //阈值消抖

float cmpValue; //阈值跟踪基准值
int k,i;
float sumCmpValue;
float V1; //每隔600采样点取样
float V2;

//i2c变量

//ecap



int main(void)
{


    //初始化系统
    InitSysCtrl();
    DINT;
    //初始化外设中断管理
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
    //变量初始化
    conversionCount = 0;
    k=0;
    i=0;
    sumCmpValue=0;

    //初始化相关模块
    InitCpuTimers();
    InitGpio();
    InitAdc();
    InitECap();
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


void adc_isr(void)
{
    voltage1[conversionCount] = AdcRegs.ADCRESULT0 >>4;
    voltage2[conversionCount] = AdcRegs.ADCRESULT1 >>4;

    //每隔3000个采样点取值
    if(k==3){
      V1=voltage1[conversionCount];
    }else if(k==6){
      V2=voltage1[conversionCount];
      k=0;
    }
    //计算最小值，得到阈值
//    if(doGetThrehold)
    threhold=getThrehold(voltage1[conversionCount]);

    //阈值比较，过滤输出脉冲
    if(voltage1[conversionCount]>threhold){
        threholdCount++;
    }else{
        threholdCount=0;

    }
    if(threholdCount>5){
        GpioDataRegs.GPASET.bit.GPIO0=1;
//        GpioDataRegs.GPCSET.bit.GPIO79=1;
//        doGetThrehold=0;
        //获取水滴信号起点
    }else if(threholdCount==0){
        GpioDataRegs.GPACLEAR.bit.GPIO0=1;
//        GpioDataRegs.GPCCLEAR.bit.GPIO79=1;
//        doGetThrehold=1;
        //获取水滴信号终点
    }

    //清空缓存区
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

    //AD开始转换信号
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;

    PieCtrlRegs.PIEACK.all=PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;
}


float getThrehold(float V_IN){

    //每隔一个缓存周期更新一下比较基准值，跟踪电压
    if(conversionCount==0){
        //V1和V2是每隔600个采样点取值，因为这样同时取到波峰概率小
        //如果V1和V2在隔这么远取值还相差不大，说明大概率是低电平
        //把V1和V2更低的那个更新为当前基准值
        if(abs(V1-V2)<50)
        cmpValue=V1<V2? V1:V2;
        sumCmpValue=0;
        i=0;
    }

    sumCmpValue+=V_IN<cmpValue? V_IN:cmpValue; //取平均值，过滤骤变点
    i++;

    return (sumCmpValue/i)+filteringValue;
}


void ecap_isr(){

    wPeriod+=ECap2Regs.CAP3-ECap2Regs.CAP1;
    PieCtrlRegs.PIEACK.bit.ACK4 = 1; //第四组已经响应中断
    ECap2Regs.ECCLR.all = 0xFFFF; //将所有中断清除
}
