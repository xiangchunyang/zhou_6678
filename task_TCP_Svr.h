/*
 * task_TCP_recvSvr.h
 *
 *  Created on: 2016-8-17
 *      Author: Administrator
 */

#ifndef TASK_TCP_RECVSVR_H_
#define TASK_TCP_RECVSVR_H_

#define PACKSIZE 1024//Bytes���ϴ���PC�������ݵķְ���С
#define PACKSIZEFROMPC 1036//Bytes��PC�������ز���ʱ�ķְ���С
#define PARASPACKSIZETOPC PACKSIZEFROMPC//Bytes��PC�������ز���ʱ�ķְ���С
/**--------PC�����еĲ������� �йس���------------------*/
#define LENGTH_DW_MAINCTRL        0x0400    //hxl:���������ȣ���λ��Double word
#define LENGTH_DW_TRANSMITDELY    0x0800
#define LENGTH_DW_RECVDELY        0x2000
#define LENGTH_DW_TRANSMITMUX     0x0400
#define LENGTH_DW_RECVMUX         0x0400
#define LENGTH_DW_TGC             0x0400
#define MAXLEN_PERWRPURE_DW         128//0x2000 //hxl:PC�����η��͵Ĳ������ݵ���ĳ���(�����������֣�������)
#define CMDLEN_PERWR_W             4//0x2000 //hxl:PC�����η��͵� �����ֵĳ��ȣ���λword
#define MAXLENPERWR_W              128*2+4
#define BYTESPERWORD              4
#define BYTESPERDW            8
#define L_DW_PCREADPARAS          512//hxl:����FPGA������ʵ�ʲ�������ȷ��;double word
//#define HEAD_PCREADPARAS          0xFFFFFFFF//hxl:PCҪ���ȡ����ʱ��DSP�ϴ�������Ҫ��ӵ�ͷ��
#define HEAD0_PCREADPARAS0          0xaaaa5555//hxl:PCҪ���ȡ����ʱ��DSP�ϴ�������Ҫ��ӵ�ͷ��hxl:2017-09-25�������޸ĸ�ʽ
#define HEAD0_PCWRTPARAS0           0x5555aaaa
#define HEAD0_PCWRTPARAS1           0x55553333
#define HEAD0_PCREADPARAS1          0x33335555
#define SIZE_PARASARRAY           30         //hxl:��Ҫ���ݸ�SRIO�Ĳ����Ļ���Ĵ�С��
#define TIME_A_FRAME              2000000     //hxl:FPGA ����һ֡ͼ���ʱ��
/*------------------����һ��ö���������ڽ���PC������ָ�������----------------------------------*/
typedef enum
{
	DOWNPARAS=1,//���ز��� ,hxl:PC��Ҫ�����ز�����DSP��
	READPARAS=2,//��ȡ����,hxl:PC��Ҫ���DSP��ȡ������
	UPDATA=3,//�ϴ��ز�����,hxl:PC��Ҫ��DSP�ϴ��ز����ݡ�
	STOP=4//�жϴ��䣬hxl:PC��Ҫ��DSP�жϴ���

}CMDTYPE;
/*------------------����һ���ṹ���������ڽ���PC�����Ĳ���----------------------------------*/

typedef struct
{

	unsigned int headForGuide;//hxl:ע�⣬����ռ32 bit���Ա�֤ parasarrayλ��8 Bytes ����Ĵ洢λ����
	CMDTYPE cmd_type;//hxl:32bit
    unsigned int addr;//hxl:���ڱ�������д��FPGA�˵ĵ�ַ��32bit
    unsigned int length_DW;//hxl:���ڱ�����������,��λdouble Word.32bit
    Uint8 parasarray[MAXLEN_PERWRPURE_DW*8];//hxl:���в��������в�����ĳ���.��֤align 8����


}PARASBUF0;
typedef struct
{
	CMDTYPE cmd_type;
    unsigned int addr;//hxl:���ڱ�������д��FPGA�˵ĵ�ַ��32bit
    unsigned int length_DW;//hxl:���ڱ�����������,��λdouble Word
    Uint8 (*ptr_parasarray);//hxl:ָ��������ݵ�ָ��


}PARASBUF1;
typedef struct
{
	CMDTYPE cmd_type;
    unsigned int addr;//hxl:���ڱ�������д��FPGA�˵ĵ�ַ��32bit
    unsigned int length_DW;//hxl:���ڱ�����������,��λdouble Word
    Uint8 parasarray[BYTESPERDW];//hxl:ָ��


}COMMANDBUF2;



extern void task_TCP_Svr();
extern PARASBUF0 paras_toFPGA[SIZE_PARASARRAY];
extern int counts_svr;
#endif /* TASK_TCP_RECVSVR_H_ */
