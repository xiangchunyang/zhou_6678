/*
 * SRIO_TransmitToFPGA.c
 *
 *  Created on: 2016-12-7
 *      Author: hxl
 */
#include <xdc/std.h>
//#include <string.h>
//#include <c6x.h>
//#include <xdc/runtime/IHeap.h>

#include <xdc/runtime/System.h>
/* CSL SRIO Functional Layer */
//#include <ti/csl/csl_srio.h>
/* CSL Chip Functional Layer */
#include <ti/csl/csl_chip.h>//hxl:
#include <ti/csl/csl_srioAux.h>
#include <ti/drv/srio/srio_drv.h>
#include <ti/sysbios/family/c64p/Hwi.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>
#include <ti/sysbios/family/c66/tci66xx/CpIntc.h>
#include <ti/sysbios/family/c66/cache.h>
/* CSL Cache Functional Layer */
#include <ti/csl/csl_cacheAux.h>//hxl:hxl added
#include "task_TCP_Svr.h"
#include "SRIO_TransmitToFPGA.h"
#include "SRIO_loopbackDioIsr.h"
#include "device_srio_loopback.h"





int tailIndex_toFPGA=0;
int cnt_toFPGA=0;//hxl:表征paras_toFPGA中等待发送的参数组的数量
char* ptr_DataBuffer;//hxl added this
uint32_t SRIO_DstAddr_LSB=0x0c010000;//hxl:FPGA address.modified for loopback test
unsigned int addr;
/* Global variables to count good and bad transfers */
volatile UInt32 srioDioLsuGoodTransfers = 0;
volatile UInt32 srioDioLsuBadTransfers  = 0;

Srio_SockHandle srioSocket_test1;
/* Array containing SRIO socket handles */
Srio_SockHandle srioSocket[SRIO_DIO_LSU_ISR_NUM_SOCKETS];

