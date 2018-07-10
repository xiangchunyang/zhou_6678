/**
 *   @file  device_srio.c
 *
 *   @brief   
 *      The 6608 SRIO Device specific code. The SRIO driver calls out
 *      this code to initialize the SRIO IP block. The file is provided as 
 *      a sample configuration and should be modified by customers for 
 *      their own platforms and configurations.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2010 Texas Instruments, Inc.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  \par
*/

/* SRIO Driver Includes. */
#include <ti/drv/srio/srio_types.h>
#include <ti/drv/srio/include/listlib.h>
#include <ti/drv/srio/srio_drv.h>

/* CSL SRIO Functional Layer */
#include <ti/csl/csl_srio.h>
#include <ti/csl/csl_srioAux.h>
#include <ti/csl/csl_srioAuxPhyLayer.h>

/* CSL BootCfg Module */
#include <ti/csl/csl_bootcfg.h>
#include <ti/csl/csl_bootcfgAux.h>

/* CSL Chip Functional Layer */
#include <ti/csl/csl_chip.h>

/* CSL PSC Module */
#include <ti/csl/csl_pscAux.h>

/* QMSS Include */
#include <ti/drv/qmss/qmss_drv.h>
/*hxl added this line*/
#include <ti/csl/cslr_cgem.h>

#include "device_srio_loopback.h" //hxl:added 2017-10-13
/**********************************************************************
 ************************* LOCAL Definitions **************************
 **********************************************************************/

/* These are the GARBAGE queues which are used by the TXU to dump the 
 * descriptor if there is an error instead of recycling the descriptor
 * to the free queue. */
#define GARBAGE_LEN_QUEUE 905
#define GARBAGE_TOUT_QUEUE 906
#define GARBAGE_RETRY_QUEUE 907
#define GARBAGE_TRANS_ERR_QUEUE 908
#define GARBAGE_PROG_QUEUE 909
#define GARBAGE_SSIZE_QUEUE 910

/* SRIO Device Information
 * - 16 bit Device Identifier.
 * - 8 bit Device Identifier.
 * - Vendor Identifier. 
 * - Device Revision. */
#define DEVICE_VENDOR_ID 0x30
#define DEVICE_REVISION 0x0

/* SRIO Assembly Information
 * - Assembly Identifier
 * - Assembly Vendor Identifier. 
 * - Assembly Device Revision. 
 * - Assembly Extension Features */
#define DEVICE_ASSEMBLY_ID 0x0
#define DEVICE_ASSEMBLY_VENDOR_ID 0x30
#define DEVICE_ASSEMBLY_REVISION 0x0
#define DEVICE_ASSEMBLY_INFO 0x0100

/**********************************************************************
 ************************* Extern Definitions *************************
 **********************************************************************/

extern const uint32_t DEVICE_ID1_16BIT;
extern const uint32_t DEVICE_ID1_8BIT;
extern const uint32_t DEVICE_ID2_16BIT;
extern const uint32_t DEVICE_ID2_8BIT;
extern const uint32_t DEVICE_ID3_16BIT;
extern const uint32_t DEVICE_ID3_8BIT_ID;
extern const uint32_t DEVICE_ID4_16BIT;
extern const uint32_t DEVICE_ID4_8BIT_ID;
/*hxl added this*/
CSL_CgemRegs *gpCGEM_regs;

CSL_SrioRegs *gpSRIO_regs = (CSL_SrioRegs *)CSL_SRIO_CONFIG_REGS;

int flag_hasRun = 0;
Qmss_QueueHnd queueHnd0[6] = {-1, -1, -1, -1, -1, -1};
/**********************************************************************
 *********************** DEVICE SRIO FUNCTIONS ***********************
 **********************************************************************/

/** @addtogroup SRIO_DEVICE_API
 @{ */

/**
 *  @b Description
 *  @n  
 *      The function provides the initialization sequence for the SRIO IP
 *      block. This can be modified by customers for their application and
 *      configuration.
 *
 *  @retval
 *      Success     -   0
 *  @retval
 *      Error       -   <0
 */
