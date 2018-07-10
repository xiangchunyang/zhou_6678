/*
 * task_TCP_R.c
 *
 *  Created on: 2015-9-21
 *      Author: Administrator
 */
#include <ti/csl/csl_chip.h>	 //hxl:added for stamp counter use
#include <ti/ndk/inc/netmain.h>  //包含相对路径，在库目录中搜索 hxl added
#include <ti/sysbios/BIOS.h>	 //for 参数：BIOS_WAIT_FOREVER
#include <ti/sysbios/knl/Task.h> //hxl added
//#include <ti/sysbios/knl/Semaphore.h>// hxl added
#include <xdc/runtime/System.h>			//hxl added
#include <ti/sysbios/family/c64p/Hwi.h> //hxl:2016-7-26 added
/* CSL Cache Functional Layer */
#include <ti/csl/csl_cacheAux.h> //hxl added for calling CACHE_wbL1d()  2017-6-21
#include <ti/platform/platform.h>
#include "App_common.h"
#include "SRIO_loopbackDioIsr.h" //hxl added
#include "processTask.h"		 ////2016-8-17:hxl: added
#include "task_TCP_Svr.h"
#include "SRIO_TransmitToFPGA.h"
#include "test.h"
#include "idleFunctions_appUser.h" //hxl:2017-6-16 added
#include "device_srio_loopback.h"  //hxl:2017-6-16 added
/*
#pragma DATA_SECTION(dataToSend, ".sharedVar1")//将数据分配到ddr3 SRAM中
#pragma DATA_ALIGN (dataToSend, 8)
short dataToSend[(TESTSIZE)/4]={0};//hxl added*/

#pragma DATA_SECTION(data_toPC, ".sharedVar") //将数据分配到ddr3 SRAM中
#pragma DATA_ALIGN(data_toPC, 8)
Uint8 data_toPC[POINTSAFTERPRO * 2] = {0}; //hxl added

/*******************************************************************************
 * 1.0x104000——测试灯地址(低4位控制灯)
 * 2.0x100000——主控参数的测试地址(长度0x400DW)
 * *****************************************************************************/
#pragma DATA_SECTION(paras_PCwrite, ".PCParasVars")								 //将数据分配到L2 SRAM中,用于SRIO发送或接收。必须放置在L2SRAM中
#pragma DATA_ALIGN(paras_PCwrite, 8)											 //8
PARASBUF0 paras_PCwrite = {0, STOP, DSTADDR_START, 1, {1, 0, 0, 0, 0, 0, 0, 0}}; //hxl:包含命令字和参数数据;用于缓存PC机下传过来的原始数据

/*------------paras_PCwrite1仅用于回读测试-----------------------------*/
#pragma DATA_SECTION(paras_PCread, ".PCParasVars") //将数据分配到L2 SRAM中
#pragma DATA_ALIGN(paras_PCread, 8)
volatile PARASBUF0 paras_PCread = {0, STOP, DSTADDR_FPGA_RAM, MAXLEN_PERWRPURE_DW, {0, 0, 0, 0, 0, 0, 0, 0}}; //hxl:包含命令字和参数数据;用于缓存PC机下传过来的原始数据

#pragma DATA_SECTION(commandToFPGA, ".PCParasVars")								 //将数据分配到L2 SRAM中,用于SRIO发送或接收。必须放置在L2SRAM中
#pragma DATA_ALIGN(commandToFPGA, 8)											 //8
PARASBUF0 commandToFPGA = {0, STOP, DSTADDR_START, 1, {1, 0, 0, 0, 0, 0, 0, 0}}; //hxl:包含命令字;用于DSP与FPGA之间的启动和停止指令的发送

/*
#pragma DATA_SECTION( paras_toFPGA, ".ParasPCMem")//
#pragma DATA_ALIGN (paras_toFPGA, 16)//
volatile PARASBUF1 paras_toFPGA[SIZE_PARASARRAY];//hxl:用于缓存来自PC机的参数，准备送给SRIO线程

#pragma DATA_SECTION( paras_fromFPGA, ".ParasPCMem")//
#pragma DATA_ALIGN (paras_fromFPGA, 16)//
volatile PARASBUF1 paras_fromFPGA[SIZE_PARASARRAY];//hxl:用于缓存来自PC机的参数，准备送给SRIO线程
*/

PARASBUF0 paras_toFPGA[SIZE_PARASARRAY]; //hxl:用于缓存来自PC机的参数，并回传给PC，用于测试
UInt8 **ptr_srcDataBuffer = srcDataBuffer;
int index_toFPGA = 0;
static CMDTYPE cmd_type = STOP; //hxl:1-下载参数；2-读取参数；3-启动上传数据；4-中断传输

int cntISR0 = 0, cntISR1 = 0; //hxl:用于统计任务执行次数 for test
char *ptrToW = (char *)&paras_PCwrite;

