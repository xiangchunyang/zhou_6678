/*
 * idleFunctions_appuser.c
 *
 *  Created on: 2017-6-15
 *      Author: Administrator
 */
#include <ti/csl/csl_chip.h> //hxl:for using stamp timer
/* CSL SRIO Functional Layer */
#include <ti/csl/csl_srio.h>
//#include <ti/csl/csl_srioAux.h>
#include <ti/csl/csl_srioAuxPhyLayer.h>
#include "task_TCP_Svr.h"
#include "idleFunctions_appUser.h"
#include "device_srio_loopback.h"

int flags_portNotOK[4] = {0}; //hxl:SRIO ports ok
int flag_hSrioExcp = 0;		  //hxl:CSL SRIO Handle Exception flag

/*2017-6-15 add an idle function to check SRIO port*/
/*void   CheckSRIOPort_idle_task(void)
 {

 }
*/
void CheckSRIOPortOK(void)
{

	CSL_SrioHandle hSrio;
	int i, j, startTime;
	// Get the CSL SRIO Handle. //
	hSrio = CSL_SRIO_Open(0);
	if (hSrio == NULL)
		flag_hSrioExcp = 1; //hxl:CSL SRIO Handle Exception happen
#ifndef SIMULATOR_SUPPORT
	// This code checks if the ports are operational or not. The functionality is not supported
	// on the simulator. //
	for (j = 0; j < 10; j++) //hxl:多次检测，防止拔线测试时的连接抖动
	{
		//for(i = 0; i < 4; i++)//hxl:modified it for port0 test only
		for (i = OUTPORTID_SRIO; i < OUTPORTID_SRIO + 1; i++)
		{
			if (CSL_SRIO_IsPortOk(hSrio, i) != TRUE) //hxl:?é???????÷SPn_ERR_STAT??port error and status CSR
				flags_portNotOK[i] = 1;				 //hxl:port i ?ì??
		}
		startTime = TSCL;
		while ((TSCL - startTime) < SRIO_PORT_CHECKEXCEPT_TIME)
			;
	}
//hxl:?????±???????°?????ì??
#endif
}
