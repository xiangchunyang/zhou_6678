
#ifndef DEBUG
#define DEBUG //hxl:用于控制调试过程的一些打印消息
#undef DEBUG  //release version need this line
/*******用于与FPGA的不同版本程序的兼容*********************/
//#define FPGAPROGAM_V2BELOW     //hxl:V1,V2
//#define FPGAPROGAM_V3     //hxl:V3,V4,v5
//#define FPGAPROGAM_V4_1
#define FPGAPROGAM_V4_2

#ifdef FPGAPROGAM_V4_1
#define FPGAPROGAM_V4ABOVE 1
#endif
#ifdef FPGAPROGAM_V4_2
#define FPGAPROGAM_V4ABOVE 1
#endif

/*******定义FPGA的RAM地址*********************************************************/
#ifdef FPGAPROGAM_V2BELOW
/*------用于NORMAL_MODE and SENDDATAANDREADBACK---------------*/
//hxl:版本2不支持DSP端通过写FPGA端特定地址启动数据传输和doorbell
/*-------用于NORMAL_MODE and RECVDATAANDDBL-------------------*/
#define DSTADDR_FPGA_RAM 0x00800000 //hxl:FPGA 端程序版本：v2-2，V2-3，v2-4，v2-5
#endif

#ifdef FPGAPROGAM_V3
#define FPGAPROGAM_V3ABOVE 1
/*-------用于NORMAL_MODE and RECVDATAANDDBL------------------*/
#define DSTADDR_FPGA_RAM 0x50000000 //hxl:FPGA 端程序版本：v3

#endif

#ifdef FPGAPROGAM_V4ABOVE
/*-------用于NORMAL_MODE and RECVDATAANDDBL-----------------*/
#define DSTADDR_START 0x50040000	   //hxl:FPGA 端启动数据线传输功能对应的RAM地址，1DW ； FPGA 端程序版本：v4-1
#define SHADOWDSTADDR_START 0x50040080 //hxl:FPGA 端启动数据线传输功能对应的RAM地址的影子寄存器，1DW；：FPGA 端程序版本：v4-1
/*-------用于NORMAL_MODE and SENDDATAANDREADBACK--------------*/
#define DSTADDR_FPGA_RAM 0x50000000 //hxl:FPGA 端程序版本：v3

#endif
#define LEN_STARTCOMMAND 1 //hxl:DSP 启动FPGA上传数据的指令字长度，1 DW
/*******定义一些控制参数及常量*********************************************************/

#endif
