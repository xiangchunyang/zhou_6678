/*
 * SRIO_loopbackDio.h
 *
 *  Created on: 2016-6-30
 *      Author: hxl
 *      Used for declare vars defined in SRIO_loopbackDioIsr.c and used in other .c
 *      file
 */

#ifndef SRIO_LOOPBACKDIO_H_
#define SRIO_LOOPBACKDIO_H_

/* SRIO Driver Include File. */
#include <ti/drv/srio/srio_drv.h>//hxl added
#include <ti/drv/srio/srio_osal.h>//hxl added
/* Platform utilities include */
#include <ti/platform/platform.h>
#include <ti/sysbios/knl/Semaphore.h>////2016-2-29:hxl: added;//2016-6-30 hxl added it to client.h
//#include <ti/sysbios/heaps/HeapMem.h>//hxl:added 2017-5-11
#include "App_common.h"
/*----hxl:-macro definations------------*/
/**********************************************************************
 ************************** LOCAL Definitions *************************
 **********************************************************************/
/*----hxl move the following part to this SRIO_loopbackDioIsr.h from SRIO_loopbackDioIsr.c-------------*/
/* This is the Number of host descriptors which are available & configured
 * in the memory region for this example. */
//#define NUM_HOST_DESC               128 //hxl:deleted
#define NUM_HOST_DESC_SRIO  128 //2016-6-29 hxl added
/* This is the size of each descriptor. */
//#define SIZE_HOST_DESC              48 //hxl:deleted
#define  SIZE_HOST_DESC_SRIO  48    //2016-6-29 hxl:added

/* MTU of the SRIO Driver. We are currently operating @ MTU of 256 bytes. */
#define SRIO_MAX_MTU				1024//256//hxl modified from 256 to 1024

/* This is the size of the data buffer which is used for DIO Sockets. */
#define SIZE_DIO_PACKET				8//1024//256//128//256//8//8//128 //hxl modified to 1024
#define SIZE_DIO_PACKET_R 			8//128//16//256//1024//8//1024 //hxl:added
/* Defines the core number responsible for system initialization. */
#define CORE_SYS_INIT               0

/* Defines number of DIO sockets used in this example */
#define SRIO_DIO_LSU_ISR_NUM_SOCKETS        1

/* Defines number of source buffers in this example */
#define SRIO_DIO_NUM_SRCBUF   2//

/* ISR timeout value (in cycles) used in this example */
#define SRIO_DIO_LSU_ISR_TIMEOUT        1000000//100000//hxl:cpu circle,1G时钟频率
/*-end---hxl move the following part to this SRIO_loopbackDioIsr.h from SRIO_loopbackDioIsr.c-------------*/
/*------hxl:macro about parameters from PC----------------------------------*/

#define SRIO_DstBase_LSB 0x0c010000//0x10000
#define SRIO_MAX_LSU_NUM 					8
#define SRIO_MAX_DEVICEID_NUM 				16
/*--------hxl type defination--------*/


typedef enum
{
  NOERROR_SRIO=0,//hxl:
  ERR_DIOSOC_DOWRTE=1,//hxl:dioSockets_doWr()
  ERR_DIOSOC_DOREAD=2//hxl:dioSockets_doR()
} ERROR_NUM_SRIO_WR;

/*-----vars defined in file SRIO_loopbackDioIsr.c and referenced in file other .c source file other than SRIO_loopbackDioIsr.c-----*/
extern volatile UInt32 srioDoorblIsrServiced;//2016-6-30 hxl added
extern volatile UInt32 srioDoorblIsrCnt;
extern volatile UInt32 srioLsuIsrServiced;
extern volatile UInt32 srioDioIsrCnt;
extern volatile int Flag_EvenOdd;
extern volatile UInt16 WriteToIndex;

extern const uint32_t DEVICE_ID2_8BIT;
extern UInt32          coreNum;

extern Srio_DrvConfig  drvCfg;//hxl:added for SRIO driver
extern Srio_DrvHandle  hDrvManagedSrioDrv;
extern CSL_SrioHandle  hSrioCSL;
//extern volatile short beforeData[LINENUM_BUF][TESTSIZE];
extern short beforeData[LINENUM_BUF][TOTALPOINTS];
extern Semaphore_Handle mysem1, mysem2;//定义一个semaphore handle
extern platform_info	gPlatformInfo;
extern UInt8* srcDataBuffer[SRIO_DIO_NUM_SRCBUF];
extern volatile Uint32   isSRIOInitialized[(SRIO_MAX_CACHE_ALIGN / sizeof(Uint32))];


extern void master_core_task();
/* Enable Time Stamp Counter */
extern void CSL_tscEnable(void);
extern CSL_Uint64 CSL_tscRead(void);//hxl:added this line
extern UInt32 l2_global_address (Uint32 addr);
extern  int dioDriverStart(Srio_DrvConfig* ptr_drvCfg,Srio_DrvHandle* ptr_hDrvManagedSrioDrv);
//extern Void SRIOisrConfig(Srio_DrvHandle* ptr_hDrvManagedSrioDrv);
extern Void SRIOisrConfig_Dbl(Srio_DrvHandle* ptr_hDrvManagedSrioDrv,Uint8 isEnble);
extern Void SRIOisrConfig_LSU(Srio_DrvHandle* ptr_hDrvManagedSrioDrv,Uint8 isEnble);
extern Void SRIOSrcDstBufAllocI(UInt8* srcDataBuffer[SRIO_DIO_NUM_SRCBUF],int bufSizeInB,int bufNum);
extern Int32 srio_soft_reset (void);
extern Int32 enable_srio (void);

extern short  tx_rx_distance[Num_line][Point_image];



//extern HeapMem_Handle heap1;
/*-end----vars defined in file SRIO_loopbackDioIsr.c and referenced in file other .c source file other than SRIO_loopbackDioIsr.c-----*/
#endif /* SRIO_LOOPBACKDIO_H_ */


