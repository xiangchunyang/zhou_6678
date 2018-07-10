/*
*******************hxl added this file******************
*/

#include <ti/csl/csl_xmc.h>
#include <ti/csl/cslr_xmc.h>
#include <ti/csl/csl_xmcAux.h>
#include <ti/sysbios/family/c66/Cache.h> //2016-9-2 hxl added this file
#include <stdio.h>						 //2016-9-2 hxl added this file

static char *ptr_MSM = (char *)0x0c000000; //hxl:MSM的起始地址
/**********************************************************************
 ************************* LOCAL Definitions **************************
 **********************************************************************/
/*Not applicable now*/

#pragma CODE_SECTION(MSM_init, ".text:MSM_init");

void MSM_init(void)
{
	UInt32 val_MAR;
	//--hxl:设置MSM对应的MAR寄存器中的PFX bit为0，关掉XMC的预取功能---//
	Cache_setMar((void *)ptr_MSM, 0x400000, 0); //set MSM to non prefetchable though XMC
	val_MAR = Cache_getMar((void *)ptr_MSM);	// Gets the MAR register for the specified base address(Ptr baseAddr);
	if ((val_MAR & (1 << 3)) == 1)
	{
		printf("fail to disable MSM prefetchability!");
	}
}
