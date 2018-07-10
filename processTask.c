#include <stdio.h>
#include <xdc/runtime/System.h>		  //试一试 hahahhahahhhhhh   uu额u额uu
#include <ti/sysbios/hal/Hwi.h>		  //zailaishiyixiaggggg
#include <ti/sysbios/knl/Task.h>	  //hxl added
#include <ti/sysbios/knl/Semaphore.h> // hxl
//#include <xdc/runtime/Memory.h>//hxl added
#include <xdc/runtime/Error.h> //hxl added
/*libs provided by ti*/
#include <math.h>				//FFT所需
#include <ti/dsplib/dsplib.h>   //hxl added
#include <ti/mathlib/mathlib.h> //数学函数库 sin，cos等
#include "App_common.h"
/*------------与fft变换有关的头文件-----------------------*/
#include "SRIO_loopbackDioIsr.h" //hxl added
#include "client.h"				 ////2016-8-17:hxl: added
#include "processTask.h"		 ////2016-8-17:hxl: added
/*=====hxl--global vars===============================================================================*/
unsigned addrs_fir[5] = {ADDR_FIR0, ADDR_FIR1, ADDR_FIR2, ADDR_FIR3, ADDR_INDEX_FIR};

//---------------测试所使用的文件数据存放的数组-----
#pragma DATA_SECTION(afterProcsData, ".sharedVar1") //用于模拟处理后的数据存放的数组
#pragma DATA_ALIGN(afterProcsData, 8)				//hxl:指定data0变量的地址放在128字节对齐的位置上，即其地址的低7位为0；
short afterProcsData[POINTSAFTERPRO] = {0};			//hxl:int 型4个字节
#pragma DATA_SECTION(firs_native, ".filterVars1");  //hxl:align double word
#pragma DATA_ALIGN(firs_native, 8);
/*--------------与0相位滤波有关的变量-------用于预存储的滤波器系数和变量------用于备份-------------------------------------------------------------------------------------------------*/
BANDFILTER firs_native[4] =
	{

		{
			/*****************34阶fir**中心频率5.5M*****1M to 10M BF********************************************************/
			0.3552,
			34,
			{
				-0.036698,
				0.011505,
				-0.012700,
				-0.076229,
				-0.116941,
				-0.073673,
				-0.007693,
				-0.040904,
				-0.156127,
				-0.181211,
				-0.052184,
				0.027986,
				-0.140540,
				-0.375683,
				-0.236647,
				0.387454,
				1.000000,
				1.000000,
				0.387454,
				-0.236647,
				-0.375683,
				-0.140540,
				0.027986,
				-0.052184,
				-0.181211,
				-0.156127,
				-0.040904,
				-0.007693,
				-0.073673,
				-0.116941,
				-0.076229,
				-0.012700,
				0.011505,
				-0.036698,
			},

		},
		{
			/*****************65阶fir**中心频率2.5M*************************************************************/
			0.1177,
			65,
			{
				//hxl:matlab 直接生成的系数，没有归一化.
				0.019407,
				-0.004205,
				0.002338,
				0.008610,
				0.012078,
				0.011570,
				0.007386,
				0.000853,
				-0.006222,
				-0.012137,
				-0.015637,
				-0.016139,
				-0.013775,
				-0.009355,
				-0.004173,
				0.000142,
				0.001918,
				-0.000203,
				-0.006836,
				-0.017733,
				-0.031523,
				-0.045815,
				-0.057652,
				-0.063866,
				-0.061904,
				-0.050299,
				-0.029224,
				-0.000523,
				0.032351,
				0.064984,
				0.092639,
				0.111156,
				0.117660,
				0.111156,
				0.092639,
				0.064984,
				0.032351,
				-0.000523,
				-0.029224,
				-0.050299,
				-0.061904,
				-0.063866,
				-0.057652,
				-0.045815,
				-0.031523,
				-0.017733,
				-0.006836,
				-0.000203,
				0.001918,
				0.000142,
				-0.004173,
				-0.009355,
				-0.013775,
				-0.016139,
				-0.015637,
				-0.012137,
				-0.006222,
				0.000853,
				0.007386,
				0.011570,
				0.012078,
				0.008610,
				0.002338,
				-0.004205,
				0.019407,
			},
		},

		{
			/*****************67阶fir**中心频率3.5M******************************************************************/
			0.1520,
			67,
			{
				//hxl:matlab 直接生成的系数，没有归一化
				-0.000744,
				0.000101,
				-0.000386,
				-0.001335,
				-0.002721,
				-0.004133,
				-0.004745,
				-0.003570,
				0.000075,
				0.006063,
				0.013176,
				0.019272,
				0.021959,
				0.019581,
				0.012129,
				0.001603,
				-0.008471,
				-0.014349,
				-0.013745,
				-0.007191,
				0.001639,
				0.006994,
				0.003293,
				-0.012229,
				-0.037573,
				-0.065683,
				-0.086233,
				-0.089051,
				-0.068037,
				-0.024080,
				0.034272,
				0.092857,
				0.136113,
				0.152032,
				0.136113,
				0.092857,
				0.034272,
				-0.024080,
				-0.068037,
				-0.089051,
				-0.086233,
				-0.065683,
				-0.037573,
				-0.012229,
				0.003293,
				0.006994,
				0.001639,
				-0.007191,
				-0.013745,
				-0.014349,
				-0.008471,
				0.001603,
				0.012129,
				0.019581,
				0.021959,
				0.019272,
				0.013176,
				0.006063,
				0.000075,
				-0.003570,
				-0.004745,
				-0.004133,
				-0.002721,
				-0.001335,
				-0.000386,
				0.000101,
				-0.000744,
			},
		},

		{/*****************64阶fir**中心频率5M***********************************************************/
		 0.1887,
		 64,
		 {
			 //hxl:matlab 直接生成的系数，没有归一化.中心频率5M Hz
			 0.008156,
			 -0.013248,
			 0.006752,
			 0.008259,
			 -0.000784,
			 -0.012461,
			 -0.020121,
			 -0.019569,
			 -0.011143,
			 -0.000263,
			 0.005692,
			 0.003093,
			 -0.004379,
			 -0.007829,
			 -0.000224,
			 0.016915,
			 0.032991,
			 0.035887,
			 0.021841,
			 0.000083,
			 -0.013145,
			 -0.007722,
			 0.010100,
			 0.019500,
			 0.000082,
			 -0.050037,
			 -0.106706,
			 -0.131095,
			 -0.094394,
			 -0.000086,
			 0.112647,
			 0.188673,
			 0.188673,
			 0.112647,
			 -0.000086,
			 -0.094394,
			 -0.131095,
			 -0.106706,
			 -0.050037,
			 0.000082,
			 0.019500,
			 0.010100,
			 -0.007722,
			 -0.013145,
			 0.000083,
			 0.021841,
			 0.035887,
			 0.032991,
			 0.016915,
			 -0.000224,
			 -0.007829,
			 -0.004379,
			 0.003093,
			 0.005692,
			 -0.000263,
			 -0.011143,
			 -0.019569,
			 -0.020121,
			 -0.012461,
			 -0.000784,
			 0.008259,
			 0.006752,
			 -0.013248,
			 0.008156,
		 }},
		/*****************66阶fir**中心频率7.5M******************************************************************/
		/*
	{

		0.2115,
		66,
		{//hxl:matlab 直接生成的系数，没有归一化
			0.003231,-0.006375,0.018294,0.014360,0.001046,-0.009826,-0.009364,-0.002408,
			-0.002572,-0.011213,-0.012779,0.002405,0.020088,0.019050,0.001873,-0.007210,
			0.001481,0.006150,-0.013636,-0.039572,-0.033468,0.006640,0.035849,0.023049,
			-0.000072,0.017437,0.059651,0.044590,-0.063647,-0.173318,-0.145666,0.034788,
			0.211544,0.211544,0.034788,-0.145666,-0.173318,-0.063647,0.044590,0.059651,
			0.017437,-0.000072,0.023049,0.035849,0.006640,-0.033468,-0.039572,-0.013636,
			0.006150,0.001481,-0.007210,0.001873,0.019050,0.020088,0.002405,-0.012779,
			-0.011213,-0.002572,-0.002408,-0.009364,-0.009826,0.001046,0.014360,0.018294,
			-0.006375,0.003231,
		},
	},*/
};
#pragma DATA_SECTION(index_firs_native, ".filterVars1"); //hxl:align double word
Uint8 index_firs_native[NUM_FIRINDEX] = {0};			 //hxl:每个元素值表示选择的滤波器序号，支持多达2048根线独立选择不同的滤波器
#pragma DATA_ALIGN(index_firs_native, 2);				 //hxl:必须保证至少按照字节对齐
/*--------------与0相位滤波有关的变量-------用于实时引用的滤波器系数和变量-------------------------------------------------------------------------------------------------------*/
#pragma DATA_SECTION(firs, ".filterVars"); //hxl:align double word
#pragma DATA_ALIGN(firs, 8);
BANDFILTER firs[4];
#pragma DATA_SECTION(index_firs, ".filterVars"); //hxl:align double word
Uint8 index_firs[NUM_FIRINDEX] = {0};			 //hxl:每个元素值表示选择的滤波器序号，支持多达2048根线独立选择不同的滤波器
#pragma DATA_ALIGN(index_firs, 2);				 //hxl:必须保证至少按照字节对齐
/*----------------------------------------------------------------------------------------------------------------------------*/