#pragma CODE_SECTION(SrioDevice_init, ".text:SrioDevice_init");
int32_t SrioDevice_init(void) //hxl:可以根据实际应用需要进行修改；在调用SRIO Driver API时必须先调用该函数
{
    CSL_SrioHandle hSrio;
    int32_t i;
    SRIO_PE_FEATURES peFeatures;
    SRIO_OP_CAR opCar;
    Qmss_QueueHnd queueHnd;
    uint8_t isAllocated;
    uint32_t gargbageQueue[] = {GARBAGE_LEN_QUEUE, GARBAGE_TOUT_QUEUE,
                                GARBAGE_RETRY_QUEUE, GARBAGE_TRANS_ERR_QUEUE,
                                GARBAGE_PROG_QUEUE, GARBAGE_SSIZE_QUEUE};
    /* Get the CSL SRIO Handle. */
    hSrio = CSL_SRIO_Open(0);
    if (hSrio == NULL)
        return -1;

    /* Code to disable SRIO reset isolation */
    if (CSL_PSC_isModuleResetIsolationEnabled(CSL_PSC_LPSC_SRIO))
        CSL_PSC_disableModuleResetIsolation(CSL_PSC_LPSC_SRIO);

    /* Disable the SRIO Global block */
    CSL_SRIO_GlobalDisable(hSrio);

    /* Disable each of the individual SRIO blocks. */
    for (i = 0; i <= 9; i++)
        CSL_SRIO_DisableBlock(hSrio, i);

    /* Set boot complete to be 0; we are not done with the initialization. */
    CSL_SRIO_SetBootComplete(hSrio, 0);

    /* Now enable the SRIO block and all the individual blocks also. */
    CSL_SRIO_GlobalEnable(hSrio); //hxl:全局使能
    for (i = 0; i <= 9; i++)
        CSL_SRIO_EnableBlock(hSrio, i); //hxl:使能SRIO的子模块

    /* Configure SRIO ports to operate in loopback mode. */ //hxl:实际应用中需要更改该field为normal
                                                            /*CSL_SRIO_SetLoopbackMode(hSrio, 0);//hxl:共4个端口,设置寄存器PER_SET_CNTL1中的Loopback [3-0]field
    CSL_SRIO_SetLoopbackMode(hSrio, 1);
    CSL_SRIO_SetLoopbackMode(hSrio, 2);
    CSL_SRIO_SetLoopbackMode(hSrio, 3);*/

    //----------2016-1-5 hxl added the following code--------------
    CSL_SRIO_SetNormalMode(hSrio, 0);
    CSL_SRIO_SetNormalMode(hSrio, 1);
    CSL_SRIO_SetNormalMode(hSrio, 2);
    CSL_SRIO_SetNormalMode(hSrio, 3);
    //-----ended-----2016-1-5 hxl added the following code--------------

    /* Enable Automatic Priority Promotion of response packets. */
    CSL_SRIO_EnableAutomaticPriorityPromotion(hSrio); //hxl:设置寄存器PER_SET_CNTL中的field PROMOTE_DIS

    /* Set the SRIO Prescalar select to operate in the range of 44.7 to 89.5 */
    CSL_SRIO_SetPrescalarSelect(hSrio, 0); //设置寄存器PER_SET_CNTL中的field PRESCALER_SELECT

    /* Unlock the Boot Configuration Kicker */
    CSL_BootCfgUnlockKicker(); //hxl:memory-mapped registers (MMRs),

    /*****************************设置link rate 为1.25G*********************************************************/
    /*-------------hxl modified for 1.25GBPS------------------------------------------*/
    /* Assuming the link rate is 1.25; program the PLL accordingly. */
    /*CSL_BootCfgSetSRIOSERDESConfigPLL (0x241);//hxl:0b0010,0100,0001 ,MPY=8X,想设置为1.25G
	 	CSL_BootCfgSetSRIOSERDESRxConfig (0, 0x004404b5);//hxl:0b0000,0000,0100,0100,0000,0100,1011,0101
	 	CSL_BootCfgSetSRIOSERDESRxConfig (1, 0x004404b5);//hxl:rate——Eighth
	 	CSL_BootCfgSetSRIOSERDESRxConfig (2, 0x004404b5);
	 	CSL_BootCfgSetSRIOSERDESRxConfig (3, 0x004404b5);

	 	// Configure the SRIO SERDES Transmit Configuration. //
	 	CSL_BootCfgSetSRIOSERDESTxConfig (0, 0x001807b5);//hxl:ob0000,0000,0001,1000,0000,0111,1011,0101
	 	CSL_BootCfgSetSRIOSERDESTxConfig (1, 0x001807b5);//hxl:rate——Eighth
	 	CSL_BootCfgSetSRIOSERDESTxConfig (2, 0x001807b5);
	 	CSL_BootCfgSetSRIOSERDESTxConfig (3, 0x001807b5);
	 	*/
    /*------end-------hxl modified for 1.25GBPS------------------------------------------*/
    /***********************设置link rate 为3.125 GBPS*************************************************************/
    // Assuming the link rate is 3125; program the PLL accordingly. // //hxl:对于寄存器：SRIO_SERDES_CFGPLL register
    CSL_BootCfgSetSRIOSERDESConfigPLL(0x229); //hxl:0b0010,0010,1001 ,MPY=5X, 参考时钟为312.M  //hxl:寄存器SRIO_SERDES_CFGPLL

    // Configure the SRIO SERDES Receive Configuration. //
    CSL_BootCfgSetSRIOSERDESRxConfig(0, 0x00440495); //hxl:0b0000,0000,0100,0100,0000,0100,1001,0101,//register:SRIO_SERDES_CFGRX
    CSL_BootCfgSetSRIOSERDESRxConfig(1, 0x00440495); //hxl:rate——half
    CSL_BootCfgSetSRIOSERDESRxConfig(2, 0x00440495);
    CSL_BootCfgSetSRIOSERDESRxConfig(3, 0x00440495);

    // Configure the SRIO SERDES Transmit Configuration. //
    CSL_BootCfgSetSRIOSERDESTxConfig(0, 0x00180795); //hxl:0b0000,0000,0001,1000,0000,0111,1001,0101,//register:SRIO_SERDES_CFGTX
    CSL_BootCfgSetSRIOSERDESTxConfig(1, 0x00180795); //hxl:rate——half
    CSL_BootCfgSetSRIOSERDESTxConfig(2, 0x00180795);
    CSL_BootCfgSetSRIOSERDESTxConfig(3, 0x00180795);

    /***********************设置link rate 为5GBPS*************************************************************/
    /*
	 	CSL_BootCfgSetSRIOSERDESConfigPLL (0x241);//hxl:0b0010,0100,0001 ,MPY=8X,参考时钟为312.M  //hxl:寄存器SRIO_SERDES_CFGPLL
	 	// Configure the SRIO SERDES Receive Configuration. //
	 	CSL_BootCfgSetSRIOSERDESRxConfig (0, 0x00440495);//hxl:0b0000,0000,0100,0100,0000,0100,1001,0101,//register:SRIO_SERDES_CFGRX
	 	CSL_BootCfgSetSRIOSERDESRxConfig (1, 0x00440495);//hxl:rate——half
	 	CSL_BootCfgSetSRIOSERDESRxConfig (2, 0x00440495);
	 	CSL_BootCfgSetSRIOSERDESRxConfig (3, 0x00440495);
	 	// Configure the SRIO SERDES Transmit Configuration. //
	 	CSL_BootCfgSetSRIOSERDESTxConfig (0, 0x00180795);//hxl:0b0000,0000,0001,1000,0000,0111,1001,0101,//register:SRIO_SERDES_CFGTX
	 	CSL_BootCfgSetSRIOSERDESTxConfig (1, 0x00180795);//hxl:rate——half
	 	CSL_BootCfgSetSRIOSERDESTxConfig (2, 0x00180795);
	 	CSL_BootCfgSetSRIOSERDESTxConfig (3, 0x00180795);
	 	*/
    /*********end**************设置link rate 为5GBPS*************************************************************/

#ifndef SIMULATOR_SUPPORT
    /* Loop around till the SERDES PLL is not locked. */
    while (1)
    {
        uint32_t status;

        /* Get the SRIO SERDES Status */
        CSL_BootCfgGetSRIOSERDESStatus(&status);
        if (status & 0x1)
            break;
    }
#endif

    /* Clear the LSU pending interrupts. */
    CSL_SRIO_ClearLSUPendingInterrupt(hSrio, 0xFFFFFFFF, 0xFFFFFFFF);
    CSL_SRIO_ClearDoorbellPendingInterrupt(hSrio, 0, 0xFFFF); //hxl:added
    CSL_SRIO_ClearDoorbellPendingInterrupt(hSrio, 1, 0xFFFF); //hxl:added
    CSL_SRIO_ClearDoorbellPendingInterrupt(hSrio, 2, 0xFFFF); //hxl:added
    CSL_SRIO_ClearDoorbellPendingInterrupt(hSrio, 3, 0xFFFF); //hxl:added
    /* Set the Device Information */
    CSL_SRIO_SetDeviceInfo(hSrio, DEVICE_ID1_16BIT, DEVICE_VENDOR_ID, DEVICE_REVISION);

    /* Set the Assembly Information */
    CSL_SRIO_SetAssemblyInfo(hSrio, DEVICE_ASSEMBLY_ID, DEVICE_ASSEMBLY_VENDOR_ID,
                             DEVICE_ASSEMBLY_REVISION, DEVICE_ASSEMBLY_INFO);

    /* TODO: Configure the processing element features
     *  The SRIO RL file is missing the Re-transmit Suppression Support (Bit6) field definition */
    peFeatures.isBridge = 0;
    peFeatures.isEndpoint = 0;
    peFeatures.isProcessor = 1;
    peFeatures.isSwitch = 0;
    peFeatures.isMultiport = 0;
    peFeatures.isFlowArbiterationSupported = 0;
    peFeatures.isMulticastSupported = 0;
    peFeatures.isExtendedRouteConfigSupported = 0;
    peFeatures.isStandardRouteConfigSupported = 1;
    peFeatures.isFlowControlSupported = 1;
    peFeatures.isCRFSupported = 0;
    peFeatures.isCTLSSupported = 1;
    peFeatures.isExtendedFeaturePtrValid = 1;
    peFeatures.numAddressBitSupported = 1;
    CSL_SRIO_SetProcessingElementFeatures(hSrio, &peFeatures);

    /* Configure the source operation CAR */
    memset((void *)&opCar, 0, sizeof(opCar));
    opCar.portWriteOperationSupport = 1;
    opCar.atomicClearSupport = 1;
    opCar.atomicSetSupport = 1;
    opCar.atomicDecSupport = 1;
    opCar.atomicIncSupport = 1;
    opCar.atomicTestSwapSupport = 1;
    opCar.doorbellSupport = 1;
    opCar.dataMessageSupport = 1;
    opCar.writeResponseSupport = 1;
    opCar.streamWriteSupport = 1;
    opCar.writeSupport = 1;
    opCar.readSupport = 1;
    opCar.dataStreamingSupport = 1;
    CSL_SRIO_SetSourceOperationCAR(hSrio, &opCar);

    /* Configure the destination operation CAR */
    memset((void *)&opCar, 0, sizeof(opCar));
    opCar.portWriteOperationSupport = 1;
    opCar.doorbellSupport = 1;
    opCar.dataMessageSupport = 1;
    opCar.writeResponseSupport = 1;
    opCar.streamWriteSupport = 1;
    opCar.writeSupport = 1;
    opCar.readSupport = 1;
    CSL_SRIO_SetDestOperationCAR(hSrio, &opCar);

    /* Set the 16 bit and 8 bit identifier for the SRIO Device. */
    CSL_SRIO_SetDeviceIDCSR(hSrio, DEVICE_ID1_8BIT, DEVICE_ID1_16BIT);

    /* Enable TLM Base Routing Information for Maintainance Requests & ensure that
     * the BRR's can be used by all the ports. */
    CSL_SRIO_SetTLMPortBaseRoutingInfo(hSrio, 0, 1, 1, 1, 0); //hxl:寄存器TLM_SP(n)_BRR_x_CTL
    CSL_SRIO_SetTLMPortBaseRoutingInfo(hSrio, 0, 2, 1, 1, 0);
    CSL_SRIO_SetTLMPortBaseRoutingInfo(hSrio, 0, 3, 1, 1, 0);
    CSL_SRIO_SetTLMPortBaseRoutingInfo(hSrio, 1, 0, 1, 1, 0);

    /* Configure the Base Routing Register to ensure that all packets matching the 
     * Device Identifier & the Secondary Device Id are admitted. */
    CSL_SRIO_SetTLMPortBaseRoutingPatternMatch(hSrio, 0, 1, DEVICE_ID2_16BIT, 0xFFFF);
    CSL_SRIO_SetTLMPortBaseRoutingPatternMatch(hSrio, 0, 2, DEVICE_ID3_16BIT, 0xFFFF);
    CSL_SRIO_SetTLMPortBaseRoutingPatternMatch(hSrio, 0, 3, DEVICE_ID4_16BIT, 0xFFFF);
    CSL_SRIO_SetTLMPortBaseRoutingPatternMatch(hSrio, 1, 0, DEVICE_ID2_8BIT, 0xFF);

    if (queueHnd0[0] < 0) //hxl:如果队列还没有打开
    {
        /* We need to open the Garbage collection queues in the QMSS. This is done to ensure that
		 * these queues are not opened by another system entity. */
        for (i = 0; i < 6; i++)
        {
            /* Open the Garabage queues */
            queueHnd = Qmss_queueOpen(Qmss_QueueType_GENERAL_PURPOSE_QUEUE, gargbageQueue[i], &isAllocated);
            if (queueHnd < 0)
                return -1;

            /* Make sure the queue has not been opened already; we dont the queues to be shared by some other
			 * entity in the system. */
            if (isAllocated > 1) //hxl:程序运行后第二次调用该函数 SrioDevice_init 时，这一步出错。
                return -1;
            queueHnd0[i] = queueHnd; //hxl:记录garage handle
        }

        /* Set the Transmit Garbage Collection Information. */
        CSL_SRIO_SetTxGarbageCollectionInfo(hSrio, GARBAGE_LEN_QUEUE, GARBAGE_TOUT_QUEUE,
                                            GARBAGE_RETRY_QUEUE, GARBAGE_TRANS_ERR_QUEUE,
                                            GARBAGE_PROG_QUEUE, GARBAGE_SSIZE_QUEUE);
    }
    else //hxl:如果该函数调用过
    {
        for (i = 0; i < 6; i++)
        {
            Qmss_queueClose(queueHnd0[i]);
        }
        /* We need to open the Garbage collection queues in the QMSS. This is done to ensure that
		 * these queues are not opened by another system entity. */
        for (i = 0; i < 6; i++)
        {
            /* Open the Garabage queues */
            queueHnd = Qmss_queueOpen(Qmss_QueueType_GENERAL_PURPOSE_QUEUE, gargbageQueue[i], &isAllocated);
            if (queueHnd < 0)
                return -1;

            /* Make sure the queue has not been opened already; we dont the queues to be shared by some other
			 * entity in the system. */
            if (isAllocated > 1) //hxl:程序运行后第二次调用该函数 SrioDevice_init 时，这一步出错。
                return -1;
            queueHnd0[i] = queueHnd; //hxl:记录garage handle
        }

        /* Set the Transmit Garbage Collection Information. */
        CSL_SRIO_SetTxGarbageCollectionInfo(hSrio, GARBAGE_LEN_QUEUE, GARBAGE_TOUT_QUEUE,
                                            GARBAGE_RETRY_QUEUE, GARBAGE_TRANS_ERR_QUEUE,
                                            GARBAGE_PROG_QUEUE, GARBAGE_SSIZE_QUEUE);
    }
    /* Set the Host Device Identifier. */
    CSL_SRIO_SetHostDeviceID(hSrio, DEVICE_ID1_16BIT);

    /* Configure the component tag CSR */
    CSL_SRIO_SetCompTagCSR(hSrio, 0x00000000);

    /* Configure the PLM for all the ports. */
    for (i = 0; i < 4; i++)
    {
        /* Set the PLM Port Silence Timer. */
        CSL_SRIO_SetPLMPortSilenceTimer(hSrio, i, 0x2); //hxl:silence Timer

        /* TODO: We need to ensure that the Port 0 is configured to support both
         * the 2x and 4x modes. The Port Width field is read only. So here we simply
         * ensure that the Input and Output ports are enabled. */
        CSL_SRIO_EnableInputPort(hSrio, i); //hxl:设置寄存器(SPn_CTL,CSR)
        CSL_SRIO_EnableOutputPort(hSrio, i);

        /* Set the PLM Port Discovery Timer. */
        CSL_SRIO_SetPLMPortDiscoveryTimer(hSrio, i, 0x2);

        /* Reset the Port Write Reception capture. */
        CSL_SRIO_SetPortWriteReceptionCapture(hSrio, i, 0x0);
    }

    /* Set the Port link timeout CSR */
    CSL_SRIO_SetPortLinkTimeoutCSR(hSrio, 0x000FFF); //hxl:SP_LT_CTL,CSR寄存器

    /* Set the Port General CSR: Only executing as Master Enable */
    CSL_SRIO_SetPortGeneralCSR(hSrio, 0, 1, 0); //hxl:SP_GEN_CTL CSR寄存器 P3-94

    /* Clear the sticky register bits. */
    CSL_SRIO_SetLLMResetControl(hSrio, 1); //hxl:设置寄存器REG_RST_CTL，配置寄存器的复位行为

    /* Set the device id to be 0 for the Maintenance Port-Write operation 
     * to report errors to a system host. */
    CSL_SRIO_SetPortWriteDeviceId(hSrio, 0x0, 0x0, 0x0); //hxl:寄存器PW_TGT_ID，P3-108

    /* Set the Data Streaming MTU */
    //CSL_SRIO_SetDataStreamingMTU (hSrio, 64);//hxl:设置寄存器：DS_LL_CTL，CSR;hxl:64——256B，255——1024B
    CSL_SRIO_SetDataStreamingMTU(hSrio, 255);
    /* Configure the path mode for the ports. */ //hxl:路径配置
    for (i = 0; i < 4; i++)
        CSL_SRIO_SetPLMPortPathControlMode(hSrio, i, 0); //hxl:路径模式mode为0,1X；设置寄存器PLM_SP_PATH_CTL

    /* Set the LLM Port IP Prescalar. */
    CSL_SRIO_SetLLMPortIPPrescalar(hSrio, 0x21); //hxl:设置寄存器PRESCALAR_SRV_CLK，定义时钟频率的缩放系数(为ip_clk)

    /*---2016-7-21 hxl added it for little endian mode B swap--*/
    CSL_SRIO_SetLSUSwappingMode(hSrio, 0x03); //hxl:注意
    CSL_SRIO_SetMAUSwappingMode(hSrio, 0x03); //for fpga,0x03;for DSP,0x0
    /*-end--2016-7-21 hxl added it for little endian mode B swap--*/
    /* Enable the peripheral. */
    CSL_SRIO_EnablePeripheral(hSrio); //hxl:设置寄存器：PCR(peripheral control register),SRIO从reset至normal最后要使能的field，P3-41

    /* Configuration has been completed. */
    CSL_SRIO_SetBootComplete(hSrio, 1); //hxl：设置寄存器：PER_SET_CNTL，p3-43；禁止写只读寄存器

    //#ifndef SIMULATOR_SUPPORT
    /* This code checks if the ports are operational or not. The functionality is not supported 
     * on the simulator. */
    //for(i = 0; i < 4; i++)//hxl:modified it for port0 test only
    //    for(i = OUTPORTID_SRIO; i < (OUTPORTID_SRIO+1); i++)//hxl:仅检测使用的一个端口
    //            while (CSL_SRIO_IsPortOk (hSrio, i) != TRUE);//hxl:查询寄存器SPn_ERR_STAT，port error and status CSR
    //#endif

    /* Set all the queues 0 to operate at the same priority level and to send packets onto Port 0 */
    for (i = 0; i < 16; i++)
        CSL_SRIO_SetTxQueueSchedInfo(hSrio, i, 0, 0);
    /*
    for (i =0 ; i < 16; i++)
            CSL_SRIO_SetTxQueueSchedInfo(hSrio, i,2, 0);
            */

    /* Set the Doorbell route to determine which routing table is to be used 
     * This configuration implies that the Interrupt Routing Table is configured as 
     * follows:-
     *                            General line  DOORBELL Dedicated line
     *  Interrupt Destination 0 - INTDST 0       	//INTDST 16
     *  Interrupt Destination 1 - INTDST 1       	//INTDST 17
     *  Interrupt Destination 2 - INTDST 2    		//INTDST 18
     *  Interrupt Destination 3 - INTDST 3    		//INTDST 19
     */
    CSL_SRIO_SetDoorbellRoute(hSrio, 1); //使用DOORBELL通用中断线,not dedicated interrupt,设置寄存器INTERRUPT_CTL

    /* Route the Doorbell interrupts. //hxl deleted this section
     *  Doorbell Register 0 - All 16 Doorbits are routed to Interrupt Destination 0. 
     *  Doorbell Register 1 - All 16 Doorbits are routed to Interrupt Destination 1. 
     *  Doorbell Register 2 - All 16 Doorbits are routed to Interrupt Destination 2. 
     *  Doorbell Register 3 - All 16 Doorbits are routed to Interrupt Destination 3. */
    CSL_SRIO_RouteDoorbellInterrupts(hSrio, 0, 0, 0); //hxl:DOORBELL寄存器0的第0 bit路由至dst0,通用中断线的DST0.hxl added:DOORBELL0_0bit---INTDST 0;设置寄存器doorbell中断路由寄存器DOORBELLn_ICRR

    // Route LSU0 ICR0 to INTDST1//hxl added it here
    CSL_SRIO_RouteLSUInterrupts(hSrio, 0, 1); //hxl:LSU:Load/Store Unit
    // Route LSU0 ICR1 to INTDST1
    CSL_SRIO_RouteLSUInterrupts(hSrio, 1, 1);
    // Route LSU0 ICR2 to INTDST1
    CSL_SRIO_RouteLSUInterrupts(hSrio, 2, 1);

    /* Initialization has been completed. */
    return 0;
}

/**
@}
*/