/*****************************************************************************************
 *  @b Description
 *  @n
 *   hxl:   The function is used to send Para data to FPGA
 *   NWRITE to FPGA
 *   hxl:当前设置下，使用LSU0，shadow register分配：LSU0——4；LSU1——4，....

 *  @param[in]  hSrioDrv
 *      Handle to the SRIO driver
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
 Int32 dioSockets_doWr(Srio_DrvHandle hSrioDrv, uint8_t dio_ftype, uint8_t dio_ttype,volatile PARASBUF0* ptr_Buf)
{
    Srio_SockBindAddrInfo   bindInfo;
    Srio_SockAddrInfo       to;
    uint16_t                sockIdx;
    //uint16_t                srcDstBufIdx = 0;
    int32_t                 startTime;
   // UInt8                   **srcDataBufPtr = NULL;//, **dstDataBufPtr = NULL;//hxl modified
    uint8_t                 compCode;
    Srio_SockHandle srioSocket_DIO;
    //uint8_t dio_ftype,dio_ttype;
   /* System_printf ("*************************************************************\n");
    System_printf ("******* DIO Socket NWrite (Core %d) ********\n", coreNum);
    System_printf ("*************************************************************\n");*/

    /*---------------hxl creat socket for doorbell and Swrite to FPGA, bind with the information-------------------------------------------*/
    sockIdx=0;
    /* Reset global counters before next run */
	srioDioLsuGoodTransfers = 0;
	srioDioLsuBadTransfers  = 0;
    /* Open DIO SRIO Non-Blocking Socket for Nwrite*/
    srioSocket_DIO = Srio_sockOpen (hSrioDrv, Srio_SocketType_DIO, FALSE);//hxl:打开socket,非阻塞式的
    if (srioSocket_DIO == NULL)
     {
          System_printf ("Error: Unable to open the DIO Nwrite socket \n");
          return -1;
      }
     /* DIO Binding Information: Use 16 bit identifiers and we are bound to the first source id.
     * and we are using 16 bit device identifiers. */
    bindInfo.dio.doorbellValid  =0;// 0;//hxl:modified it for test
    bindInfo.dio.intrRequest    =1; //1;//hxl:modified it for test;是否使能LSU发送完成中断：0——disable
    bindInfo.dio.supInt         = 0;
    bindInfo.dio.xambs          = 0;
    bindInfo.dio.priority       = 0;
    bindInfo.dio.outPortID      = OUTPORTID_SRIO;//hxl:port0 //2;
    bindInfo.dio.idSize         = 0; //1;//hxl:0b00——8bit ID;0b01——16bit ID
    bindInfo.dio.srcIDMap       = sockIdx;//hxl:选择使用哪个sorceID寄存器
    bindInfo.dio.hopCount       = 0;
    bindInfo.dio.doorbellReg    = 0;
    bindInfo.dio.doorbellBit    = 0;//hxl:表示发送参数完成.0~15bit

   /* Bind the SRIO socket: DIO sockets do not need any binding information. */
    if (Srio_sockBind_DIO (srioSocket_DIO, &bindInfo) < 0)
    {
      System_printf ("Error: Binding the SIO socket failed.\n");
       return -1;
    }
    /*----hxl set the information of dst end--------------------------------*/
    //Populate the DIO Address Information where the data is to be sent.
    //----Swrite DIO  base address initial---------
     to.dio.dstID         = DEVICE_ID2_8BIT;//FPGA ID
     to.dio.ttype         = dio_ttype;
     to.dio.ftype         = dio_ftype;
    /*===================hxl:send the  paras ===========================*/
    /*--hxl--Nwrite the  paras to FPGA-----------------------------------------------*/
    {

		to.dio.rapidIOMSB    = 0x0;
		to.dio.rapidIOLSB    =ptr_Buf->addr;//SRIO_DstBase_LSB;//SRIO_DstBase_LSB;//hxl:FPGA 接收端的地址
		//if ((dio_ftype == Srio_Ftype_WRITE) && (dio_ttype == Srio_Ttype_Write_NWRITE_R))
		{
		  /* DIO Write operation */
			ptr_DataBuffer=(char*)(ptr_Buf->parasarray);
			ptr_DataBuffer=(char*)l2_global_address((Uint32)ptr_DataBuffer);//hxl:source addr必须转换为全局地址
			//Cache_wb((void*)ptr_DataBuff er, paras_toFPGA[tailIndex_toFPGA].length_DW*BYTESPERDW, Cache_Type_ALLD, 1);//hxl:当source buf位于MSM时，必须注意cache的一致性维护的问题。否则出错
		}
		/* Send the DIO Information. */
		if (Srio_sockSend_DIO (srioSocket_DIO, (void*)ptr_DataBuffer, ptr_Buf->length_DW*BYTESPERDW, (Srio_SockAddrInfo*)&to) < 0)
		{
			System_printf ("Debug(Core %d): DIO Socket send Failed\n", coreNum);
			return -1;
		}
		/* Wait for the interrupt to occur without touching the peripheral. */
		/* Other useful work could be done here such as by invoking a scheduler */
		startTime = TSCL;
		while((! srioLsuIsrServiced) && ((TSCL - startTime) < SRIO_DIO_LSU_ISR_TIMEOUT));
		if (! srioLsuIsrServiced)
		{
			//System_printf ("NWRITE ISR didn't happen within %d circles!!! \n", SRIO_DIO_LSU_ISR_TIMEOUT);
			System_printf ("NWRITE ISR didn't happen within set time!!! \n");
		//return -1;
		}
		srioLsuIsrServiced=0;//hxl:全局变量。用于表明中断服务函数已完成
		/* Debug Message: Display ISR count */
#ifdef DEBUG
		System_printf ("Debug(Core %d): ISR Count: %d\n", coreNum, srioDioIsrCnt);//hxl记录中断次数
#endif
		/* Read the completion code filled by the ISR */
		compCode = 0xFF;
		if (Srio_getSockOpt(srioSocket_DIO, Srio_Opt_DIO_READ_SOCK_COMP_CODE, &compCode, sizeof(uint8_t)) < 0)
		{
			System_printf ("Error: Unable to read the completion code in socket\n");
			return -1;
		}
		/* Was the transfer good. */
		if (compCode == 0)
		{
			srioDioLsuGoodTransfers++;
		}
		else
		{
			srioDioLsuBadTransfers++;
		}
		/* Debug Message: Print error counters */
#ifdef DEBUG
		System_printf ("Debug(Core %d): Transfer paras Completion without Errors - %d\n", coreNum, srioDioLsuGoodTransfers);
		System_printf ("Debug(Core %d): Transfer paras Completion with Errors    - %d\n", coreNum, srioDioLsuBadTransfers);
#endif
		if (srioDioLsuBadTransfers > 0)
		{
			System_printf ("Debug(Core %d): DIO Transfer Paras Data NOT completed successfully !!! \n", coreNum);
			return -1;
		  }
#ifdef DEBUG
		System_printf ("Debug(Core %d): NWRITE with Interrupts Passed\n", coreNum);
#endif


    }
    /*-end----hxl--Nwrite the  paras to FPGA-----------------------------------------------*/
    Srio_sockClose_DIO (srioSocket_DIO);
    return 0;
}
 /*****************************************************************************************
  *  @b Description
  *  @n
  *   hxl:   The function is used to send Para data to FPGA
  *   NWRITE to FPGA

  *  @param[in]  hSrioDrv
  *      Handle to the SRIO driver
  *
  *  @retval
  *      Success -   0
  *  @retval
  *      Error   -   <0
  */
  Int32 dioSockets_doR(Srio_DrvHandle hSrioDrv, uint8_t dio_ftype, uint8_t dio_ttype,volatile PARASBUF0* ptr_Buf)
 {
     Srio_SockBindAddrInfo   bindInfo;
     Srio_SockAddrInfo       to;
     uint16_t                sockIdx,count;
     //uint16_t                srcDstBufIdx = 0;
     int32_t                 startTime;
     //UInt8                   **srcDataBufPtr = NULL;//, **dstDataBufPtr = NULL;//hxl modified
     uint8_t                 compCode;
     Srio_SockHandle srioSocket_DIO;
     //uint8_t dio_ftype,dio_ttype;

#ifdef DEBUG
     System_printf ("*************************************************************\n");
     System_printf ("******* DIO Socket NRead (Core %d) ********\n", coreNum);
     System_printf ("*************************************************************\n");
#endif
     /*---------------hxl creat socket for doorbell and Swrite to FPGA, bind with the information-------------------------------------------*/
     sockIdx=0;
     /* Reset global counters before next run */
 	srioDioLsuGoodTransfers = 0;
 	srioDioLsuBadTransfers  = 0;
     /* Open DIO SRIO Non-Blocking Socket for Nwrite*/
     srioSocket_DIO = Srio_sockOpen (hSrioDrv, Srio_SocketType_DIO, FALSE);//hxl:打开socket,非阻塞式的
     if (srioSocket_DIO == NULL)
      {
           System_printf ("Error: Unable to open the DIO Nwrite socket \n");
           return -1;
       }
      /* DIO Binding Information: Use 16 bit identifiers and we are bound to the first source id.
      * and we are using 16 bit device identifiers. */
     bindInfo.dio.doorbellValid  =0;// 0;//hxl:modified it for test
     bindInfo.dio.intrRequest    =1; //1;//hxl:modified it for test;是否使能LSU发送完成中断：0——disable
     bindInfo.dio.supInt         = 0;
     bindInfo.dio.xambs          = 0;
     bindInfo.dio.priority       = 2;//0;//hxl:modified 2017-5-31
     bindInfo.dio.outPortID      = OUTPORTID_SRIO;//0;//2;//hxl:port0
     bindInfo.dio.idSize         = 0; //1;//hxl:0b00——8bit ID;0b01——16bit ID
     bindInfo.dio.srcIDMap       = sockIdx;//hxl:选择使用哪个sorceID寄存器
     bindInfo.dio.hopCount       = 0;
     bindInfo.dio.doorbellReg    = 0;
     bindInfo.dio.doorbellBit    = 0;//hxl:表示发送参数完成.0~15bit

    /* Bind the SRIO socket: DIO sockets do not need any binding information. */
     if (Srio_sockBind_DIO (srioSocket_DIO, &bindInfo) < 0)
     {
       System_printf ("Error: Binding the SIO socket failed.\n");
        return -1;
     }
     /*----hxl set the information of dst end--------------------------------*/
     //Populate the DIO Address Information where the data is to be sent.
	to.dio.dstID         = DEVICE_ID2_8BIT;//FPGA ID
	to.dio.ttype         = dio_ttype;
	to.dio.ftype         = dio_ftype;
	/*--hxl--NRead the  paras back from FPGA-----------------------------------------------*/
     {

     	addr=ptr_Buf->addr;
 		//ptr_DataBuffer =(char*) (ptr_Buf->parasarray) + 0x10000000;//hxl:source addr必须转换为全局地址;//addr必须转换为全局地址
     	ptr_DataBuffer =(char*) (ptr_Buf->parasarray);
     	ptr_DataBuffer=(char*)l2_global_address((Uint32)ptr_DataBuffer);

 		for(count=0;count<ptr_Buf->length_DW*BYTESPERDW;count+=SIZE_DIO_PACKET_R)
 		{
			/* Send the DIO Information. */
			to.dio.rapidIOMSB    = 0x0;
			to.dio.rapidIOLSB    =addr;//SRIO_DstBase_LSB;//SRIO_DstBase_LSB;//hxl:FPGA 接收端的地址
			if (Srio_sockSend_DIO (srioSocket_DIO, (void*)ptr_DataBuffer, SIZE_DIO_PACKET_R, (Srio_SockAddrInfo*)&to) < 0)

			{
				System_printf ("Debug(Core %d): DIO Socket send Failed\n", coreNum);
				return -1;
			}
			/* Wait for the interrupt to occur without touching the peripheral. */
			/* Other useful work could be done here such as by invoking a scheduler */
			startTime = TSCL;
			while((! srioLsuIsrServiced) && ((TSCL - startTime) < SRIO_DIO_LSU_ISR_TIMEOUT));
			if (! srioLsuIsrServiced)
			{
				System_printf ("NREAD ISR didn't happen within set time!!!\n");
			//return -1;
			}
			CSL_SRIO_ClearLSUPendingInterrupt (hSrioCSL, 0xFFFFFFFF, 0xFFFFFFFF);
			srioLsuIsrServiced=0;//hxl:全局变量。用于表明中断服务函数已完成
			/* Debug Message: Display ISR count */
			//System_printf ("Debug(Core %d): ISR Count: %d\n", coreNum, srioDbgDioIsrCnt);//hxl记录中断次数
			/* Read the completion code filled by the ISR */
			compCode = 0xFF;
			if (Srio_getSockOpt(srioSocket_DIO, Srio_Opt_DIO_READ_SOCK_COMP_CODE, &compCode, sizeof(uint8_t)) < 0)
			{
			System_printf ("Error: Unable to read the completion code in socket\n");
			return -1;
			}
			/* Was the transfer good. */
			if (compCode == 0)
			{
				srioDioLsuGoodTransfers++;
			}
			else
			{
				srioDioLsuBadTransfers++;
			}
			/* Debug Message: Print error counters */
			//System_printf ("Debug(Core %d): Transfer paras Completion without Errors - %d\n", coreNum, srioDioLsuGoodTransfers);
			//System_printf ("Debug(Core %d): Transfer paras Completion with Errors    - %d\n", coreNum, srioDioLsuBadTransfers);

			if (srioDioLsuBadTransfers > 0)
			{
				System_printf ("Debug(Core %d): DIO Transfer Paras Data NOT completed successfully !!! \n", coreNum);
				return -1;
			  }
#ifdef DEBUG
			System_printf ("Debug(Core %d): DIO NREAD with Interrupts Passed\n", coreNum);
#endif
			addr+=SIZE_DIO_PACKET_R;
			ptr_DataBuffer+=SIZE_DIO_PACKET_R;
		}

     }
     Srio_sockClose_DIO (srioSocket_DIO);
     return 0;
 }