BANDFILTERBAND centralFreq = CentralFreq_5M;
//---准备滤波器所需的存储变量------------

#pragma DATA_SECTION(x, ".filterVars"); //hxl:align double word
#pragma DATA_ALIGN(x, 8);
//short x[NX*sizeof(short int)];
//float x[NX]={0};
float x[2 * TOTALPOINTS] = {0};

#pragma DATA_SECTION(r, ".filterVars");
#pragma DATA_ALIGN(r, 8);
//short r[NX*sizeof(short int)];
float r[2 * TOTALPOINTS];

short int nh, nr; //滤波所需的变量
unsigned long long timet;
/*-------end-------与0相位滤波有关的变量-----------------------------*/

//------------------------------------------------------------------------------------------------
/*
#pragma DATA_SECTION(xw, ".FFT")//DDR3中
#pragma DATA_ALIGN(xw, 8);
float   xw [2*TOTALPOINTS];

#pragma DATA_SECTION(rw, ".FFT")//DDR3中
#pragma DATA_ALIGN(rw, 8);
float   rw [2*TOTALPOINTS];
*/

#pragma DATA_SECTION(Hilbert_x, ".shared") //DDR3中
#pragma DATA_ALIGN(Hilbert_x, 8);
short Hilbert_x[8][TOTALPOINTS];

