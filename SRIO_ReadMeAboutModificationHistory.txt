/*2016-1-04 ���ĵ���¼������LoopbackDioIsrexampleproject�Ļ����������ĸĶ�*/
/*һ���Ķ�--------------------------------------------------*/
    1.SRIO��ʼ�������ĸĶ���
                 ����port rate Ϊ1.25G
                ���ö˿�ģʽΪnormal
    2.�Ķ���dstID��ΪFPGA��ID(0xFF);DIO dst address(LSB):0x100000;dio.idSize-->0,ʹ��8 bit ID
	3.�����Ķ���data buf�ĳ�ʼ����дFPGA���ٻض������ݵ���֤����дΪ������ʽ��
	4.�Ķ�����Ӻ���dioSockets_SW_DoorbellIsrTest1(Srio_DrvHandle hSrioDrv)�ͱ�дmyDoorblComeIsr()��
	Ŀ���ǣ�Ϊ�˽���doorbell ISR test in DSP loopback mode������Ӧ��(����FPGA������������)
	5.�Ķ������dioSockets_SW_withDoorbl()������������FPGA swrite��������
/*�������ڵ�����--------------------------------------------------*/
	1.����:�� SIZE_DIO_PACKET=4ʱ����Srio_sockSend()��num_bytes��Ϊ4ʱ�����ͳ���completetion code Ϊ0b00.
	2.2016-2-26 �� FPGA�˷���doorbell��DSP�ˣ�DSP���ж���Ӧ�ٶ���FPGA�˷����ٶȲ������
	FPGA����Swriteһ���ߵ�ʱ����1s��СΪ900usʱ�����ݴ���������(DSP���ж��������һ��buf�ռ�ʱ��ǡFPGAд��buf)��
	��������2016-4-13ǰ�����������������ĸĶ���
	(1)ȥ���˳�����������е��õļ���sprintf������(sprintf�ĵ��õ���DSP���ж���Ӧ�ٶ���FPGA�˷����ٶȲ�������Ʋ����)��
	(2)�����Ӳ����ʼ������EVM_init()�����ڳ�ʼ��EVM���ϵ�6678��Main PLLʱ�ӣ�����system reset��core0��cycle��Ϊ100M��
/*������ʵ�ֵĹ���--------------------------------------------------*/
1. �Ի�ģʽ�£���������128��256�ֽڵİ�������һ���ߵ����ݣ���������ɺ󣬷���doorbell���ж�dsp��
   src address��Core0��L2Sram�� Ŀ�ĵ�ַ��0x0c010000.
�μ�������static Int32 dioSockets_SW_DoorbellIsrLoopbackTest(Srio_DrvHandle hSrioDrv);(�ú�����Ҫ���ڲ���)
2.2016-2-23:FPGA Swrite DSP  FPGA Swrite DSP��ÿ1s����һ����ʱ��DSP��������10����ʱ���ɹ�
  2016-4-13:FPGA Swrite DSP, ÿ1ms����һ����(16384x2 Bytes)ʱ��DSP��������100����ʱ�����ݽ��ճɹ���������100���ߺ󣬼��бȽϽ��������Ƿ�����֪Դ������ͬ
  2016-4-20:FPGA Swrite DSP, ÿ1ms����һ����ʱ��DSP��������3000����ʱ�����ݽ��ճɹ�.ÿ������һ���ߣ�ʵʱ�Ƚϸ������ݣ��Ƚ���ɺ��ٽ�����һ���ߣ��ٱȽϡ�
�μ�����static Int32 dioSockets_SW_DoorbellIsr_test1(Srio_DrvHandle hSrioDrv);(�ú�����Ҫ���ڲ���)
2.2016-4-21 �Ի�ģʽ�£��ȷ�doorbell��Ȼ���ٷ��Ͳ���������0x0c010000,SWRITE��(ԭ��swrite���ɹ�������Ϊ���������һ�����ڴ���Ϣ����driver�йص���Ϣ),
���������ݺ��ٴη���doorbell ֪ͨ���ݷ������
�μ�����static Int32 dioSockets_SW_withDoorbl(Srio_DrvHandle hSrioDrv);(�ú���������ʵ��Ӧ����Ƶĺ���)

/*=========2016-6-27====������Դ��SRIO_App2FPGA_FromLoopbackDioIsrExamp��copy����ֹ��2016-6-27������ʷ��¼��SRIO_App2FPGA_FromLoopbackDioIsrExamp��ȫ��ͬ===================================*/	
�ġ����ڵ����⣺
1.��Ҫ��һ��ȷ��FPGA�˾���Ķ�д��ַ
2.��doorbell�ж��ж�ȡ������ʱ������16384�������ᵼ�¶��ߣ��ڸ���2048������PC����������ȶ��������Բ����ߡ�
3.��ǰ״̬�£���SRIO NWRITE��NREADʱ����ʱ����ʾ���������isr��320000circles�ڣ�û����Ӧ��