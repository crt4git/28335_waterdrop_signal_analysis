#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#define PWM_CLK   10e3                // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT

#define UP_DOWN   2
#define UP        0
#define DOWN      1

#define CDIV_NO    0
#define CDIV_2    1
#define CDIV_4    2
#define CDIV_8    3
#define CDIV_16   4

#define HDIV_NO    0
#define HDIV_2    1

#define HDIV_4    2
#define HDIV_6    3
#define HDIV_8    4
#define HDIV_10   5

#define T_1US   75
#define T_1MS   7500


void EPwmSetup()
{
    InitEPwm1Gpio();
   // InitEPwm();
//  EPwm1Regs.TBSTS.all=0;
    EPwm1Regs.TBPHS.half.TBPHS=0;        //������λ����
    EPwm1Regs.TBCTR=0;                   //ʱ������������

    EPwm1Regs.TBPRD = 1500;                //TBPRD 65536
    EPwm1Regs.CMPA.half.CMPA = 37;
    EPwm1Regs.CMPB = 30;


    EPwm1Regs.TBCTL.bit.CTRMODE = UP_DOWN;         //������
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;       //��ֹ��λ����
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;  // ����Ӱ�ӼĴ���ģʽ
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //�ر�ͬ���ź�
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = HDIV_NO;
    EPwm1Regs.TBCTL.bit.CLKDIV = CDIV_NO;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;   // CTR = 0  epwm A ��1
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR; // CTR = CMPA epwm A ��0

    EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;
    EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;


//  EPwm1Regs.AQCTLB.all=0;
//  EPwm1Regs.AQSFRC.all=0;
//  EPwm1Regs.AQCSFRC.all=0;

    EPwm1Regs.DBCTL.all=0xb;          // EPWMxB is inverted
    EPwm1Regs.DBRED=0;
    EPwm1Regs.DBFED=0;

    EPwm1Regs.TZSEL.all=0;
    EPwm1Regs.TZCTL.all=0;
    EPwm1Regs.TZEINT.all=0;
    EPwm1Regs.TZFLG.all=0;
    EPwm1Regs.TZCLR.all=0;
    EPwm1Regs.TZFRC.all=0;

    EPwm1Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
    EPwm1Regs.ETFLG.all=0;
    EPwm1Regs.ETCLR.all=0;
    EPwm1Regs.ETFRC.all=0;

    EPwm1Regs.PCCTL.all=0;

        EPwm1Regs.ETSEL.bit.INTEN =1;  //���¼������ж�
        EPwm1Regs.ETSEL.bit.INTSEL=1;  //һ���¼�����

        EPwm1Regs.ETSEL.bit.SOCAEN =1; //ʹ��epwmxSOCA�źŲ���
        EPwm1Regs.ETSEL.bit.SOCASEL=4; //���ϼ��� TBCTR=CMPA����epwmxSOCA�ź�

        EPwm1Regs.ETSEL.bit.SOCBEN =1; //ʹ��epwmxSOCB�źŲ���
        EPwm1Regs.ETSEL.bit.SOCBSEL=4; //���ϼ��� TBCTR=CMPB����epwmxSOCB�ź�


        EPwm1Regs.ETPS.bit.INTPRD =1;  //��һ���¼�ʱ����SOC�ź�
        EPwm1Regs.ETPS.bit.SOCAPRD=1;
        EPwm1Regs.ETPS.bit.SOCBPRD=1;

        EPwm1Regs.ETCLR.bit.INT=1;


}