#pragma DATA_SECTION(Hilbert_y, ".shared") //DDR3中
#pragma DATA_ALIGN(Hilbert_y, 8);
short Hilbert_y[8][TOTALPOINTS];

#pragma DATA_SECTION(ready_x, ".FFTVars") //DDR3中
#pragma DATA_ALIGN(ready_x, 8);
volatile short ready_x[7][Num_line * Point_image];

#pragma DATA_SECTION(ready_y, ".FFTVars") //DDR3中
#pragma DATA_ALIGN(ready_y, 8);
volatile short ready_y[7][Num_line * Point_image];

#pragma DATA_SECTION(w_sp, ".shared") //DDR3中
//#pragma DATA_SECTION(w_sp, ".FFT")//DDR3中
#pragma DATA_ALIGN(w_sp, 8);
float w_sp[2 * TOTALPOINTS];

#pragma DATA_SECTION(w_spi, ".shared") //DDR3中
//#pragma DATA_SECTION(w_spi, ".FFT")//DDR3中
#pragma DATA_ALIGN(w_spi, 8);
float w_spi[2 * TOTALPOINTS];

#pragma DATA_SECTION(brev, ".sharedVar") //DDR3中
unsigned char brev[64] = {
	0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
	0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
	0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
	0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
	0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
	0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
	0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
	0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f};

//-------------------------------------------------------------------------------------------------------

/***************************************************************************************************************************/
/* 对原始数据实现0相位带通滤波；中心频率5MHz
 * 滤波器系数和输入均为float型
 * 存在的问题：存在较明显的量化误差
 * 函数输入：原始数据为short型。
 * 注意：中间变量x和r放置于DDR中时，函数DSPF_sp_fir_r2()的运行出现异常
 * */

