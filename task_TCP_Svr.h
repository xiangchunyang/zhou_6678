/*
 * task_TCP_recvSvr.h
 *
 *  Created on: 2016-8-17
 *      Author: Administrator
 */

#ifndef TASK_TCP_RECVSVR_H_
#define TASK_TCP_RECVSVR_H_

#define PACKSIZE 1024//Bytes。上传至PC机的数据的分包大小
#define PACKSIZEFROMPC 1036//Bytes。PC机的下载参数时的分包大小
#define PARASPACKSIZETOPC PACKSIZEFROMPC//Bytes。PC机的下载参数时的分包大小
/**--------PC机下行的参数数据 有关常量------------------*/
#define LENGTH_DW_MAINCTRL        0x0400    //hxl:纯参数长度，单位：Double word
#define LENGTH_DW_TRANSMITDELY    0x0800
#define LENGTH_DW_RECVDELY        0x2000
#define LENGTH_DW_TRANSMITMUX     0x0400
#define LENGTH_DW_RECVMUX         0x0400
#define LENGTH_DW_TGC             0x0400
#define MAXLEN_PERWRPURE_DW         128//0x2000 //hxl:PC机单次发送的参数数据的最长的长度(不包括命令字，纯参数)
#define CMDLEN_PERWR_W             4//0x2000 //hxl:PC机单次发送的 命令字的长度，单位word
#define MAXLENPERWR_W              128*2+4
#define BYTESPERWORD              4
#define BYTESPERDW            8
#define L_DW_PCREADPARAS          512//hxl:根据FPGA传来的实际参数长度确定;double word
//#define HEAD_PCREADPARAS          0xFFFFFFFF//hxl:PC要求读取参数时，DSP上传数据需要添加的头。
#define HEAD0_PCREADPARAS0          0xaaaa5555//hxl:PC要求读取参数时，DSP上传数据需要添加的头。hxl:2017-09-25，重新修改格式
#define HEAD0_PCWRTPARAS0           0x5555aaaa
#define HEAD0_PCWRTPARAS1           0x55553333
#define HEAD0_PCREADPARAS1          0x33335555
#define SIZE_PARASARRAY           30         //hxl:需要传递给SRIO的参数的缓存的大小。
#define TIME_A_FRAME              2000000     //hxl:FPGA 发送一帧图像的时间
/*------------------定义一个枚举类型用于解析PC传来的指令的类型----------------------------------*/
typedef enum
{
	DOWNPARAS=1,//下载参数 ,hxl:PC机要求下载参数至DSP。
	READPARAS=2,//读取参数,hxl:PC机要求从DSP读取参数。
	UPDATA=3,//上传回波数据,hxl:PC机要求DSP上传回波数据。
	STOP=4//中断传输，hxl:PC机要求DSP中断传输

}CMDTYPE;
/*------------------定义一个结构体类型用于接收PC传来的参数----------------------------------*/

typedef struct
{

	unsigned int headForGuide;//hxl:注意，必须占32 bit，以保证 parasarray位于8 Bytes 对齐的存储位置上
	CMDTYPE cmd_type;//hxl:32bit
    unsigned int addr;//hxl:用于表征参数写入FPGA端的地址，32bit
    unsigned int length_DW;//hxl:用于表征参数长度,单位double Word.32bit
    Uint8 parasarray[MAXLEN_PERWRPURE_DW*8];//hxl:所有参数类型中参数最长的长度.保证align 8条件


}PARASBUF0;
typedef struct
{
	CMDTYPE cmd_type;
    unsigned int addr;//hxl:用于表征参数写入FPGA端的地址，32bit
    unsigned int length_DW;//hxl:用于表征参数长度,单位double Word
    Uint8 (*ptr_parasarray);//hxl:指向参数数据的指针


}PARASBUF1;
typedef struct
{
	CMDTYPE cmd_type;
    unsigned int addr;//hxl:用于表征参数写入FPGA端的地址，32bit
    unsigned int length_DW;//hxl:用于表征参数长度,单位double Word
    Uint8 parasarray[BYTESPERDW];//hxl:指向


}COMMANDBUF2;



extern void task_TCP_Svr();
extern PARASBUF0 paras_toFPGA[SIZE_PARASARRAY];
extern int counts_svr;
#endif /* TASK_TCP_RECVSVR_H_ */