static int headIndex_toFPGA = 0, tailIndex_toFPGA = 0, cnt_toFPGA = 0;
/*-----move here from task_TCP_recvSvr() ------------------------------*/
int cnt_ReadParas = 0;
int flag_READPARAS = 0;
int restart_taskTCPSvr = 0;
int errn;
int key1;
Uint8 *Pdata = data_toPC; //"Hello!";//hxl added

int totalBytes;
int index_readyToSend = 0;
//static int cmd_PC=0;//hxl:初始状态为：没有接收PC机的指令

int num_PACKETS = 2 * TOTALPOINTS / 4 / PACKSIZE; //hxl:将一根线分成若干包发送
int countTime = 0;								  //hxl:用于统计任务执行次数 for test
int counts_svr = 0;								  //hxl:用于统计任务执行次数 for test
int32_t startTime0;
static void cmdAnalysis(void);
static void DSPWriteSTOPToFPGA_SRIO_DIO(PARASBUF0 *ptr);
static void DSPWriteStartToFPGA_SRIO_DIO(PARASBUF0 *ptr);

ERROR_NUM_SRIO_WR errn_srio = NOERROR_SRIO; //hxl:added
SOCKET stcpactive = INVALID_SOCKET;

//--------------------------------------------------------

#define INTERRUPT_LINE 0

/* Notify event number that the app uses */
#define EVENTID 10

//UInt32 times = 0;
//UInt16 recvnumes = 0;

#define masterProc 0
#define sloverProc1 1
#define sloverProc2 2
#define sloverProc3 3
#define sloverProc4 4
#define sloverProc5 5
#define sloverProc6 6
#define sloverProc7 7
#define sloverNum 2

#include <xdc/std.h>

/*  -----------------------------------XDC.RUNTIME module Headers    */
#include <xdc/runtime/System.h>

/*  ----------------------------------- IPC module Headers           */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/Ipc.h>
/*  ----------------------------------- BIOS6 module Headers         */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>

/*  ----------------------------------- To get globals from .cfg Header */
#include <xdc/cfg/global.h>

//----------------------------------------

