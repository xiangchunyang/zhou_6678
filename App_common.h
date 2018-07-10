/*============================================================
 * 2015-7-21
 * 公用的全局变量
 * ========================================================*/

/*------------与处理有关的头文件-----------------------*/
#define TOTALPOINTS 2048 //16384 //hxl:由FPGA发送来的原始数据线的长度.一根原始线的数据点数，每个数据占2个字节
/**************原始数据线4倍降采样****************************************/

#define TESTBYTES (TOTALPOINTS * sizeof(short) / 2) //TOTALPOINTS*sizeof(short)// // //hxl:发送回PC机的线数据点数是原始点数经过4倍降采样后的数据
#define POINTSAFTERPRO TOTALPOINTS / 2              //TOTALPOINTS        //(TESTSIZE/4) //TESTSIZE //                      //hxl:后处理后降采样之后，每根数据线的点数，每个点占2个字节

/**************原始数据线不降采样****************************************/
/*#define TESTBYTES TESTSIZE*sizeof(short)//hxl:发送回PC机的线数据点数是原始点数
#define POINTSAFTERPRO TESTSIZE //hxl:不做后处理，不降采样时，每根数据线的点数，每个点占2个字节
*/
#define numLines_LongImage 251 //hxl:一幅纵波图像包括的线数目
#define LINENUM_BUF 200        //需要缓存的线数目
#define BYTESPERTRAN TESTBYTES
#define MAXINT 2e31
#define ImageUP 1 //表示PC机命令：要求上传图像数据

//---------hxl 定义几个全局便量--------data和data0的分配是按照由大到小的顺序分配的，不是按照出现的先后顺序------------------

#define Num_line 400
#define Point_image 333
