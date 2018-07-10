/*
 * SRIO_TransmitToFPGA.h
 *
 *  Created on: 2016-12-7
 *      Author: Administrator
 */

#ifndef SRIO_TRANSMITTOFPGA_H_
#define SRIO_TRANSMITTOFPGA_H_




/*------hxl:macro about parameters from PC----------------------------------*/

#define SRIO_DstBase_LSB 0x0c010000    //0x10000





/*----function declaration---------------------------*/
extern Int32 dioSockets_doWr(Srio_DrvHandle hSrioDrv,uint8_t dio_ftype, uint8_t dio_ttype,volatile PARASBUF0* ptr_Buf);
extern Int32 dioSockets_doR(Srio_DrvHandle hSrioDrv, uint8_t dio_ftype, uint8_t dio_ttype,volatile PARASBUF0* ptr_Buf);


#endif /* SRIO_TRANSMITTOFPGA_H_ */