void task_TCP_Svr(void) //from nullsvr(),// Fast Sockets Programming
{
	SOCKET stcp = INVALID_SOCKET;
	//SOCKET   stcpactive = INVALID_SOCKET;
	struct sockaddr_in sin1;
	struct timeval timeout; // Timeout struct for select
	int size, tmp = -1;
	HANDLE hBuffer; //HANDLE表示void* 类型
	//char     *pBuf=buf_Recv;//hxl:使用recv()函数接收时
	char *pBuf; //hxl:使用recvnc()接收时
	int bytes, leftBytes;
	int cntBytes = -1;
	int i, len, leftN;
	char *ptrToR;
	int pack_left = 0, head_left = CMDLEN_PERWR_W * BYTESPERWORD;
	//Error_Block eb;
	unsigned int addr;
	int linth, firIndex0;

	// Allocate the file environment for this task
	fdOpenSession(TaskSelf()); //hxl：分配文件描述符表，并与任务handle相关联。使用socket的任务必须调用该函数

	// Create the main TCP listen socket
	stcp = socket(AF_INET, SOCK_STREAMNC, IPPROTO_TCP); //创建一个流式套接字,hxl modified
	//stcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建一个流式套接字
	if (stcp == INVALID_SOCKET) //如果出错，退出。INVALID_SOCKET表示错误信息
	{
		printf("Fail socket, %d\n", fdError());
		goto leave;
	}

	// Set Port = 7, leaving IP address = Any
	bzero(&sin1, sizeof(struct sockaddr_in));
	sin1.sin_family = AF_INET;
	sin1.sin_len = sizeof(sin1);
	sin1.sin_port = htons(7);

	// Bind socket
	if (bind(stcp, (PSA)&sin1, sizeof(sin1)) < 0) //将端口等信息与socket绑定
	{
		printf("Fail to bind socket, %d\n", fdError());
		goto leave;
	}

	//// Start listening
	if (listen(stcp, 1) < 0) //将socket转换为被动侦听socket，并将客户端发送的连接请求放入请求队列中
	{
		printf("Fail to listen on socket, %d\n", fdError());
		goto leave;
	}
	// Configure our timeout to be 15 seconds
	timeout.tv_sec = 15;
	timeout.tv_usec = 0;
	setsockopt(stcp, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	setsockopt(stcp, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	//printf("EchoSrv Initialized\n");//hxl:for debug
	platform_write("EchoSrv Initialized\n");

	//Timer_start(timerHandle);//静态创建的myTimer句柄  //hxl added:启动定时器
	// Run until task is destroyed by the system
	// Check for a new TCP connection
	size = sizeof(sin1);

	//-----------------------------------------------------
	master_core_task(); //----------------------------------------------------zhou---2018--
						//--------------------------------------------------------

	/************for SRIO releated config***********************************************/
	if (dioDriverStart(&drvCfg, &hDrvManagedSrioDrv) == -1) //hxl:配置和开启SRIO driver
	{

		platform_write("SRIO driver failed to satrt\n");
	}
	/*SRIOSrcDstBufAllocI(ptr_srcDataBuffer,MAXLEN_PERWRPURE_DW*BYTESPERDW,2);//hxl:为SRIO传输分配用户buf
	paras_PCwrite.ptr_parasarray=ptr_srcDataBuffer[0];//hxl:为paras_PCwrite指定部分指针
	paras_fromFPGA.ptr_parasarray=ptr_srcDataBuffer[1];*/
	/***********************************************************/
	SRIOisrConfig_LSU(&hDrvManagedSrioDrv, 0); //hxl added。关闭LSU完成中断
	SRIOisrConfig_Dbl(&hDrvManagedSrioDrv, 0); //hxl:关闭doorbell 中断

	isSRIOInitialized[0] = 0;  //hxl:core 0 是SRIO的初始化core
	if (srio_soft_reset() < 0) //hxl:SRIO PLL is disable and SRIO is power down
	{
		System_printf("Error: SRIO PSC power off Failed\n");
		return;
	}
	if (enable_srio() < 0) //hxl:重新使能电源和时钟
	{
		System_printf("Error: SRIO PSC Initialization Failed\n");
		return;
	}
	if (SrioDevice_init() < 0) //hxl:重新初始化SRIO并等待物理层port ok
	{
		printf("SrioDevice_init error!/n");
		goto leave; //hxl:任务结束
	}

	//Write to the SHARED memory location at this point in time. The other cores cannot execute
	// till the SRIO Driver is up and running.//
	isSRIOInitialized[0] = 1; //hxl:位于".srioSharedMem"的全局变量
	// The SRIO IP block has been initialized. We need to writeback the cache here because it will
	//ensure that the rest of the cores which are waiting for SRIO to be initialized would now be
	// woken up. //
	CACHE_wbL1d((void *)&isSRIOInitialized[0], 128, CACHE_WAIT); //向L1dcatch写128字节数据 isSRIOInitialized

	//--hxl:DSP通知FPGA启动回波数据上传--------------------
	//---hxl:先通知FPGA停止上传数据和doorbell发送-----
	SRIOisrConfig_LSU(&hDrvManagedSrioDrv, 1);   //hxl added。LSU完成中断重新使能
	SRIOisrConfig_Dbl(&hDrvManagedSrioDrv, 1);   //hxl added
	DSPWriteSTOPToFPGA_SRIO_DIO(&commandToFPGA); //hxl:DSP 写停止指令给FPGA
	/*******************************************************************/

	// Enable Time Stamp Counter //
	CSL_tscEnable(); //hxl:used the time stamp counter
	//---hxl:先通知FPGA停止上传数据和doorbell发送-----
	platform_write("DSP stopped FPGA from sending data line ! \n");
	//SRIOisrConfig_Dbl(&hDrvManagedSrioDrv,0);//hxl:关闭doorbell 中断

	/*****end*******for SRIO releated config***********************************************/
	for (;;)
	{
		if (INVALID_SOCKET == stcpactive) //hxl:若尚未与客户端连接
		{
			/* Connect socket */
			stcpactive = accept(stcp, (PSA)&sin1, &size); //阻塞式用法。从请求队列中接受一个链接请求，并建立连接，返回一个新的连接socket供新的连接使用;
			if (stcpactive == INVALID_SOCKET)
			{
				goto leave;
			}
		}
		/*下面采用非阻塞式接收，以备随时接收PC机发送来的指令*/
		cntBytes = (int)recvnc(stcpactive, (void **)&pBuf, MSG_DONTWAIT, &hBuffer); //hxl:非阻塞式接收 . #define MSG_WAITALL 0x0004
		//cnt = (int)recv(stcpactive, (void *)pBuf,PACKSIZEFROMPC,MSG_DONTWAIT);//hxl:非阻塞式接收 . #define MSG_WAITALL 0x0004
		/***hxl：若接收到数据，将pBuf中接收的数据copy出来,放入循环数组********************************/
		if (cntBytes > 0) //hxl:接收数据中包含完整的命令字长
		{
			ptrToR = pBuf;
			len = 0;		  //hxl:实际单次读取的参数数据的长度
			leftN = cntBytes; //hxl:记录pbuf中尚未读取的字节数
			while (leftN > 0)
			{
				if (pack_left == 0) //hxl:上一组参数已经完整读取并保存
				{
					if (head_left <= leftN) //hxl:新一组参数包的指令字可以完整读取
					{
						memcpy((char *)ptrToW, (char *)ptrToR, head_left);
						len = head_left;
						head_left = CMDLEN_PERWR_W * BYTESPERWORD; //hxl:指令 字全部读取完毕,重新开始下一组命令字;
						ptrToR += len;
						leftN -= len;
						if (paras_PCwrite.cmd_type == DOWNPARAS)
						{
							pack_left = paras_PCwrite.length_DW * 2 * BYTESPERWORD; //hxl:当前参数组的剩余字节数
							ptrToW = (char *)paras_PCwrite.parasarray;				//hxl:下一步写纯参数数据至ptr_parasarray
						}
						else //hxl:其余指令类型条件下，仅有指令，无纯参数数据部分
						{
							pack_left = 0;
							ptrToW = (char *)(&paras_PCwrite);
						}
						/*------分析读取的指令字--------------------------------*/
						cmdAnalysis();
						//--end----分析读取的指令字--------------------------------//
					}
					else //hxl:新一组参数包的指令字尚不能完整读取
					{
						memcpy((char *)ptrToW, (char *)ptrToR, leftN);
						len = leftN;
						head_left -= leftN; //hxl:指令 字全部读取完毕;
						ptrToW += len;
						ptrToR += len;
						leftN -= len;
					}
				}
				else //hxl:上一组参数尚未读取完毕//当且仅当cmd_type==DOWNPARAS时，才会执行该分支
				{
					if (leftN >= pack_left) //hxl:当前组参数可以保存完毕
					{
						memcpy((void *)ptrToW, ptrToR, pack_left); //hxl:指令数据存入参数buffer;
						len = pack_left;
						pack_left = 0;
						ptrToW = (char *)(&paras_PCwrite);
						ptrToR += len;
						leftN -= len;
						/*---将本组参数指令及参数数据缓存，后面回传给PC进行验证--实际应用程序中本段不需要-----------------*/
						/*
						//printf("receive %d Bs!\n",paras_PCwrite.length_DW);
						paras_toFPGA[headIndex_toFPGA]=paras_PCwrite;
						headIndex_toFPGA++;*/
						cnt_toFPGA++;
						/*if(headIndex_toFPGA==SIZE_PARASARRAY)
						{
							headIndex_toFPGA=0;
						}*/
						/*--end-将本组参数数据缓存，后面回传给PC进行验证--实际应用程序中本段不需要-----------------*/
						switch (paras_PCwrite.headForGuide) //hxl:3-将本组参数数据根据引导头进行解析和分配
						{
						case HEAD0_PCWRTPARAS0: //通过SRIO写本组参数至FPGA
						{
							/*------此处需要通过SRIO写本组参数至FPGA---------*/
							addr = paras_PCwrite.addr;
							paras_PCwrite.addr = addr * 8;
							if (coreNum == CORE_SYS_INIT)
							{
								if (dioSockets_doWr(hDrvManagedSrioDrv, Srio_Ftype_WRITE, Srio_Ttype_Write_NWRITE_R, &paras_PCwrite) < 0) //hxl:注意相应的FTYPE和TTYPE组合
								{
									System_printf("Error: DSP2FPGA DIO NWRITE failed\n");
									errn_srio = ERR_DIOSOC_DOWRTE;
									//Task_exit();//2017-10-24 hxl modified
								}
							}
							paras_PCwrite.addr = addr;
#ifdef DEBUG
							printf("write to FPGA %dth:%d DWs\n", cnt_toFPGA, paras_PCwrite.length_DW);
#endif
							/*--end----此处需要通过SRIO写本组参数至FPGA---------*/
							break;
						}

						case HEAD0_PCWRTPARAS1: //hxl:就地保存本组参数供DSP后处理使用
						{

							addr = paras_PCwrite.addr;
							firIndex0 = fir_addrToIndex(addr); //hxl:地址标签到索引的映射
							addr -= addrs_fir[firIndex0];	  //hxl:计算偏移地址
							if (firIndex0 < 4)
							{
								memcpy((&firs_native[firIndex0] + addr * 8), (void *)(paras_PCwrite.parasarray), paras_PCwrite.length_DW * BYTESPERDW); //hxl:保存当前的滤波器系数组
							}
							else if (firIndex0 == 4)
							{
								memcpy((void *)(index_firs_native + addr * 8), (void *)(paras_PCwrite.parasarray), paras_PCwrite.length_DW * BYTESPERDW);
							}
							break;
						}
						default:
							break;
						}
					}
					else //hxl:当前组参数尚未保存完毕
					{
						memcpy((void *)ptrToW, ptrToR, leftN); //hxl:指令数据存入参数buffer;
						len = leftN;
						pack_left -= len;
						ptrToW += len;
						ptrToR += len;
						leftN -= len;
					}
				}
			}
			recvncfree(hBuffer); //hxl:用于recvnc()接收时
		}
		/*end**hxl：若接收到数据，将pBuf中接收的数据copy出来,放入循环数组********************************/
		else if (cntBytes == 0) //hxl:cnt=0，表示对端关闭连接
		{
			if (stcpactive != INVALID_SOCKET)
			{
				fdClose(stcpactive); //hxl:对端关闭连接时，将已经打开的socket关闭
			}
			stcpactive = INVALID_SOCKET; //下次重新连接
			cmd_type = STOP;			 //hxl:从初始命令类型开始
			errn = fdError();			 //hxl:提取错误并判断。added 2017-10-17;进入该分支后错误码为 EWOULDBLOCK。
		}
		else if (cntBytes < 0) //hxl:表示接收出错或超时(有可能对端并未发送);
		{
			/******此处若调用stcpactive=INVALID_SOCKET;需谨慎，易导致连接在不必要的时候关闭*******************/
			errn = fdError();		 //hxl:提取错误并判断。
			if (errn == EWOULDBLOCK) //hxl:超时;可能是对端未发送，此时不能关闭连接
			{
				;
			}
		}
		//-------hxl:此处用于连续上传图像数据-------------------------------//
		{
			/*------//hxl:实际应用程序必须保留该段代码，从doorbell中断请求信号量-------------------------*/
			//tmp=Semaphore_pend(mysem1,BIOS_WAIT_FOREVER);//hxl:请求中断的信号量 //第二组，2016-8-19.对应LINENUM_BUF=80，经测试，稳定
			//tmp=Semaphore_pend(mysem1, 50);//hxl:modified for dealing SRIO  crashing，2017-6-19
			tmp = Semaphore_pend(mysem1, BIOS_NO_WAIT); //hxl:modified for dealing SRIO  crashing，2017-6-19.task不挂起
			if (1 == tmp)
			/*---end---//hxl:实际应用程序必须保留该段代码，从doorbell中断请求信号量-------------------------*/
			{
				//key1 = Hwi_disableInterrupt(10);//hxl:2016-7-26 doorbell使用的是中断10.注意中断关闭时间太长易有问题。

				memcpy((char *)Pdata, (char *)(&beforeData[index_readyToSend][0]), 4); //hxl:前4个字节为帧号和线号
				linth = beforeData[index_readyToSend][1];							   //hxl:提取当前的数据线的线序号
				//time0=(unsigned long long)TSCH;//hxl:用于统计时间
				//time0=(((unsigned long long)time0)<<32)+(unsigned long long)TSCL;
				zeroPhaseFilter_F((const short *)&beforeData[index_readyToSend][2], firs[index_firs[linth]]); //hxl:第1、2元素为帧号和线号，不参与滤波
				//------hxl:4倍下采样--------

				//for(i=2;i<POINTSAFTERPRO;i++)//hxl:采用4倍下采样
				//{
				//	((short*)Pdata)[i]= (short)(r[4*(i-2)]);
				//}

				//------hxl:2倍下采样--------
				for (i = 2; i < POINTSAFTERPRO; i++) //hxl:采用2倍下采样,2018-01-08
				{
					//((short*)Pdata)[i]=(short)(r[2*(i-2)]);
				}

				//------hxl:暂时不下采样。等待PC端软件修改后再下抽--------
				//for(i=2;i<POINTSAFTERPRO;i++)//hxl:
				//{
				//	((short*)Pdata)[i]= (short)(r[i-2]);
				//}
				//time1=(unsigned long long)TSCH;//hxl:用于统计时间
				//time1=(time1<<32)+(unsigned long long)TSCL;
				//DeltTime1=time1-time0;
				//Hwi_restoreInterrupt(10,key1);

				/*
				for(i=2;i<POINTSAFTERPRO;i++)//hxl:采用4倍下采样
				{
					((short*)Pdata)[i]= beforeData[index_readyToSend][2*(i-1)];
				}*/
				//----------一次发送一根线-----------------------------------------
				totalBytes = 0;
				leftBytes = BYTESPERTRAN - totalBytes;
				while (totalBytes < BYTESPERTRAN)
				{
					bytes = send(stcpactive, (char *)(&Pdata[totalBytes]), leftBytes, 0); //hxl:moved here*/
					totalBytes += bytes;
					leftBytes = BYTESPERTRAN - totalBytes;
				}
				//-------------end---一次发送一根线----------------------------------

				if (totalBytes < 0) //hxl modified
				{
					fdClose(stcpactive);
					stcpactive = INVALID_SOCKET;
					errn = fdError();
					if (errn == 9 || errn == 57) //hxl:2017-6-15: 9——PC机软件在传输数据时突然关闭；57——对端关闭连接,PC机软件关闭
					{
						//hxl:此处需要进一步确认
						DSPWriteSTOPToFPGA_SRIO_DIO(&commandToFPGA); //hxl:DSP 通知FPGA停止发送数据线
						platform_write("DSP stopped FPGA from sending data line because PC closed! \n");
					}
				}
				else
				{
					index_readyToSend++;
					if (LINENUM_BUF == index_readyToSend)
					{
						index_readyToSend = 0;
					}
				}

				counts_svr = 0; //hxl:能请求到sem1说明SRIO doorbell isr 能收到，则SRIO接口当前连接正常
			}
			/*---end---added for test------------------------------*/
			else
			{

				counts_svr++; //当前未请求到doorbell信号量。
				Task_sleep(1);
			}
		}
		/****2017-6-15 hxl added the following for SRIO crash**********************************/
		if (errn_srio > 0 || counts_svr > 500000) //hxl:(1)SRIO读写出错 (2)DSP不在上传数据至PC的阶段时(可能在接收PC机参数或者较空闲阶段)或者SRIO doorbell Isr 提供的信号量已经不足时.与上面的tmp=Semaphore_pend(mysem1, BIOS_NO_WAIT)配合使用;
		{
			CheckSRIOPortOK();

			for (i = OUTPORTID_SRIO; i < OUTPORTID_SRIO + 1; i++) //hxl:仅检测当前使用的端口port0或port2
			{
				if (flags_portNotOK[i] == 1) //hxl:SRIO 物理层出现问题
				{
					flags_portNotOK[i] = 0;
					restart_taskTCPSvr = 1; //hxl:just for test,modified to 0,去掉异常处理
				}
			}
			//----hxl:SRIO 出现问题后重新初始化并重新开始运行;--------
			if (restart_taskTCPSvr == 1)
			{

				isSRIOInitialized[0] = 0;  //hxl:core 0 是SRIO的初始化core
				if (srio_soft_reset() < 0) //hxl:SRIO PLL is disable and SRIO is power down
				{
					System_printf("Error: SRIO PSC power off Failed\n");
					return;
				}
				if (enable_srio() < 0) //hxl:重新使能电源和时钟
				{
					System_printf("Error: SRIO PSC Initialization Failed\n");
					return;
				}
				if (SrioDevice_init() < 0) //hxl:重新初始化SRIO并等待物理层port ok
				{
					printf("SrioDevice_init error!/n");
					goto leave; //hxl:任务结束
				}

				//Write to the SHARED memory location at this point in time. The other cores cannot execute
				// till the SRIO Driver is up and running.//
				isSRIOInitialized[0] = 1; //hxl:位于".srioSharedMem"的全局变量
				// The SRIO IP block has been initialized. We need to writeback the cache here because it will
				//ensure that the rest of the cores which are waiting for SRIO to be initialized would now be
				// woken up. //
				CACHE_wbL1d((void *)&isSRIOInitialized[0], 128, CACHE_WAIT); //向L1dcatch写128字节数据 isSRIOInitialized

				//--hxl:DSP通知FPGA启动回波数据上传--------------------
				//---hxl:先通知FPGA停止上传数据和doorbell发送-----
				SRIOisrConfig_LSU(&hDrvManagedSrioDrv, 1);   //hxl added。LSU完成中断重新使能
				SRIOisrConfig_Dbl(&hDrvManagedSrioDrv, 1);   //hxl:重启doorbell 中断
				DSPWriteSTOPToFPGA_SRIO_DIO(&commandToFPGA); //hxl:DSP 写停止指令给FPGA
				platform_write("DSP stopped FPGA from sending data line after SRIO exception ! \n");

				//hxl:---全局变量回到初始状态------
				Flag_EvenOdd = 0;
				srioDoorblIsrCnt = 0;
				srioDoorblIsrServiced = 0;
				Semaphore_reset(mysem1, 0); //hxl:信号量初始化为0
				WriteToIndex = 0;
				index_readyToSend = 0;
				//hxl:-end--全局变量回到初始状态------
				DSPWriteStartToFPGA_SRIO_DIO(&commandToFPGA); //hxl:此处可以写成功，但收不到响应包。
				restart_taskTCPSvr = 0;
			}
			counts_svr = 0;
			errn_srio = NOERROR_SRIO; //hxl:恢复到未出错状态
		}
		/**end**2017-6-15 hxl added the following for SRIO crash**********************************/
		//System_printf("exit recvSvr()\n");
		//Task_sleep(1);//发送完毕后，任务sleep，数据处理任务即可请求到信号量; ms
	}
leave:
	// We only get here on an error - close the sockets
	if (stcp != INVALID_SOCKET)
		fdClose(stcp);
	fdCloseSession(TaskSelf());
	printf("EchoSrv Fatal Error\n");
	// This task is killed by the system - here, we block
	TaskBlock(TaskSelf());
}

/******************************************************************************************************************************
 *cmdAnalysis()
 * hxl:2017-1-12
 * 分析由PC机接收来的指令，并执行不同的操作
 * 函数内部的语句修改时请慎重！！！！！
 ******************************************************************************************************************************/
static void cmdAnalysis(void)
{
	unsigned int addr;
	unsigned firIndex;
	cmd_type = paras_PCwrite.cmd_type;

	switch (cmd_type) //hxl:根据paras_PCwrite.cmd_type的值采取不同的操作
	{
	case DOWNPARAS: //hxl:1——表示下载参数   /*待参数内容接收完毕后再由SRIO转发给FPGA*/
	{
		/*************************************************************/
		break;
	}

	case READPARAS: //hxl:2——表示读取参数   /*后面需要修改为处理函数的形式*/
	{
		memset(((char *)Pdata), 0x00, BYTESPERTRAN);
		switch (paras_PCwrite.headForGuide) //hxl:根据本组参数的引导头进行判断。4-待读取的参数的准备工作
		{

		case HEAD0_PCWRTPARAS0: //通过SRIO读取FPGA的参数
		{
			/***************************************************************************
					 * 向FPGA发送读请求，读取FPGA中的参数数据.需要进一步确定FPGA端目的地址
					***************************************************************************/
			if (coreNum == CORE_SYS_INIT)
			{
				/*****模拟：从FPGA请求参数*************************************************/
				paras_PCread = paras_PCwrite;
				addr = paras_PCwrite.addr;
				paras_PCread.addr = addr * 8;																			 //hxl:将double word地址转换为byte地址
				if (dioSockets_doR(hDrvManagedSrioDrv, Srio_Ftype_REQUEST, Srio_Ttype_Request_NREAD, &paras_PCread) < 0) //hxl:注意相应的FTYPE和TTYPE组合
				{
					System_printf("Error: DSP2FPGA DIO NREAD failed\n");
					errn_srio = ERR_DIOSOC_DOREAD;
					//Task_exit();//2017-10-24 hxl modified
				}
				/*--end-将本组参数数据缓存，后面回传给PC进行验证-------------------*/
				countTime++; //hxl:用于统计命令解析次数，for test
#ifdef DEBUG
				printf(" Read from FPGA %dth:%d DWs\n", countTime, paras_PCread.length_DW);
#endif
			}
			//-----实际应用中，无须copy，直接利用paras_fromFPGA发送--------------------//
			paras_PCread.headForGuide = HEAD0_PCREADPARAS0; //hxl:上传参数时自定义的上传引导头：需要从FPGA读回的参数
			paras_PCread.addr = paras_PCwrite.addr;
			; //hxl:将double word地址上传给PC机，与PC机下传时格式相统一
			//--end---实际应用中，无须copy，直接利用paras_fromFPGA发送-----------------//
			break;
		}
		case HEAD0_PCWRTPARAS1: //hxl:请求读取的参数无需从FPGA读取
		{
			paras_PCread = paras_PCwrite;
			addr = paras_PCwrite.addr;
			firIndex = fir_addrToIndex(addr);
			addr -= addrs_fir[firIndex]; //hxl:计算偏移地址
			if (firIndex < 4)			 //hxl:读取滤波器系数参数
			{
				memcpy((void *)paras_PCread.parasarray, (void *)(&firs_native[firIndex] + addr * 8), paras_PCwrite.length_DW * BYTESPERDW);
			}
			else if (firIndex == 4)
			{
				memcpy((void *)paras_PCread.parasarray, (void *)(index_firs_native + addr * 8), paras_PCwrite.length_DW * BYTESPERDW);
			}
			paras_PCread.headForGuide = HEAD0_PCREADPARAS1; //hxl:上传参数时自定义的上传引导头:直接来自DSP的参数
			paras_PCread.addr = paras_PCwrite.addr;
			break;
		}
		default:
			break;
		}
		//---------hxl:数据copy至发送buf pData中------------------------------------------------------------
		memcpy(((char *)Pdata), (char *)&paras_PCread, (CMDLEN_PERWR_W + paras_PCread.length_DW * 2) * BYTESPERWORD);
		//totalBytes = send(stcpactive, (char*)(&paras_fromFPGA), BYTESPERTRAN, 0 );//hxl:上传字节需要补齐到8192B*/
		/*---end---在高版本的应用程序中，此处发送的是SRIO由FPGA接收的参数数据----------------------------------------------------------------------------*/
		totalBytes = send(stcpactive, (char *)Pdata, BYTESPERTRAN, 0);

		if (totalBytes < 0) //hxl:发送超时或出错
		{
			//fdClose( stcpactive );//hxl:
			errn = fdError();
			stcpactive = INVALID_SOCKET; //hxl:下次重新连接
			printf("Send Paras to PC error,error num %d!\n", errn);
		}
		if (totalBytes != BYTESPERTRAN) //hxl:表明发送出错
		{
			printf("Paras read by PC is shorter than POINTSAFTERPRO*2!\n"); //hxl: for debug
		}

		//hxl:参数数据上传至PC之后，恢复初始状态
		//hxl:若发送完毕：

		break;
	}
	case UPDATA: /*hxl:3——表示启动数据传输*/
	{
		//----更新滤波器系数和滤波器索引表----
		memcpy(&index_firs, &index_firs_native, sizeof(index_firs));
		memcpy(&firs, &firs_native, sizeof(firs));
		/*******hxl:写启动指令之前各变量回到初始状态********************/
		Flag_EvenOdd = 0;
		srioDoorblIsrCnt = 0;
		srioDoorblIsrServiced = 0;
		Semaphore_reset(mysem1, 0); //hxl:信号量初始化为0
		WriteToIndex = 0;
		index_readyToSend = 0;
		SRIOisrConfig_Dbl(&hDrvManagedSrioDrv, 1); //hxl:重启doorbell 中断
												   /*****写启动指令给FPGA**************/
		DSPWriteStartToFPGA_SRIO_DIO(&paras_PCwrite);

		/****end***hxl:在最终版的应用程序中,此处需要向FPGA写指令********************/
		break;
	}
	case STOP: /*hxl:4——表示中断数据传输*/
	{
		DSPWriteSTOPToFPGA_SRIO_DIO(&paras_PCwrite);
		//stcpactive = INVALID_SOCKET;//hxl:此处不能设置为无效连接，即断开连接，因为后面还希望继续接收启动传输等其它指令
		/*************************************************************/
		break;
	}
	//---end-解析命令并保存数据-------
	default: //hxl:cmd_type>5或cmd_type=0时，阻塞式接收命令字
	{
		;
	}
	}
}
/**********************************************************************************/
void DSPWriteSTOPToFPGA_SRIO_DIO(PARASBUF0 *ptr)
{
	int i;
	if (coreNum == CORE_SYS_INIT)
	{

		ptr->addr = DSTADDR_START;
		ptr->length_DW = LEN_STARTCOMMAND;
		ptr->parasarray[0] = 0x01;
		for (i = 1; i < BYTESPERDW; i++)
		{
			ptr->parasarray[i] = 0x00;
		}
		/*****给FPGA写停止上传指令*****************************/
		for (i = 0; i < 3; i++) //hxl:重复写3次，保证写成功  //
		{
			if (dioSockets_doWr(hDrvManagedSrioDrv, Srio_Ftype_SWRITE, Srio_Ttype_Swrite_DEFAULT, ptr) < 0) //hxl:注意相应的FTYPE和TTYPE组合
			{
				System_printf("Error: DSP2FPGA DIO NWRITE STOP command failed\n");
				errn_srio = ERR_DIOSOC_DOWRTE;
				//Task_exit();
			}
		}
		/*****等待片刻*********/
		startTime0 = TSCL;
		while (((TSCL - startTime0) < TIME_A_FRAME))
			;							 //hxl:等待FPGA发送完整最后一帧图像
										 /******读取FPGA端的指令状态*************/
		ptr->addr = SHADOWDSTADDR_START; //hxl:仅用于SRIO loopback 测试;实际应为FPGA端内存的地址
		ptr->length_DW = LEN_STARTCOMMAND;
		ptr->parasarray[0] = 0x00; //
		//hxl:--读取指令字并验证-----
		while (ptr->parasarray[0] != 0x01)
		{
			if (dioSockets_doR(hDrvManagedSrioDrv, Srio_Ftype_REQUEST, Srio_Ttype_Request_NREAD, ptr) < 0) //hxl:注意相应的FTYPE和TTYPE组合
			{
				System_printf("Error: DSP2FPGA DIO NREAD STOP command status  failed\n");
				errn_srio = ERR_DIOSOC_DOREAD;
				//Task_exit();//2017-10-24 hxl modified
			}
		}
	}
}
void DSPWriteStartToFPGA_SRIO_DIO(PARASBUF0 *ptr)
{
	int i;
	ptr->addr = DSTADDR_START; //hxl:与zyy协商修改地址
	ptr->length_DW = 1;		   //hxl:需要向FPGA写当前命令字的内容,命令字长度为1W
	//-----hxl:要写给FPGA的指令内容:1或0(与FPGA之间的约定)，表示启动或停止传输---------
	ptr->parasarray[0] = 0x00; //hxl:0——启动，1——停止
	for (i = 1; i < BYTESPERDW; i++)
	{
		ptr->parasarray[i] = 0x00; //hxl:要写给FPGA的内容:0(与FPGA之间的约定)
	}
	if (coreNum == CORE_SYS_INIT)
	{
		if (dioSockets_doWr(hDrvManagedSrioDrv, Srio_Ftype_SWRITE, Srio_Ttype_Swrite_DEFAULT, ptr) < 0)
		//if (dioSockets_doWr(hDrvManagedSrioDrv,Srio_Ftype_WRITE, Srio_Ttype_Write_NWRITE_R,&paras_PCwrite) < 0)//hxl:注意相应的FTYPE和TTYPE组合；实际应用程序中，此句必须保留
		{
			System_printf("Error: DSP2FPGA DIO NWRITE START command failed\n");
			errn_srio = ERR_DIOSOC_DOWRTE;
			//Task_exit();//hxl:2017-10-24  modified
		}
	}
}
