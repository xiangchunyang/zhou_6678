/*
 * FFTVaribles.h
 *
 *  Created on: 2015-8-28
 *      Author: Administrator
 */

#ifndef FFTVARIBLES_H_
#define FFTVARIBLES_H_

/*--------------��0��λ�˲��йصı���-----------------------------*/
#define NX ((TOTALPOINTS - 2) / 4 * 4 - 1) //nx=nh+nr-1.��nr��nh��Ϊ4�ı�������Ҫ��NX��4ȡ��Ϊ3;�ܵ��õ��˲��⺯��������;ͬʱ��TOTALPOINTSΪ�ܻز�����һ���ߵ�����ǰ2����Ϊ֡�ź��ߺţ��������˲���
#define NUM_FIRINDEX 2048				   //hxl:�˲����������Ԫ�ظ���
#define NUM_FIRCOEF 128					   //hxl:ÿ���˲�����Ľ���
#define LEN_FIR_DW 65					   //hxl:ÿ���˲�����ռ�ݵ����ֽ���
#define ADDR_INDEX_FIR 0x00000200		   //hxl:DWƫ��
#define ADDR_FIR0 0x00000000			   //hxl:DWƫ��
#define ADDR_FIR1 0x00000041			   //hxl:DWƫ��
#define ADDR_FIR2 0x00000082			   //hxl:DWƫ��
#define ADDR_FIR3 0x000000C3			   //hxl:DWƫ��

typedef enum
{
	CentralFreq_5M = 0,
	CentralFreq_2p5M,
	CentralFreq_7p5M
} BANDFILTERBAND;

typedef struct
{

	const float MaxFirCoeff;			//4 Bytes.
	unsigned int l_firCoeffs;			//hxl:��ʾϵ������ĳ���;//4 Bytes
	const float firCoeffs[NUM_FIRCOEF]; //hxl:�˲���ϵ������,4 Bytes*128

} BANDFILTER;

//---------hxl:FFT�㷨��صı���-------------------------------------------------------------------------------------
/* Global definitions */
/* Number of samples for which FFT needs to be calculated */

extern void gen_twiddle_fft_sp_hxl();
extern void gen_twiddle_fft_sp_ifft();

extern void zeroPhaseFilter_F(const short *ptr, BANDFILTER BandFilter);
extern void ChooseBandFilter(BANDFILTERBAND centralFreq, BANDFILTER *ptr_BandFilter);
extern unsigned fir_addrToIndex(unsigned addr);
/*-------hxl-------�����ⲿ����-----------------------------*/
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
