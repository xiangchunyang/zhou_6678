/*2016-1-04 本文档记录在例程LoopbackDioIsrexampleproject的基础上所做的改动*/
/*一。改动--------------------------------------------------*/
    1.SRIO初始化函数的改动：
                 设置port rate 为1.25G
                设置端口模式为normal
    2.改动：dstID设为FPGA的ID(0xFF);DIO dst address(LSB):0x100000;dio.idSize-->0,使用8 bit ID
	3.其他改动：data buf的初始化及写FPGA后再回读的数据的验证，改写为函数形式。
	4.改动：添加函数dioSockets_SW_DoorbellIsrTest1(Srio_DrvHandle hSrioDrv)和编写myDoorblComeIsr()，
	目的是：为了进行doorbell ISR test in DSP loopback mode及正常应用(接收FPGA发送来的数据)
	5.改动：添加dioSockets_SW_withDoorbl()函数，用于向FPGA swrite参数数据
/*二。存在的问题--------------------------------------------------*/
	1.问题:当 SIZE_DIO_PACKET=4时，即Srio_sockSend()的num_bytes数为4时，发送出错，completetion code 为0b00.
	2.2016-2-26 日 FPGA端发送doorbell至DSP端，DSP端中断响应速度与FPGA端发送速度不相符；
	FPGA数据Swrite一根线的时间由1s减小为900us时，数据传输有问题(DSP端中断中清除下一个buf空间时，恰FPGA写该buf)。
	该问题于2016-4-13前解决。解决问题所做的改动：
	(1)去掉了程序或任务函数中调用的几处sprintf函数，(sprintf的调用导致DSP端中断响应速度与FPGA端发送速度不相符看似不相符)；
	(2)添加了硬件初始化函数EVM_init()，用于初始化EVM板上的6678的Main PLL时钟，以免system reset后core0的cycle降为100M。
/*三。已实现的功能--------------------------------------------------*/
1. 自环模式下，连续发送128个256字节的包，（即一根线的数据），发送完成后，发送doorbell并中断dsp。
   src address：Core0的L2Sram， 目的地址：0x0c010000.
参见函数：static Int32 dioSockets_SW_DoorbellIsrLoopbackTest(Srio_DrvHandle hSrioDrv);(该函数主要用于测试)
2.2016-2-23:FPGA Swrite DSP  FPGA Swrite DSP，每1s发送一根线时，DSP连续接收10根线时，成功
  2016-4-13:FPGA Swrite DSP, 每1ms发送一根线(16384x2 Bytes)时，DSP连续接收100根线时，数据接收成功。接收完100根线后，集中比较接收数据是否与已知源数据相同
  2016-4-20:FPGA Swrite DSP, 每1ms发送一根线时，DSP连续接收3000根线时，数据接收成功.每接收完一根线，实时比较该线数据，比较完成后再接收下一根线，再比较。
参见函数static Int32 dioSockets_SW_DoorbellIsr_test1(Srio_DrvHandle hSrioDrv);(该函数主要用于测试)
2.2016-4-21 自环模式下：先发doorbell，然后再发送参数数据至0x0c010000,SWRITE。(原来swrite不成功，是因为错误清除了一部分内存信息。与driver有关的信息),
发送完数据后，再次发送doorbell 通知数据发送完成
参见函数static Int32 dioSockets_SW_withDoorbl(Srio_DrvHandle hSrioDrv);(该函数是面向实际应用设计的函数)

/*=========2016-6-27====本工程源于SRIO_App2FPGA_FromLoopbackDioIsrExamp的copy，截止到2016-6-27上述历史记录与SRIO_App2FPGA_FromLoopbackDioIsrExamp完全相同===================================*/	
四。存在的问题：
1.需要进一步确认FPGA端具体的读写地址
2.在doorbell中断中读取线数据时，复制16384点数，会导致丢线；在复制2048点数后，PC软件启动后，稳定下来可以不丢线。
3.当前状态下，向SRIO NWRITE和NREAD时，有时会提示：发送完成isr在320000circles内，没有响应。