/***************************************************************************************************************************/
void zeroPhaseFilter_F(const short *ptr, BANDFILTER BandFilter)
{
	int j;				 //hxl：测试信息长度 added
	float tmp, maxCoeff; //data0[TestSize];//hxl:int 型4个字节
	int nh, nr;
	//short (*ptr_data0)[TOTALPOINTS]=afterData;
	//float   x[4096];
	//float   r[4096];
	/******************************0相位带通滤波处理*******************************************************/
	//---准备滤波器所需的变量------------
	//nh=sizeof(fir_coeffs)/sizeof(short int);
	//nr=NX+1-nh;//hxl:输出信号点数与输入信号点数及滤波器阶数的关系,nr%4==0
	nh = BandFilter.l_firCoeffs;
	maxCoeff = BandFilter.MaxFirCoeff;
	nr = NX + 1 - nh;

	//hxl:将待处理的数据copy出来
	for (j = 0; j < NX * sizeof(short); j++)
	{
		x[j] = (float)(ptr[j]);
	}
	//1.第一次滤波
	DSPF_sp_fir_r2(x, (const float *)(BandFilter.firCoeffs), r, nh, nr); //单次滤波;The code is interruptible.
	//第一次滤波结果:前方补零(nh-1个)，然后左右翻转,放入x中
	for (j = 0; j < nr; j++)
	{
		x[nr - 1 - j] = r[j] * maxCoeff; //hxl:滤波器幅度校正。与CCS中滤波的实现过程有关
	}
	for (j = nr; j < NX; j++)
	{
		x[j] = 0; //补零(nh-1个)
	}
	//第二次滤波的有关长度-----------------------------
	//nx:第二次滤波输入信号的长度(经补零后，与原滤波输入信号长度相同)
	//nr:输出信号点数与输入信号点数及滤波器阶数的关系,nr%4==0
	//2.第二次滤波，Input：x;滤波器系数：fir_coeffs；输出：r
	//第二次带通滤波的输入信号是r中的信号，长度为nr;nr%4==0
	DSPF_sp_fir_r2(x, (const float *)(BandFilter.firCoeffs), r, nh, nr); //第二次滤波，输出置于r2中
	//第二次滤波结果前方需要补零(nh-1个)，然后左右翻转
	for (j = 0; j < nr / 2; j++)
	{
		tmp = r[j] * maxCoeff; //hxl:滤波器幅度校正。与CCS中滤波的实现过程有关
		r[j] = r[nr - 1 - j] * maxCoeff;
		r[nr - 1 - j] = tmp; //第二次滤波输出结果左右翻转
	}
	for (j = nr; j < NX; j++)
	{
		r[j] = 0; //第二次滤波结果前方需要补零(nh-1个),
	}
	//hxl:将滤波结果补零至与原输入信号等长，TotalPoints. r2_f【nr，nx】,
	for (j = NX; j < TOTALPOINTS; j++)
	{
		r[j] = 0; //第二次滤波结果前方需要补零(nh-1个),//hxl:注意最终输出结果为浮点型
	}
	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// float SigMag;
	//SigMag = 1;//幅度调整scale
	for (j = 0; j < TOTALPOINTS; j++)
	{
		x[2 * j] = r[j];
		//x[2*j]  = SigMag*r[j];
		x[2 * j + 1] = (float)0.0;
	}
	//	    gen_twiddle_fft_sp_hxl (w_sp, TOTALPOINTS);
	// ----Call FFT routine -----------//
	//输入点数必须是2的次方
	DSPF_sp_fftSPxSP(TOTALPOINTS, x, w_sp, r, brev, 2, 0, TOTALPOINTS); //16384%4!=0,第6个参数设为2,否则设为4;
	for (j = TOTALPOINTS / 2 + 1; j < TOTALPOINTS; j++)					//取FFT的正半轴频率成分,福频率部分置零
	{
		r[2 * j] = (float)0.0;
		r[2 * j + 1] = (float)0.0;
	}
	for (j = 1; j <= TOTALPOINTS / 2; j++) //取FFT的正半轴频率成分X2
	{
		tmp = r[2 * j] * 2;
		r[2 * j] = tmp;
		tmp = r[2 * j + 1] * 2;
		r[2 * j + 1] = tmp;
	}
	//gen_twiddle_fft_sp_ifft (w_spi, TOTALPOINTS);
	DSPF_sp_ifftSPxSP(TOTALPOINTS, r, w_spi, x, brev, 2, 0, TOTALPOINTS); //16384%4!=0,第6个参数设为2,否则设为4;
	/*	    for (j=0; j <TOTALPOINTS; j++)
	        {
	        Hilbert_x[coreNum][j] = (short)x[2*j];
	        Hilbert_y[coreNum][j] = (short)x[2*j+1];

	         }
       */
	//-----------------------------------------------------------------------------------------------------------
	//-----------------------------------TFM---------------------------------------------------------------------
	// int M = 1
	//  int N = 3;   //M表示发射阵元，N表示接收阵元
	int k;
	int distance;
	for (j = 0; j < Num_line; j++)
	{
		for (k = 0; k < Point_image; k++)
		{
			distance = _sadd(tx_rx_distance[(_abs(k - 1))][j], tx_rx_distance[(_abs(k - 3))][j]);
			ready_x[coreNum][j * Point_image + k] = (short)x[2 * distance];
			ready_y[coreNum][j * Point_image + k] = (short)x[2 * distance + 1];
			// ready_x[coreNum][j*Point_image+k] = Hilbert_x[coreNum][distance];
			// ready_y[coreNum][j*Point_image+k] = Hilbert_y[coreNum][distance];
		}
	}

	//-----------------------------------TFM----------------------------------------------------------------------
}

