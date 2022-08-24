/***************************�����Ƶ���****************************
**  �������ƣ�YS-V0.7����ʶ��ģ����������
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.7����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2013.9.13
**  ˵��������ģʽ�� ��ÿ��ʶ��ʱ����Ҫ˵��С�ܡ�������� �����ܹ�������һ����ʶ��
/***************************�����Ƶ���******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/

uint8 idata nAsrStatus=0;	
void MCU_init(); 
void ProcessInt0(); //ʶ��������
void delay(unsigned long uldata);
void User_handle(uint8 dat);//�û�ִ�в�������
void Delay200ms();
void Led_test(void);//��Ƭ������ָʾ
uint8_t G0_flag=DISABLE;//���б�־��ENABLE:���С�DISABLE:��ֹ���� 
sbit LED=P4^2;//�ź�ָʾ��

/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i=0;
	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*���ڳ�ʼ��*/
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	
	#ifdef TEST	
    PrintCom("һ�����С��\r\n"); /*text.....*/
	PrintCom("�������1���������\r\n"); /*text.....*/
	PrintCom("	2����������֤\r\n"); /*text.....*/
	PrintCom("	3������\r\n"); /*text.....*/
	PrintCom("	4���ص�\r\n"); /*text.....*/
	PrintCom("  5������\r\n"); /*text.....*/
	PrintCom("	6���Ϻ�\r\n"); /*text.....*/
	PrintCom("	7������\r\n"); /*text.....*/
	#endif

	while(1)
	{
		switch(nAsrStatus)
		{
			case LD_ASR_RUNING:
			case LD_ASR_ERROR:		
				break;
			case LD_ASR_NONE:
			{
				nAsrStatus=LD_ASR_RUNING;
				if (RunASR()==0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR���ӹؼ��������ASR����*/
				{
					nAsrStatus = LD_ASR_ERROR;
				}
				break;
			}
			case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
			{				
				nAsrRes = LD_GetResult();		/*��ȡ���*/
				User_handle(nAsrRes);//�û�ִ�к��� 
				nAsrStatus = LD_ASR_NONE;
				break;
			}
			case LD_ASR_FOUNDZERO:
			default:
			{
				nAsrStatus = LD_ASR_NONE;
				break;
			}
		}// switch	 			
	}// while

}
/***********************************************************
* ��    �ƣ� 	 LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
**********************************************************/
void Led_test(void)
{
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
}
/***********************************************************
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;
	

	LD_MODE = 0;		//	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0=1;
	EX0=1;
	EA=1;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void Delay200us()		//@22.1184MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 5;
	j = 73;
	do
	{
		while (--j);
	} while (--i);
}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	while(uldata--)
	Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/***********************************************************
* ��    �ƣ� �жϴ�������
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				
}
/***********************************************************
* ��    �ƣ��û�ִ�к��� 
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸� 
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
**********************************************************/
void User_handle(uint8 dat)
{ 
			switch(dat)		    /*对结果执行相关操作*/
			{
				case CODE_kysd:	  //命令“开浴室灯”
					PrintCom("kysd\r\n");  //串口发送"开浴室灯"指令
					break;
				case CODE_gysd:		//命令“关浴室灯”			
					PrintCom("gysd\r\n"); //串口发送"关浴室灯"指令
					break;
				case CODE_kwsd:		//命令“开卧室灯”				
					PrintCom("kwsd\r\n"); //串口发送“开卧室灯"指令
					break;
				case CODE_gwsd:		//命令“关卧室灯”				
					PrintCom("gwsd\r\n"); //串口发送“关卧室灯”指令
					break;
				case CODE_kktd:		//命令“开客厅灯”			
						PrintCom("kktd\r\n"); //串口发送“开客厅灯”指令
					break;
				case CODE_gktd:		//命令“关客厅灯”				
						PrintCom("gktd\r\n"); //串口发送开浴室灯指令
					break;
					case CODE_kctd:		//命令“开餐厅灯”			
						PrintCom("kctd\r\n"); //串口发送开浴室灯指令
					break;
					case CODE_gctd:		//命令“关餐厅灯”				
						PrintCom("gctd\r\n"); /*ͨ�����ڷ�������*/
					break;	
					case CODE_kqbd:		//命令“关浴室灯”				
						PrintCom("kqbd\r\n"); /*ͨ�����ڷ�������*/
													break;
					case CODE_gqbd:		//命令“关浴室灯”				
						PrintCom("gqbd\r\n"); /*ͨ�����ڷ�������*/
													break;	
					case CODE_kycd:		/*�����Ӿ�صơ�*/				
						PrintCom("kycd\r\n"); /*ͨ�����ڷ�������*/
													break;	
					case CODE_gycd:		/*�����Ӿ�صơ�*/				
						PrintCom("gycd\r\n"); /*ͨ�����ڷ�������*/
													break;
					case CODE_ks:		/*���������*/				
						PrintCom("ks\r\n"); /*ͨ�����ڷ�������*/
													break;	
					case CODE_kdfs:		/*���������ȡ�*/				
						PrintCom("kdfs\r\n"); /*ͨ�����ڷ�������*/
													break;	
					case CODE_gdfs:		/*����ص���ȡ�*/				
						PrintCom("gdfs\r\n"); /*ͨ�����ڷ�������*/
													break;																																																																									
							default:PrintCom("������ʶ�𷢿���\r\n"); /*text.....*/break;
				}	
		/*	}	
			else 	
			{
				PrintCom("��˵��һ������\r\n"); /*text.....*/	
		/*	}	*/
}	 
