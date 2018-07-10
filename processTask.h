/*
 * FFTVaribles.h
 *
 *  Created on: 2015-8-28
 *      Author: Administrator
 */

#ifndef FFTVARIBLES_H_
#define FFTVARIBLES_H_

/*--------------与0相位滤波有关的变量-----------------------------*/
#define NX ((TOTALPOINTS - 2) / 4 * 4 - 1) //nx=nh+nr-1.若nr与nh均为4的倍数，则要求NX对4取余为3;受调用的滤波库函数的限制;同时，TOTALPOINTS为总回波数据一根线点数，前2个点为帧号和线号，不参与滤波。
#define NUM_FIRINDEX 2048				   //hxl:滤波器索引表的元素个数
#define NUM_FIRCOEF 128					   //hxl:每个滤波器组的阶数
#define LEN_FIR_DW 65					   //hxl:每个滤波器组占据的总字节数
#define ADDR_INDEX_FIR 0x00000200		   //hxl:DW偏移
#define ADDR_FIR0 0x00000000			   //hxl:DW偏移
#define ADDR_FIR1 0x00000041			   //hxl:DW偏移
#define ADDR_FIR2 0x00000082			   //hxl:DW偏移
#define ADDR_FIR3 0x000000C3			   //hxl:DW偏移

typedef enum
{
	CentralFreq_5M = 0,
	CentralFreq_2p5M,
	CentralFreq_7p5M
} BANDFILTERBAND;

typedef struct
{

	const float MaxFirCoeff;			//4 Bytes.
	unsigned int l_firCoeffs;			//hxl:表示系数数组的长度;//4 Bytes
	const float firCoeffs[NUM_FIRCOEF]; //hxl:滤波器系数数组,4 Bytes*128

} BANDFILTER;

//---------hxl:FFT算法相关的变量-------------------------------------------------------------------------------------
/* Global definitions */
/* Number of samples for which FFT needs to be calculated */

extern void gen_twiddle_fft_sp_hxl();
extern void gen_twiddle_fft_sp_ifft();

extern void zeroPhaseFilter_F(const short *ptr, BANDFILTER BandFilter);
extern void ChooseBandFilter(BANDFILTERBAND centralFreq, BANDFILTER *ptr_BandFilter);
extern unsigned fir_addrToIndex(unsigned addr);
/*-------hxl-------声明外部函数-----------------------------*/
//extern float r[2*TOTALPOINTS];
extern float w_sp[2 * TOTALPOINTS];
extern float w_spi[2 * TOTALPOINTS];
//extern short   Hilbert_y[8][TOTALPOINTS];
//extern short   Hilbert_y[8][TOTALPOINTS];
extern volatile short ready_x[7][Num_line * Point_image];
extern volatile short ready_y[7][Num_line * Point_image];

//extern float r[TOTALPOINTS];
extern BANDFILTERBAND centralFreq;
extern BANDFILTER firs[4];
extern BANDFILTER firs_native[4];
extern Uint8 index_firs_native[NUM_FIRINDEX];
extern Uint8 index_firs[NUM_FIRINDEX];
extern unsigned addrs_fir[5];
#endif /* FFTVARIBLES_H_ */