void gen_twiddle_fft_sp_hxl(float *w, int n)
{
	int i, j, k;
	double x_t, y_t, theta1, theta2, theta3;
	const double PI = 3.141592654;

	for (j = 1, k = 0; j <= n >> 2; j = j << 2)
	{
		for (i = 0; i<n>> 2; i += j)
		{
			theta1 = 2 * PI * i / n;
			y_t = cos(theta1);
			x_t = sin(theta1);
			w[k] = (float)x_t;
			w[k + 1] = (float)y_t;

			theta2 = 4 * PI * i / n;
			y_t = cos(theta2);
			x_t = sin(theta2);
			w[k + 2] = (float)x_t;
			w[k + 3] = (float)y_t;

			theta3 = 6 * PI * i / n;
			y_t = cos(theta3);
			x_t = sin(theta3);
			w[k + 4] = (float)x_t;
			w[k + 5] = (float)y_t;
			k += 6;
		}
	}
}

void gen_twiddle_fft_sp_ifft(float *w, int n)
{
	int i, j, k;
	double x_t, y_t, theta1, theta2, theta3;
	const double PI = 3.141592654;

	for (j = 1, k = 0; j <= n >> 2; j = j << 2)
	{
		for (i = 0; i<n>> 2; i += j)
		{
			theta1 = 2 * PI * i / n;
			y_t = cos(theta1);
			x_t = -sin(theta1);
			w[k] = (float)x_t;
			w[k + 1] = (float)y_t;

			theta2 = 4 * PI * i / n;
			y_t = cos(theta2);
			x_t = -sin(theta2);
			w[k + 2] = (float)x_t;
			w[k + 3] = (float)y_t;

			theta3 = 6 * PI * i / n;
			y_t = cos(theta3);
			x_t = -sin(theta3);
			w[k + 4] = (float)x_t;
			w[k + 5] = (float)y_t;
			k += 6;
		}
	}
}

/******************************************************************************************************************
 * 该函数用于：根据滤波有关参数的地址标签，对不同的滤波参数进行编号和索引的映射：
 *       地址(DW) 		对应编号      				        参数
 * (1) 0x0000,0000,		  0					滤波器系数组1
 * (2) 0x0000,0041,		  1					滤波器系数组2
 * (3) 0x0000,0082,		  2					滤波器系数组3
 * (4) 0x0000,00C3,		  3					滤波器系数组4
 * (5) 0x0000,0200,		  4					滤波器索引表
 * ***************************************************************************************************************/
unsigned fir_addrToIndex(unsigned addr)
{
	unsigned index;
	if (addr < ADDR_INDEX_FIR) //hxl:读取滤波器系数参数
	{
		addr = (addr - ADDR_FIR0); //hxl:DW偏移
		if (addr < LEN_FIR_DW)	 //hxl:滤波器组0
		{
			index = 0;
		}
		else if (addr < 2 * LEN_FIR_DW) //hxl:滤波器组1
		{
			index = 1;
		}
		else if (addr < 3 * LEN_FIR_DW) //hxl:滤波器组2
		{
			index = 2;
		}
		else //hxl:滤波器组3
		{
			index = 3;
		}
	}
	else //hxl:滤波器索引表
	{

		index = 4;
	}
	return index;
}
